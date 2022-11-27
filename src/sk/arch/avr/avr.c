/**
 * @brief AVR Specific code.
 * @copyright Copyright (c) 2022 Pierre Boisselier All rights reserved.
 */

#include <sk/arch.h>
#include <sk/serial.h>

extern sk_task *volatile task_current;
extern sk_task *volatile task_head;

#ifdef __AVR_ATmega328P__
ISR(TIMER1_COMPA_vect, ISR_NAKED)
{
	cli();
	sk_arch_save_context();
	task_current->sp = (sk_stack_t *)SP;
	sk_task_switch();
	sk_arch_restore_task_context(task_current);
	__asm__ __volatile__("reti" ::: "memory");
}
void sk_arch_init_preempt(void)
{
	/* Enable Timer 2 to be used as preemption mechanism. */
	TCCR1A = 0;
	TCCR1B = 0x0C; /* Prescaler set to 256 */
	TIMSK1 = 2; /* Interrupt on compare */
	TCNT1 = 0;
	/* TODO: Find an unified way to set preemption time across architectures. */
	OCR1A = SKIRT_PREEMPT_TIME;
#else
#error "MCU is not supported!"
#endif
}

SK_NAKED void sk_arch_yield(void)
{
	sk_arch_save_context();
	task_current->sp = (sk_stack_t *)SP;
	sk_task_switch();
	SK_ASSERT(task_current);
	sk_arch_restore_task_context(task_current);
	__asm__ __volatile__("sei\t\nret");
}

SK_NORETURN void sk_arch_panic(const char *msg)
{
	sk_serial_print("---- KERNEL PANIC ----\n");
	sk_serial_print("Reason: ");
	sk_serial_print(msg);
	for (;;)
		;
}

/* TODO: Add more serial options. */
void sk_arch_serial_init(void)
{
	UBRR0 = 9600L;
	/* Enable Tx and Rx operations */
	UCSR0B = (1 << RXEN0) | (1 << TXEN0);
	/* Set communication to asynchronous mode */
	char ucsrc_tmp = (0 << UMSEL00) | (0 << UMSEL01);
	ucsrc_tmp |= (1 << UCSZ01) | (1 << UCSZ00);
	UCSR0C = ucsrc_tmp;
}

void sk_arch_serial_putc(char c)
{
	/* Wait for empty transmit buffer */
	while (!(UCSR0A & (1 << UDRE0)))
		;
	/* Put data into buffer, sends the data */
	UDR0 = c;
}
