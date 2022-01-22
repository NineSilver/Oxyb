#include <mutex.h>

void mutex_lock(mutex_t* mutex)
{
    while(!__sync_bool_compare_and_swap(&mutex->locked, 0, 1));
}

void mutex_unlock(mutex_t* mutex)
{
    __sync_bool_compare_and_swap(&mutex->locked, 1, 0);
}

int mutex_locked(mutex_t* mutex)
{
    return __sync_bool_compare_and_swap(&mutex->locked, 1, 1);
}
