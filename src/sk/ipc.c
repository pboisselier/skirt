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

#include <sk/ipc.h>

/* TODO: Find a prettier way to retrieve calling task. */
extern sk_task *task_current;

#ifdef SKIRT_ALLOC_STATIC

#ifndef SKIRT_SEM_MAX
#define SKIRT_SEM_MAX SKIRT_TASK_MAX
#endif /* SKIRT_SEM_MAX */

static sk_sem sem_pool[SKIRT_SEM_MAX] = { 0 };

static inline sk_sem *sk_sem_alloc(int initial_value)
{
	for (sk_size_t i = 0; i < SKIRT_TASK_MAX; ++i) {
		if (sem_pool[i].flag == 0) {
			sem_pool[i].counter = initial_value;
			sem_pool[i].flag = 1;
			return &sem_pool[i];
		}
	}

	return NULL;
}

static inline void sk_sem_free(sk_sem *sem)
{
	SK_ASSERT(sem);
	if (sem < sem_pool || sem > &sem_pool[SKIRT_TASK_MAX - 1]) {
		SK_PANIC("Provided sem is not from the static pool!\n\r");
	}
	sem->counter = 0;
	sem->flag = 0;
}

#ifndef SKIRT_MAIL_MAX
#define SKIRT_MAIL_MAX SKIRT_TASK_MAX
#endif /* SKIRT_MAIL_MAX */

static sk_mail mail_pool[SKIRT_MAIL_MAX] = { 0 };

static inline sk_mail *sk_mail_alloc(const void *msg)
{
	for (sk_size_t i = 0; i < SKIRT_TASK_MAX; ++i) {
		if (mail_pool[i].task == NULL) {
			mail_pool[i].next = NULL;
			mail_pool[i].msg = msg;
			return &mail_pool[i];
		}
	}

	return NULL;
}

static inline void sk_mail_free(sk_mail *mail)
{
	SK_ASSERT(mail);
	if (mail < mail_pool || mail > &mail_pool[SKIRT_TASK_MAX - 1]) {
		SK_PANIC("Provided mail is not from the static pool!\n\r");
	}
	mail->msg = NULL;
	mail->task = NULL;
	mail->next = NULL;
}

#else
#error "FIXME: Only SKIRT_ALLOC_STATIC is supported!"
#endif /* SKIRT_ALLOC_STATIC */

sk_sem *sk_sem_create(int initial_value)
{
	sk_arch_disable_int();
	if (initial_value < 0) {
		initial_value = 0;
	}
	sk_sem *sem = sk_sem_alloc(initial_value);
	SK_ASSERT(sem);
	sk_arch_enable_int();
	return sem;
}

void sk_sem_destroy(sk_sem *sem)
{
	if (!sem) {
		return;
	}

	sk_arch_disable_int();
	sk_sem_free(sem);
	sk_arch_enable_int();
}

void sk_sem_release(sk_sem *sem)
{
	if (!sem) {
		return;
	}
	sk_arch_disable_int();
	sem->counter++;
	sk_arch_enable_int();
}

void sk_sem_acquire(sk_sem *sem)
{
	if (!sem) {
		return;
	}
	sk_arch_disable_int();

	while (sem->counter == 0) {
		sk_arch_disable_int();
		sk_arch_yield();
	}
	sem->counter--;
	sk_arch_enable_int();
}

bool sk_sem_try_acquire(sk_sem *sem)
{
	sk_arch_disable_int();
	if (sem->counter == 0) {
		sk_arch_enable_int();
		return false;
	}
	sem->counter--;
	sk_arch_enable_int();
	return true;
}

bool sk_mail_send_to(sk_task *task, const void *msg)
{
	sk_arch_disable_int();
	SK_ASSERT(task);

	sk_mail *mail = sk_mail_alloc(msg);
	if (!mail) {
		sk_arch_enable_int();
		return false;
	}

	mail->task = task;

	if (!task->mailbox) {
		task->mailbox = mail;
	} else {
		sk_mail *tmp = task->mailbox;
		while (tmp->next) {
			tmp = tmp->next;
		}
		tmp->next = mail;
	}

	return true;
	sk_arch_enable_int();
}

/* O0 "optimization" forced to prevent a read on task_current after enabling
 * interrupts. */
SK_NOOPTI bool sk_mail_available(void)
{
	sk_arch_disable_int();
	SK_ASSERT(task_current);
	volatile bool tmp = (task_current->mailbox != NULL);
	sk_arch_enable_int();
	return tmp;
}
const void *sk_mail_pickup(void)
{
	sk_arch_disable_int();
	if (!task_current->mailbox) {
		sk_arch_enable_int();
		return NULL;
	}

	const void *msg = task_current->mailbox->msg;
	sk_mail *tmp = task_current->mailbox;
	task_current->mailbox = task_current->mailbox->next;
	sk_mail_free(tmp);
	sk_arch_enable_int();

	return msg;
}
