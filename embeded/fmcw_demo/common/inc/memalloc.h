#ifndef __MEMALLOC_H__
#define __MEMALLOC_H__
#include <stdint.h>

void init_mem(void);
void* alloc_mem(uint32_t size);
void free_mem(void   * addr);

#endif
