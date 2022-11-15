/**
 * @brief Defines kernel types.
 * @copyright Copyright (c) 2022 Pierre Boisselier All rights reserved.
 */

#ifndef SKIRT_SK_TYPES_H
#define SKIRT_SK_TYPES_H

/* Using provided includes from GCC for AVR. */
#include <stddef.h>

typedef unsigned char sk_stack_t;
typedef size_t sk_size_t;

#ifndef NULL
#define NULL ((void *)0)
#endif /* NULL */

#endif /* SKIRT_SK_TYPES_H */
