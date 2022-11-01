#ifndef __MEMALLOC_H__
#define __MEMALLOC_H__

#include "sys.h"

void init_mem(void);
//void* alloc_mem(u32 size);
void* alloc_mem_ex(u32 size, const char* func_name, int line);
//void free_mem(void   * addr);
void free_mem_ex(void   * addr, const char* func_name, int line);
//
unsigned int GetFreeHeapKBSize(void);

#endif
