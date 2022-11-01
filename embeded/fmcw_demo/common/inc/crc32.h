#ifndef  __CRC32_H
#define __CRC32_H
#include "sys.h"

#define    XOROT    0xffffffff

uint32_t calculate_CRC32(void *pStart, uint32_t uSize);
#endif
