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
