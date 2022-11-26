/**
 * @brief Architecture-dependent functions.
 * @copyright Copyright (c) 2022 Pierre Boisselier All rights reserved.
 * @note All functions beginning with "sk_arch_*" will be defined here but
 * their implementation may be somewhere else in src/sk/arch/<arch>/<arch>.c
 */

#ifndef SKIRT_ARCH_H
#define SKIRT_ARCH_H

#include <sk/skirt.h>
#include <sk/types.h>
#include <sk/task.h>

/* Using provided AVR headers. */
#include <avr/io.h>
#include <avr/interrupt.h>

/**
 * @brief This platform supports a serial output.
 */
#define SK_SERIAL_SUPPORT 1

#ifdef SKIRT_KERNEL

#ifdef __AVR_ATmega328P__
#define SK_CONTEXT_SZ 31

/**
 * Used under MIT License.
 * @copyright Copyright (C) 2021 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 */
#define sk_arch_save_context()                                        \
	__asm__ __volatile__(                                         \
		"push   __tmp_reg__                             \n\t" \
		"in     __tmp_reg__, __SREG__                   \n\t" \
		"cli                                            \n\t" \
		"push   __tmp_reg__                             \n\t" \
		"push   __zero_reg__                            \n\t" \
		"clr    __zero_reg__                            \n\t" \
		"push   r2                                      \n\t" \
		"push   r3                                      \n\t" \
		"push   r4                                      \n\t" \
		"push   r5                                      \n\t" \
		"push   r6                                      \n\t" \
		"push   r7                                      \n\t" \
		"push   r8                                      \n\t" \
		"push   r9                                      \n\t" \
		"push   r10                                     \n\t" \
		"push   r11                                     \n\t" \
		"push   r12                                     \n\t" \
		"push   r13                                     \n\t" \
		"push   r14                                     \n\t" \
		"push   r15                                     \n\t" \
		"push   r16                                     \n\t" \
		"push   r17                                     \n\t" \
		"push   r18                                     \n\t" \
		"push   r19                                     \n\t" \
		"push   r20                                     \n\t" \
		"push   r21                                     \n\t" \
		"push   r22                                     \n\t" \
		"push   r23                                     \n\t" \
		"push   r24                                     \n\t" \
		"push   r25                                     \n\t" \
		"push   r26                                     \n\t" \
		"push   r27                                     \n\t" \
		"push   r28                                     \n\t" \
		"push   r29                                     \n\t" \
		"push   r30                                     \n\t" \
		"push   r31                                     \n\t")

/**
 * Used under MIT License.
 * @copyright Copyright (C) 2021 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 */
#define sk_arch_restore_context()                                     \
	__asm__ volatile(                                             \
		"pop  r31                    \n\t"                    \
		"pop  r30                    \n\t"                    \
		"pop  r29                    \n\t"                    \
		"pop  r28                    \n\t"                    \
		"pop  r27                    \n\t"                    \
		"pop  r26                    \n\t"                    \
		"pop  r25                    \n\t"                    \
		"pop  r24                    \n\t"                    \
		"pop  r23                    \n\t"                    \
		"pop  r22                    \n\t"                    \
		"pop  r21                    \n\t"                    \
		"pop  r20                    \n\t"                    \
		"pop  r19                    \n\t"                    \
		"pop  r18                    \n\t"                    \
		"pop  r17                    \n\t"                    \
		"pop  r16                    \n\t"                    \
		"pop  r15                    \n\t"                    \
		"pop  r14                    \n\t"                    \
		"pop  r13                    \n\t"                    \
		"pop  r12                    \n\t"                    \
		"pop  r11                    \n\t"                    \
		"pop  r10                    \n\t"                    \
		"pop  r9                     \n\t"                    \
		"pop  r8                     \n\t"                    \
		"pop  r7                     \n\t"                    \
		"pop  r6                     \n\t"                    \
		"pop  r5                     \n\t"                    \
		"pop  r4                     \n\t"                    \
		"pop  r3                     \n\t"                    \
		"pop  r2                     \n\t"                    \
		"pop    __zero_reg__                            \n\t" \
		"pop    __tmp_reg__                             \n\t" \
		"out    __SREG__, __tmp_reg__                   \n\t" \
		"pop    __tmp_reg__                             \n\t")

#else
#error "MCU not supported"
#endif

/**
 * @brief Initialize stack for a task.
 * @param func Task function pointer for initialization.
 * @param task Newly created task.
 */
SK_INLINE void sk_arch_stack_init(sk_task_func func, sk_task *task)
{
	task->sp = (task->stack + task->stack_sz - 1);
	*(task->sp) = (uint8_t)((uint16_t)func & 0xff);
	task->sp--;
	*(task->sp) = (uint8_t)(((uint16_t)func >> 8) & 0xff);
	task->sp --;
	*task->sp = (uint8_t)0x00; /* r0 to 0 */
	task->sp--;
	*task->sp = (uint8_t)0x80; /* Interrupts ON */
	task->sp--;
	*task->sp = (uint8_t)0x00; /* r1 to 0 */
	task->sp -= SK_CONTEXT_SZ;
}
/**
 * @brief Yield task to another, defined in avr.c.
 */
extern void sk_arch_yield(void) SK_NAKED;

/**
 * @brief Save SP and context.
 */
#define sk_arch_save_task_context(task)              \
	do {                                         \
		if (task) {                          \
			sk_arch_save_context();      \
			task->sp = (sk_stack_t *)SP; \
		}                                    \
	} while (0)

/**
 * @brief Restore context and SP.
 */
#define sk_arch_restore_task_context(task)        \
	do {                                      \
		SP = (sk_size_t)task->sp; \
		sk_arch_restore_context();        \
	} while (0)

/**
 * @brief Enable preemption timer.
 */
extern void sk_arch_init_preempt(void);

/**
 * @brief Nop on AVR architecture.
 */
#define sk_arch_nop() __asm__ __volatile__("nop");

/**
 * @brief Disable interrupts.
 */
#define sk_arch_enable_int() sei()

/**
 * @brief Enable interrupts.
 */
#define sk_arch_disable_int() cli()

#ifdef SK_SERIAL_SUPPORT

/**
 * @brief Put a character on the serial port.
 * @param c Character.
 */
extern void sk_arch_serial_putc(char c);

/**
 * @brief Initialize serial debug port (UART0).
 */
extern void sk_arch_serial_init(void);

#endif /* SK_SERIAL_SUPPORT */

#endif /* SKIRT_KERNEL */

#endif /* SKIRT_ARCH_H */
