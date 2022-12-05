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
 * @brief Mails & Boxes examples with SKIRT
 * @copyright Copyright (c) 2022 Pierre Boisselier All rights reserved.
 *
 * This examples provides some directions on how to use mails to pass pointers between tasks.
 * A task is sending mails to another one which will print them on the serial port.
 */

/* Contains functions starting with sk_task */
#include <sk/task.h>
/* Mails are IPCs. */
#include <sk/ipc.h>
/* For sending data on serial port. */
#include <sk/serial.h>

sk_stack_t stack1[SKIRT_TASK_STACK_SZ];
sk_stack_t stack2[SKIRT_TASK_STACK_SZ];

sk_task *t1 = NULL;
sk_task *t2 = NULL;

void func_t1(void)
{
	sk_mail_send_to(t2, "Hello!\n\r");
	sk_mail_send_to(t2, "This mail was sent by T1 :)\n\r");

	sk_serial_print("T1 finished sending to T2\n\r");
	sk_task_exit();
}

void func_t2(void)
{
	while (1) {
		if (sk_mail_available()) {
			sk_serial_print(sk_mail_pickup());
		}
	}
}

int main(void)
{
	/* If SKIRT_HARD_PRIO is not set, priority does not matter. */
	t1 = sk_task_create_static(func_t1, 2, stack1, sizeof stack1);
	t2 = sk_task_create_static(func_t2, 1, stack2, sizeof stack2);

	/* Start kernel. */
	sk_kernel_start();

	/* Never reached. */
}
