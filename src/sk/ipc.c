/**
 * @brief Inter-process communications.
 * @copyright Copyright (c) 2022 Pierre Boisselier All rights reserved.
 */

#include <sk/ipc.h>

/* TODO: Find a prettier way to retrieve calling task. */
extern sk_task *task_current;

#ifdef SKIRT_ALLOC_STATIC

#define SK_SEM_MAGIC_NUMBER (-12)

/* Using SKIRT_TASK_MAX for pool size, should be plenty. */
static sk_sem sem_pool[SKIRT_TASK_MAX] = { 0 };

static inline sk_sem *sk_sem_alloc(int initial_value)
{
	for (sk_size_t i = 0; i < SKIRT_TASK_MAX; ++i) {
		if (sem_pool[i].flag == 0) {
			sem_pool[i].counter = initial_value;
			sem_pool[i].flag = 1;
			return &sem_pool[i];
		}
	}

	return NULL;
}

static inline void sk_sem_free(sk_sem *sem)
{
	SK_ASSERT(sem);
	if (sem < sem_pool || sem > &sem_pool[SKIRT_TASK_MAX - 1]) {
		SK_PANIC("Provided sem is not from the static pool!\n");
	}
	sem->counter = 0;
	sem->flag = 0;
}
#else
#error "FIXME: Only SKIRT_ALLOC_STATIC is supported!"
#endif /* SKIRT_ALLOC_STATIC */

sk_sem *sk_sem_create(int initial_value)
{
	sk_arch_disable_int();
	if (initial_value < 0) {
		initial_value = 0;
	}
	sk_sem *sem = sk_sem_alloc(initial_value);
	SK_ASSERT(sem);
	sk_arch_enable_int();
	return sem;
}

void sk_sem_destroy(sk_sem *sem)
{
	if (!sem) {
		return;
	}

	sk_arch_disable_int();
	sk_sem_free(sem);
	sk_arch_enable_int();
}

void sk_sem_release(sk_sem *sem)
{
	if (!sem) {
		return;
	}
	sk_arch_disable_int();
	sem->counter++;
	sk_arch_enable_int();
}

void sk_sem_acquire(sk_sem *sem)
{
	if (!sem) {
		return;
	}
	sk_arch_disable_int();

	while (sem->counter == 0) {
		sk_arch_disable_int();
		sk_arch_yield_int();
	}
	sem->counter--;
	sk_arch_enable_int();
}

bool sk_sem_try_acquire(sk_sem *sem)
{
	sk_arch_disable_int();
	if (sem->counter > 0) {
		sk_arch_enable_int();
		return false;
	}
	sem->counter--;
	sk_arch_enable_int();
	return true;
}
