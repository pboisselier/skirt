/**
 * @brief Simple test.
 * @copyright Copyright (c) 2022 Pierre Boisselier All rights reserved.
 */

#include <sk/skirt.h>
#include <sk/task.h>
#include <sk/serial.h>
#include <sk/ipc.h>

sk_stack_t stack1[SKIRT_TASK_STACK_SZ];
sk_stack_t stack2[SKIRT_TASK_STACK_SZ];

sk_task *t1 = NULL;
sk_task *t2 = NULL;

sk_sem *s;

void task1(void)
{
	sk_serial_print("Task1 waiting for T2\n");
	sk_sem_acquire(s);
	sk_sem_acquire(s);
	sk_serial_print("Task1 set to wait!\n");
	sk_task_sleep(10000);
	sk_serial_print("Task1 stopped waiting :)\n");
	sk_mail_send_to(t2, "Hello!\n");
	sk_task_awake(t2);
	sk_serial_print("Task1 woke up Task2 (goodbye!) :)\n");
	sk_task_exit();
}

SK_NORETURN void task2(void)
{
	sk_serial_print("Task2 waiting\n");
	sk_task_sleep(1000);
	sk_serial_print("Task2 sem\n");
	sk_sem_release(s);
	sk_task_sleep(100);
	sk_serial_print("Task2 sem (x2)\n");
	sk_sem_release(s);
	sk_serial_print("Task2 awaiting a wake-up...\n");
	sk_task_await();
	if (sk_mail_available()) {
		const char *msg = sk_mail_pickup();
		sk_serial_print(msg);
	}
	sk_serial_print("Task2 Running...\n");
	for (;;)
		;
}

int main(void)
{
	/* Using serial output before kernel initialization requires to manually call init. */
	sk_serial_init();
	sk_serial_print("Hello, world!\n");

	/* Create tasks. */
	t1 = sk_task_create_static(task1, 0, stack1, sizeof stack1);
	t2 = sk_task_create_static(task2, 1, stack2, sizeof stack2);
	s = sk_sem_create(0);

	/* Start kernel. */
	sk_kernel_start();

	/* This will never be reached. */
	return 0;
}
