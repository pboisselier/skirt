/**
 * @brief Kernel library main functions.
 * @copyright Copyright (c) 2022 Pierre Boisselier All rights reserved.
 */
#include <sk/skirt.h>
#include <sk/arch.h>

extern sk_task* volatile task_current;

static sk_stack_t idle_stack[SKIRT_TASK_STACK_SZ];

/* Idling task, always ready. */
SK_NOOPTI SK_NORETURN void sk_kernel_idle_task(void)
{
	for (;;) {
		sk_arch_nop();
	}
	SK_VERIFY_NOT_REACHED();
}

void sk_kernel_start(void)
{
	sk_arch_serial_init();
	sk_task* idle = sk_task_create_static(sk_kernel_idle_task, 0, idle_stack,
			      sizeof idle_stack);
	sk_arch_init_preempt();

	task_current = idle;
	sk_arch_restore_task_context(task_current);
	__asm__ __volatile__ ("ret");

	SK_VERIFY_NOT_REACHED();
}
