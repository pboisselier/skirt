/**
 * @brief Provide a default serial interface.
 * @copyright Copyright (c) 2022 Pierre Boisselier All rights reserved.
 * @note Defaults are architecture-dependent (but should be 9600, 1-bit stopt, no paritiy, 8-bit data)
 */

#ifndef SKIRT_SERIAL_H
#define SKIRT_SERIAL_H

#include <sk/arch.h>

#ifdef SK_SERIAL_SUPPORT

/**
 * @brief Initialize serial.
 * @note Default configuration depends on architecture.
 */
/* FIXME: Undefined reference may occur as it is not in exported symbols to user. */
#define sk_serial_init() sk_arch_serial_init()
/**
 * @brief Print a character on the serial device.
 */
/* FIXME: Undefined reference may occur as it is not in exported symbols to user. */
#define sk_serial_putc(c) sk_arch_serial_putc(c)

/**
 * @brief Print a string on the serial device.
 */
SK_INLINE void sk_serial_print(const char *str)
{
	while (*str != '\0') {
		sk_serial_putc(*str);
		str++;
	}
}

#endif /* SK_SERIAL_SUPPORT */

#endif /* SKIRT_SERIAL_H */
