#ifndef __OXYB__SYS__PIT_H
#define __OXYB__SYS__PIT_H

#include <stdint.h>

void init_pit(int hz);
uint32_t pit_read_count();
void pit_sleep(uint16_t ms);

#endif /* !__OXYB__SYS__PIT_H */
