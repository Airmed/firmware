
#ifndef __FLASH_H__
#define __FLASH_H__

#include <stdint.h>

void flash_init();

void flash_write(uint32_t addr, uint8_t data);
uint8_t flash_read(uint32_t addr);

#endif
