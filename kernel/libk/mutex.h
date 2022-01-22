#ifndef __OXYB__LIBK__LOCK_H
#define __OXYB__LIBK__LOCK_H

#include <stdint.h>

typedef struct
{
    volatile int locked;
} mutex_t;

void mutex_lock(mutex_t* mutex);
void mutex_unlock(mutex_t* mutex);
int mutex_locked(mutex_t* mutex);

#endif /* ! __OXYB__LIBK__LOCK_H */
