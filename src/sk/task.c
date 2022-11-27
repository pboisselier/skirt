/**
 * @brief Task management, scheduling happens here.
 * @note See sk/arch/<arch>.c for specifics on context switching.
 * @copyright Copyright (c) 2022 Pierre Boisselier All rights reserved.
 */
#include <sk/task.h>
#include <sk/arch.h>

sk_task *volatile task_head;
sk_task *volatile task_current;

#ifdef SKIRT_ALLOC_STATIC
static sk_task task_pool[SKIRT_TASK_MAX] = { 0 };

static inline sk_task *sk_task_alloc(void)
{
	for (sk_size_t i = 0; i < SKIRT_TASK_MAX; ++i) {
		if (task_pool[i].stack == NULL) {
			task_pool[i].next = NULL;
			task_pool[i].sp = NULL;
			task_pool[i].priority = 0;
			task_pool[i].stack_sz = 0;
			task_pool[i].counter.since_creation = 0;
			task_pool[i].counter.waiting = 0;
			task_pool[i].counter.running = 0;
			task_pool[i].counter.sleeping = 0;
			return &task_pool[i];
		}
	}

	return NULL;
}

static inline void sk_task_free(sk_task *task)
{
	SK_ASSERT(task);
	if (task < task_pool || task > &task_pool[SKIRT_TASK_MAX - 1]) {
		SK_PANIC("Provided task is not from the static pool!\n");
	}
	task->stack = NULL;
}
#else
#error "Only SKIRT_ALLOC_STATIC is currently supported!"
#endif

/**
 * @brief Prepend a task to the list.
 * @param task Task to prepend.
 */
static inline void sk_task_prepend(sk_task *task)
{
	if (!task_head) {
		task_head = task;
		return;
	}
	task->next = task_head;
	task_head = task;
}

static inline void sk_task_remove(sk_task *task)
{
	SK_ASSERT(task_head);

	if (task == task_head) {
		task_head = task_head->next;
		return;
	}

	sk_task *tmp = task_head;
	while (tmp->next) {
		if (tmp->next == task) {
			tmp->next = task->next;
			return;
		}
		tmp = tmp->next;
	}
}

static inline void sk_task_update_counters(void)
{
	sk_task *tmp = task_head;
	while (tmp) {
		tmp->counter.since_creation += SKIRT_PREEMPT_TIME;
		switch (tmp->state) {
		case RUNNING:
			tmp->counter.running += SKIRT_PREEMPT_TIME;
			break;
		case WAITING:
			tmp->counter.waiting += SKIRT_PREEMPT_TIME;
			break;
		case SLEEPING:
			tmp->counter.sleeping--;
			if (tmp->counter.sleeping == 0) {
				tmp->state = READY;
			}
			break;
		case READY:
			break;
		}
		tmp = tmp->next;
	}
}

/* TODO: Use priority-based scheduling and make it real-time. */
static inline SK_HOT sk_task *sk_task_find_ready(void)
{
	sk_task *elected = task_current;
	sk_task *tmp = task_current;
	while (tmp->next) {
		tmp = tmp->next;
		if (tmp->state == READY) {
			return tmp;
			//if (tmp->priority > elected->priority) {
			//	elected = tmp;
			//}
		}
	}

	tmp = task_head;
	do {
		if (tmp->state == READY) {
			return tmp;
		}
		tmp = tmp->next;
	} while (tmp->next);

	return NULL;
}

void sk_task_switch(void)
{
	sk_task_update_counters();

	if (!task_current) {
		task_current = task_head;
	}

	if (task_current->state == RUNNING) {
		task_current->state = READY;
	}

	sk_task *next = sk_task_find_ready();
	SK_ASSERT(next);

	next->state = RUNNING;
	task_current = next;
}

// ------------------
// Exported symbols
// ------------------

sk_task *sk_task_create_static(sk_task_func func, short priority, void *stack,
			       sk_size_t stack_sz)
{
	sk_task *task = sk_task_alloc();
	SK_ASSERT(task);
	sk_task_prepend(task);

	task->stack = stack;
	task->stack_sz = stack_sz;
	task->priority = priority;
	task->state = READY;

	sk_arch_stack_init(func, task);


	return task;
}
void sk_task_kill(sk_task *task)
{
	sk_arch_disable_int();
	sk_task_remove(task);
	sk_task_free(task);
	sk_arch_enable_int();
}
void sk_task_exit(void)
{
	sk_arch_disable_int();

	sk_task_remove(task_current);
	sk_task_free(task_current);

	sk_arch_yield();

	SK_VERIFY_NOT_REACHED();
}
void sk_task_sleep(sk_size_t time_ms)
{
	sk_arch_disable_int();
	SK_ASSERT(task_current);
	/* TODO: Convert time_ms to nearest number of interrupts required. */
	task_current->counter.sleeping = time_ms;
	task_current->state = SLEEPING;
	sk_arch_yield();
}

void sk_task_awake(sk_task *task)
{
	sk_arch_disable_int();
	SK_ASSERT(task);
	task->state = READY;
	sk_arch_enable_int();
}

void sk_task_await(void)
{
	sk_arch_disable_int();
	SK_ASSERT(task_current);
	task_current->state = WAITING;
	sk_arch_yield();
}
