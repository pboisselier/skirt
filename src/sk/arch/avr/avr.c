/**
 * @brief AVR Specific code.
 * @copyright Copyright (c) 2022 Pierre Boisselier All rights reserved.
 */

#include <sk/arch.h>
#include <sk/serial.h>

#ifdef SKIRT_VANITY
const char *const panic_art[5] = {
	" _  _______ ____  _   _ _____ _       ____   _    _   _ ___ ____\n\r",
	"| |/ / ____|  _ \\| \\ | | ____| |     |  _ \\ / \\  | \\ | |_ _/ ___|\n\r",
	"| ' /|  _| | |_) |  \\| |  _| | |     | |_) / _ \\ |  \\| || | |    \n\r",
	"| . \\| |___|  _ <| |\\  | |___| |___  |  __/ ___ \\| |\\  || | |___\n\r",
	"|_|\\_\\_____|_| \\_\\_| \\_|_____|_____| |_| /_/   \\_\\_| \\_|___\\____|\n\r\n\r> "
};
#else
const char *const panic_art[5] = { "\n\r", "\n\r", "-- KERNEL PANIC --\n\r",
				   "\n\r", "> " };
#endif /* SKIRT_VANITY */

extern sk_task *volatile task_current;
extern sk_task *volatile task_head;

#define stack_overflow_protection(sp, stack_end)         \
	do {                                             \
		if (sp < stack_end) {                    \
			SK_PANIC("Stack overflow!\n\r"); \
		}                                        \
	} while (0)

#ifdef __AVR_ATmega328P__

#ifndef F_CPU
#define F_CPU 16000000
#endif /* F_CPU */

ISR(TIMER1_COMPA_vect, ISR_NAKED)
{
	cli();
	sk_arch_save_context();
	stack_overflow_protection((sk_stack_t *)SP, task_current->stack);
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
	for (unsigned i = 0; i < 5; ++i) {
		sk_serial_print(panic_art[i]);
	}
	sk_serial_print(msg);
	for (;;)
		;
}

/* TODO: Add more serial options. */
void sk_arch_serial_init(void)
{
	UBRR0 = F_CPU / 16L / SKIRT_SERIAL_BAUD;
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
