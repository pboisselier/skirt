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
 * @brief Kernel library main functions.
 * @copyright Copyright (c) 2022 Pierre Boisselier All rights reserved.
 */
#include <sk/skirt.h>
#include <sk/arch.h>

extern sk_task *volatile task_current;

static sk_stack_t idle_stack[SKIRT_TASK_STACK_SZ];

/* Idling task, always ready. */
SK_NOOPTI SK_NORETURN void sk_kernel_idle_task(void)
{
	for (;;) {
		sk_arch_nop();
	}
	SK_VERIFY_NOT_REACHED();
}

void sk_kernel_start(void)
{
	sk_arch_serial_init();
	sk_task *idle = sk_task_create_static(sk_kernel_idle_task, 0,
					      idle_stack, sizeof idle_stack);
	sk_arch_init_preempt();

	task_current = idle;
	sk_arch_first_yield(task_current);

	SK_VERIFY_NOT_REACHED();
}
