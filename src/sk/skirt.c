/**
 * @brief Kernel library main functions.
 * @copyright Copyright (c) 2022 Pierre Boisselier All rights reserved.
 */
#include <sk/skirt.h>
#include <sk/arch.h>

static sk_stack_t idle_stack[SKIRT_TASK_STACK_SZ];

/* Idling task, always ready. */
SK_NOOPTI SK_NORETURN void sk_kernel_idle_task(void)
{
	for (;;) {
		sk_arch_nop();
	}
}

SK_NORETURN void sk_kernel_start(void)
{
	sk_arch_serial_init();
	sk_task_create_static(sk_kernel_idle_task, 0, idle_stack,
			      sizeof idle_stack);
	sk_arch_init_preempt();
	sk_arch_yield_int();

	SK_VERIFY_NOT_REACHED();
}
