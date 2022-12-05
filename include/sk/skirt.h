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
 * @brief Main kernel include.
 * @note Definitions beginning with "SKIRT_" can be played with, those beginning with "SK_" are internal only.
 * @copyright Copyright (c) 2022 Pierre Boisselier All rights reserved.
 */

#ifndef SKIRT_SKIRT_H
#define SKIRT_SKIRT_H

/* Kernel-only symbols. */
#ifdef SKIRT_KERNEL

#define SK_STR2(x) #x
#define SK_STR(x) SK_STR2(x)
#define SK_PANIC(msg)                  \
	do {                           \
		sk_arch_disable_int(); \
		sk_arch_panic(msg);    \
	} while (0)

#ifdef SKIRT_DEBUG
#define SK_ASSERT(pred)                                         \
	do {                                                    \
		if (!(pred)) {                                  \
			SK_PANIC("Assertion \"" #pred           \
				 "\" failed at " __BASE_FILE__  \
				 ":" SK_STR(__LINE__) "!\n\r"); \
		}                                               \
	} while (0)

#define SK_VERIFY_NOT_REACHED()               \
	SK_PANIC("Function at " __BASE_FILE__ \
		 ":" SK_STR(__LINE__) " should not return!\n\r")
#else
#define SK_VERIFY_NOT_REACHED() SK_PANIC("REACHED")
#define SK_ASSERT(pred)                                    \
	do {                                               \
		if (!(pred)) {                             \
			SK_PANIC("Assertion failed!\n\r"); \
		}                                          \
	} while (0)
#endif /* SKIRT_DEBUG */

#ifdef __GNUC__
#define SK_NAKED __attribute__((naked))
#define SK_NORETURN __attribute__((noreturn))
#define SK_INLINE __attribute__((always_inline)) inline
#define SK_HOT __attribute__((hot))
#define SK_NOOPTI __attribute__((optimize("O0")))
#else
#error "FIXME: Currently only supporting GCC!."
#endif

/**
 * @brief Kernel panic!
 * @param msg Message to display on debugging port.
 */
extern void sk_arch_panic(const char *msg) SK_NORETURN;

#endif /* SKIRT_KERNEL */

/**
 * @brief Start kernel.
 * @note Tasks should be created before calling it.
 */
extern void sk_kernel_start(void);

#endif /* SKIRT_SKIRT_H */
