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
 * @brief Inter-process communications.
 * @copyright Copyright (c) 2022 Pierre Boisselier All rights reserved.
 */

#ifndef SKIRT_IPC_H
#define SKIRT_IPC_H

#include <sk/types.h>
#include <sk/task.h>
#include <sk/arch.h>

typedef void(sk_sig_handler)(int);

#ifdef SKIRT_KERNEL

/* TODO: Add a task queue of tasks waiting for a semaphore, this would improve perfs. */
typedef struct sk_sem {
	volatile int counter;
#ifdef SKIRT_ALLOC_STATIC
	char flag;
#endif /* SKIRT_ALLOC_STATIC */
} sk_sem;

typedef struct sk_sig {
	int signo;
	sk_task *waiting_tasks;
} sk_sig;

typedef struct sk_mail {
	const void *msg;
	sk_task *task;
	struct sk_mail *next;
} sk_mail;

#else
typedef struct sk_sem sk_sem;
typedef struct sk_mail sk_mail;
#endif /* SKIRT_KERNEL */

/**************
 * Semaphores *
 **************/

/**
 * Create a counting semaphore type for synchronizing tasks.
 * @param initial_value Initial value stored in the counting semaphore.
 * @return Pointer to newly created semaphore object (or NULL on error).
 */
extern sk_sem *sk_sem_create(int initial_value);
/**
 * Destroy a previously created counting semaphore.
 * @param sem Semaphore to destroy.
 */
extern void sk_sem_destroy(sk_sem *sem);
/**
 * Increments the internal counter and unblocks acquirers.
 * @param sem Semaphore to operate on.
 */
extern void sk_sem_release(sk_sem *sem);
/**
 * Decrements the internal counter or blocks until it can.
 * @param sem Semaphore to operate on.
 */
extern void sk_sem_acquire(sk_sem *sem);
/**
 * Tries to decrement the internal counter without blocking.
 * @param sem Semaphore to operate on.
 * @return True if could acquire it, false otherwise.
 * @note This will not yield to other tasks during a call.
 */
extern bool sk_sem_try_acquire(sk_sem *sem);

/*************
 * Signals   *
 *************/
extern void sk_sig_send(int sig);
extern void sk_sig_register(int sig, sk_sig_handler handler);

/****************
 * Mail & Boxes *
 ****************/
/**
  * @brief Send a mail to another task (FiFo).
  * @param task Recipient task.
  * @param msg Pointer to data.
  * @return True if mail was sent, False if not.
  */
extern bool sk_mail_send_to(sk_task *task, const void *msg);
/**
 * @brief Check if a mail is available.
 * @return True if there's a new mail, false otherwise.
 */
extern bool sk_mail_available(void);
/**
 * @brief Retrieve queued mail (FiFo).
 * @return Mail's content.
 * @note Once a mail is read, it is deleted.
 * @warning This does not do a copy, only pointers are exchanged!
 */
extern const void *sk_mail_pickup(void);

#endif /* SKIRT_IPC_H */
