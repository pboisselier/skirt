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
#define SK_CONTEXT_SZ 35

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
#define sk_arch_restore_task_context(task) \
	do {                               \
		SP = (sk_size_t)task->sp;  \
		sk_arch_restore_context(); \
	} while (0)

/**
 * @brief Enable preemption timer.
 */
extern void sk_arch_init_preempt(void);

/**
 * @brief Provide a way to use a "naked" return from kernel code.
 */
#define sk_arch_first_yield(task)                   \
	do {                                        \
		sk_arch_restore_task_context(task); \
		__asm__ __volatile__("ret");        \
	} while (0)

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

/**
 * @brief Initialize stack for a task.
 * @param func Task function pointer for initialization.
 * @param task Newly created task.
 */
SK_INLINE void sk_arch_stack_init(sk_task_func func, sk_task *task)
{
	SK_ASSERT(task->stack_sz > SK_CONTEXT_SZ);

	task->sp = (task->stack + task->stack_sz - 1);
	*(task->sp) = (uint8_t)((uint16_t)func & 0xff);
	task->sp--;
	*(task->sp) = (uint8_t)(((uint16_t)func >> 8) & 0xff);
	task->sp--;
	*task->sp = (uint8_t)0x00; /* r0 to 0 */
	task->sp--;
	*task->sp = (uint8_t)0x80; /* Interrupts ON */
	task->sp--;
	*task->sp = (uint8_t)0x00; /* r1 to 0 */
	task->sp -= (SK_CONTEXT_SZ - 4);
}
#endif /* SKIRT_KERNEL */

#endif /* SKIRT_ARCH_H */
