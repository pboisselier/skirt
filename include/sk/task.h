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
 * @brief Task management.
 * @copyright Copyright (c) 2022 Pierre Boisselier All rights reserved.
 */

#ifndef SKIRT_TASK_H
#define SKIRT_TASK_H

#include <sk/skirt.h>
#include <sk/types.h>

typedef void (*sk_task_func)(void);

#ifdef SKIRT_KERNEL

#ifndef SKIRT_PREEMPT_TIME
#define SKIRT_PREEMPT_TIME 512
#endif /* SKIRT_PREEMPT_TIME */

#ifdef SKIRT_ALLOC_STATIC

#ifndef SKIRT_TASK_MAX
#define SKIRT_TASK_MAX 5
#endif /* SKIRT_TASK_MAX */

#ifndef SKIRT_TASK_STACK_SZ
#define SKIRT_TASK_STACK_SZ 48
#endif /* SKIRT_TASK_STACK_SZ */

#endif /* SKIRT_ALLOC_STATIC*/

/* Forward declaration for Mail structure */
typedef struct sk_mail sk_mail;

typedef enum sk_state { RUNNING, READY, WAITING, SLEEPING } sk_state;

typedef struct sk_counter {
	/* Those counters are never reset. */
	sk_size_t since_creation;
	sk_size_t running;
	sk_size_t waiting;

	/* Those counters are reset when changing state. */
	sk_size_t sleeping;
	sk_size_t ready;
} sk_counter;

typedef struct sk_task {
	sk_stack_t *stack;
	sk_stack_t *sp;
	sk_size_t stack_sz;
	sk_state state;
	sk_counter counter;
	short priority;
	struct sk_task *next;
	sk_mail *mailbox;
} sk_task;

/**
 * @brief Change which process is currently running.
 */
extern void sk_task_switch(void);

#else /* Provide hidden definitions. */
typedef struct sk_task sk_task;
#endif /* SKIRT_KERNEL */

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
