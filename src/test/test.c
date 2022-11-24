/**
 * @brief Simple test.
 * @copyright Copyright (c) 2022 Pierre Boisselier All rights reserved.
 */

#include <sk/skirt.h>
#include <sk/task.h>
#include <sk/serial.h>
#include <sk/ipc.h>

sk_stack_t stack1[64];
sk_stack_t stack2[64];

sk_task *t1 = NULL;
sk_task *t2 = NULL;

sk_sem *s;

void task1(void)
{
	sk_serial_print("Task1 waiting for T2\n");
	sk_sem_acquire(s);
	sk_serial_print("Task1 set to wait!\n");
	sk_task_sleep(10000);
	sk_serial_print("Task1 stopped waiting :)\n");
	sk_task_kill(t2);
	sk_serial_print("Killed t2\n");
	sk_task_exit();

	for (;;)
		;
}

void task2(void)
{
	sk_serial_print("Task2 waiting\n");
	sk_task_sleep(1000);
	sk_serial_print("Task2 releasing sem\n");
	sk_sem_release(s);
	for (;;) {
		sk_serial_print("Task2 Running...\n");
	}
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
