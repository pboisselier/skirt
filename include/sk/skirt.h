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
#define SK_ASSERT(pred)                                        \
	do {                                                   \
		if (!(pred)) {                                 \
			SK_PANIC("Assertion \"" #pred          \
				 "\" failed at " __BASE_FILE__ \
				 ":" SK_STR(__LINE__) "!\n\r");  \
		}                                              \
	} while (0)

#define SK_VERIFY_NOT_REACHED()               \
	SK_PANIC("Function at " __BASE_FILE__ \
		 ":" SK_STR(__LINE__) " should not return!\n\r")
#else
#define SK_VERIFY_NOT_REACHED() SK_PANIC("REACHED")
#define SK_ASSERT(pred) SK_PANIC("ASSERT")
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
