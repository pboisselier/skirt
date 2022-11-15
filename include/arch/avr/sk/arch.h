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

#define sk_arch_save_context()                              \
	__asm__ volatile("push  r0                    \n\t" \
			 "in    r0, __SREG__          \n\t" \
			 "push  r0                    \n\t" \
			 "push  r1                    \n\t" \
			 "push  r2                    \n\t" \
			 "push  r3                    \n\t" \
			 "push  r4                    \n\t" \
			 "push  r5                    \n\t" \
			 "push  r6                    \n\t" \
			 "push  r7                    \n\t" \
			 "push  r8                    \n\t" \
			 "push  r9                    \n\t" \
			 "push  r10                   \n\t" \
			 "push  r11                   \n\t" \
			 "push  r12                   \n\t" \
			 "push  r13                   \n\t" \
			 "push  r14                   \n\t" \
			 "push  r15                   \n\t" \
			 "push  r16                   \n\t" \
			 "push  r17                   \n\t" \
			 "push  r18                   \n\t" \
			 "push  r19                   \n\t" \
			 "push  r20                   \n\t" \
			 "push  r21                   \n\t" \
			 "push  r22                   \n\t" \
			 "push  r23                   \n\t" \
			 "push  r24                   \n\t" \
			 "push  r25                   \n\t" \
			 "push  r26                   \n\t" \
			 "push  r27                   \n\t" \
			 "push  r28                   \n\t" \
			 "push  r29                   \n\t" \
			 "push  r30                   \n\t" \
			 "push  r31                   \n\t")

#define sk_arch_restore_context()                           \
	__asm__ volatile("pop  r31                    \n\t" \
			 "pop  r30                    \n\t" \
			 "pop  r29                    \n\t" \
			 "pop  r28                    \n\t" \
			 "pop  r27                    \n\t" \
			 "pop  r26                    \n\t" \
			 "pop  r25                    \n\t" \
			 "pop  r24                    \n\t" \
			 "pop  r23                    \n\t" \
			 "pop  r22                    \n\t" \
			 "pop  r21                    \n\t" \
			 "pop  r20                    \n\t" \
			 "pop  r19                    \n\t" \
			 "pop  r18                    \n\t" \
			 "pop  r17                    \n\t" \
			 "pop  r16                    \n\t" \
			 "pop  r15                    \n\t" \
			 "pop  r14                    \n\t" \
			 "pop  r13                    \n\t" \
			 "pop  r12                    \n\t" \
			 "pop  r11                    \n\t" \
			 "pop  r10                    \n\t" \
			 "pop  r9                     \n\t" \
			 "pop  r8                     \n\t" \
			 "pop  r7                     \n\t" \
			 "pop  r6                     \n\t" \
			 "pop  r5                     \n\t" \
			 "pop  r4                     \n\t" \
			 "pop  r3                     \n\t" \
			 "pop  r2                     \n\t" \
			 "pop  r1                     \n\t" \
			 "pop  r0                     \n\t" \
			 "out  __SREG__, r0           \n\t" \
			 "pop  r0                     \n\t")

/**
 * @brief Initialize stack for a task.
 * @param func Task function pointer for initialization.
 * @param task Newly created task.
 */
extern void sk_arch_stack_init(sk_task_func func, sk_task *task);
/**
 * @brief Yield task to another, defined in architecture-specific files.
 */
extern SK_NAKED void sk_arch_yield(void);
/**
 * @brief Yield task to another, from an interrupt handler.
 */
extern SK_NAKED void sk_arch_yield_int(void);

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
		SP = (sk_size_t)task_current->sp; \
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
