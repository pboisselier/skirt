/*
Copyright or © or Copr. Pierre Boisselier (30 nov. 2022)

skirt@pboisselier.fr

This software is a computer program whose purpose is to [describe
functionalities and technical features of your software].

This software is governed by the CeCILL license under French law and
abiding by the rules of distribution of free software.  You can  use,
modify and/ or redistribute the software under the terms of the CeCILL
license as circulated by CEA, CNRS and INRIA at the following URL
"http://www.cecill.info".

As a counterpart to the access to the source code and  rights to copy,
modify and redistribute granted by the license, users are provided only
with a limited warranty  and the software's author,  the holder of the
economic rights,  and the successive licensors  have only  limited
liability.

In this respect, the user's attention is drawn to the risks associated
with loading,  using,  modifying and/or developing or reproducing the
software by the user in light of its specific status of free software,
that may mean  that it is complicated to manipulate,  and  that  also
therefore means  that it is reserved for developers  and  experienced
professionals having in-depth computer knowledge. Users are therefore
encouraged to load and test the software's suitability as regards their
requirements in conditions enabling the security of their systems and/or
data to be ensured and,  more generally, to use and operate it in the
same conditions as regards security.

The fact that you are presently reading this means that you have had
knowledge of the CeCILL license and that you accept its terms.
*/

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
			task_pool[i].mailbox = NULL;
			task_pool[i].priority = 0;
			task_pool[i].stack_sz = 0;
			task_pool[i].counter.since_creation = 0;
			task_pool[i].counter.waiting = 0;
			task_pool[i].counter.running = 0;
			task_pool[i].counter.sleeping = 0;
			task_pool[i].counter.ready = 0;
			return &task_pool[i];
		}
	}

	return NULL;
}

static inline void sk_task_free(sk_task *task)
{
	SK_ASSERT(task);
	if (task < task_pool || task > &task_pool[SKIRT_TASK_MAX - 1]) {
		SK_PANIC("Provided task is not from the static pool!\n\r");
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
		tmp->counter.since_creation++;
		switch (tmp->state) {
		case RUNNING:
			tmp->counter.running++;
			break;
		case WAITING:
			tmp->counter.waiting++;
			break;
		case SLEEPING:
			tmp->counter.sleeping--;
			if (tmp->counter.sleeping == 0) {
				tmp->state = READY;
			}
			break;
		case READY:
			tmp->counter.ready++;
			break;
		}
		tmp = tmp->next;
	}
}

#ifdef SKIRT_HARD_PRIO
/* Always yield to the highest priority READY'd task. */
static inline SK_HOT sk_task *sk_task_find_ready(void)
{
	sk_task *elected = task_head;
	sk_task *tmp = task_head;
	while (tmp) {
		if (tmp->state == READY &&
		    (tmp->priority > elected->priority)) {
			elected = tmp;
		}
		tmp = tmp->next;
	}

	return elected;
}

#else
/* Simple round-robin. */
static inline SK_HOT sk_task *sk_task_find_ready(void)
{
	sk_task *tmp = task_current;
	while (tmp->next) {
		tmp = tmp->next;
		if (tmp->state == READY) {
			return tmp;
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
#endif /* SKIRT_HARD_PRIO */

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
	next->counter.ready = 0;
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
