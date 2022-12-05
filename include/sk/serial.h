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
 * @brief Provide a default serial interface.
 * @copyright Copyright (c) 2022 Pierre Boisselier All rights reserved.
 * @note Defaults are architecture-dependent (but should be 9600, 1-bit stopt, no paritiy, 8-bit data)
 */

#ifndef SKIRT_SERIAL_H
#define SKIRT_SERIAL_H

#include <sk/arch.h>

#ifdef SK_SERIAL_SUPPORT

#ifndef SKIRT_SERIAL_BAUD
#define SKIRT_SERIAL_BAUD 115200L
#endif /* SKIRT_SERIAL_BAUD */

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
