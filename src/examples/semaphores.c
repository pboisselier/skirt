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
 * @brief Semaphore Examples with SKIRT
 * @copyright Copyright (c) 2022 Pierre Boisselier All rights reserved.
 *
 * This examples provides some directions on how to use semaphores using the SKIRT library.
 * It counts from 0 to 9 using 3 tasks, this should always be in the same order.
 */

/* Contains functions starting with sk_task */
#include <sk/task.h>
/* Semaphore are IPCs. */
#include <sk/ipc.h>
/* For sending data on serial port. */
#include <sk/serial.h>

sk_stack_t stack1[SKIRT_TASK_STACK_SZ];
sk_stack_t stack2[SKIRT_TASK_STACK_SZ];
sk_stack_t stack3[SKIRT_TASK_STACK_SZ];

sk_task *t1 = NULL;
sk_task *t2 = NULL;
sk_task *t3 = NULL;

sk_sem *s1 = NULL;
sk_sem *s2 = NULL;
sk_sem *s3 = NULL;

/* You can use this ugly delay function to check if semaphores are working correctly.
 * Preemption and context switching will surely happen during this function but numbers will
 * always be printed in the right order thanks to semaphores.
 */
static inline void ugly_delay(void)
{
	for (volatile unsigned long i = 0; i < 100000; ++i) {
		sk_arch_nop();
	}
}

/* Counts from 0 to 9 by increment of 3. */
void func_t1(void)
{
	for (char i = 0; i < 10; i += 3) {
		sk_sem_acquire(s1);
		sk_serial_putc(i + '0');
		sk_task_sleep(100);
		sk_sem_release(s2);
	}
	sk_serial_print("\n\rHello :)\n\r");
	sk_task_exit();
}

/* Counts from 1 to 7 by increment of 3. */
void func_t2(void)
{
	for (char i = 1; i < 10; i += 3) {
		sk_sem_acquire(s2);
		sk_serial_putc(i + '0');
		sk_task_sleep(100);
		sk_sem_release(s3);
	}
	sk_task_exit();
}

/* Counts from 2 to 8 by increment of 3. */
void func_t3(void)
{
	for (char i = 2; i < 10; i += 3) {
		sk_sem_acquire(s3);
		sk_serial_putc(i + '0');
		sk_task_sleep(100);
		ugly_delay();
		sk_sem_release(s1);
	}
	sk_task_exit();
}

int main(void)
{
	/* If SKIRT_HARD_PRIO is not set, priority does not matter. */
	/* Tasks are not created in order to ensure semaphores are working properly. */
	t2 = sk_task_create_static(func_t2, 1, stack2, sizeof stack2);
	t3 = sk_task_create_static(func_t3, 2, stack3, sizeof stack3);
	t1 = sk_task_create_static(func_t1, 3, stack1, sizeof stack1);

	/* You can decide which task "starts" first by adding a 1 in its corresponding semaphore. */
	s1 = sk_sem_create(1);
	s2 = sk_sem_create(0);
	s3 = sk_sem_create(0);

	/* Start kernel. */
	sk_kernel_start();

	/* Never reached. */
}
