/**
 * @brief Task management.
 * @copyright Copyright (c) 2022 Pierre Boisselier All rights reserved.
 */

#ifndef SKIRT_TASK_H
#define SKIRT_TASK_H

#include <sk/skirt.h>
#include <sk/types.h>

typedef void (*sk_task_func)(void);

#ifdef SKIRT_KERNEL

#ifdef SKIRT_ALLOC_STATIC

#ifndef SKIRT_PREEMPT_TIME
#define SKIRT_PREEMPT_TIME 512
#endif /* SKIRT_PREEMPT_TIME */

#ifndef SKIRT_TASK_MAX
#define SKIRT_TASK_MAX 5
#endif /* SKIRT_TASK_MAX */

#endif /* SKIRT_ALLOC_STATIC*/

/* Forward declaration for Mail structure */
typedef struct sk_mail sk_mail;

typedef enum sk_state { RUNNING, READY, WAITING, SLEEPING } sk_state;

typedef struct sk_counter {
	sk_size_t since_creation;
	sk_size_t running;
	sk_size_t waiting;
	sk_size_t sleeping;
} sk_counter;

typedef struct sk_task {
	sk_stack_t *stack;
	sk_stack_t *sp;
	sk_size_t stack_sz;
	sk_state state;
	sk_counter counter;
	short priority;
	struct sk_task *next;
	sk_mail* mailbox;
} sk_task;

/**
 * @brief Change which process is currently running.
 */
extern void sk_task_switch(void);

#else /* Provide hidden definitions. */
typedef struct sk_task sk_task;
#endif /* SKIRT_KERNEL */

#ifndef SKIRT_TASK_STACK_SZ
#define SKIRT_TASK_STACK_SZ 48
#endif /* SKIRT_TASK_STACK_SZ */

/**
 * @brief Create a task with a static stack.
 * @param func Task function.
 * @param priority Task priority.
 * @param stack Allocated memory where the stack will be stored.
 * @param stack_size Size of the stack.
 * @return Pointer to created task.
 * @note All tasks should be created before starting kernel.
 */
extern sk_task *sk_task_create_static(sk_task_func, short priority, void *stack,
				      sk_size_t stack_sz);

/**
 * @brief Kill a running task.
 * @param task Task to be killed
 */
extern void sk_task_kill(sk_task *task);

/**
 * @brief Exit current task.
 */
extern void sk_task_exit(void);

/**
 * @brief Put current task to sleep for at least time_ms.
 * @param time_ms Minimum time in ms.
 */
extern void sk_task_sleep(sk_size_t time_ms);

/**
 * @brief Wake-up a waiting task.
 * @param task Task in WAITING state.
 */
extern void sk_task_awake(sk_task *task);
/**
 * @brief Put current task (caller) in WAITING state.
 * @note Only "sk_task_awake" will change its state!
 */
extern void sk_task_await(void);

#endif /* SKIRT_TASK_H */
