#ifndef __COMMON_H__
#define __COMMON_H__

#include "stdio.h"

#define    EPERM       1    /* Operation not permitted */
#define    ENOENT      2    /* No such file or directory */
#define    ESRCH       3    /* No such process */
#define    EINTR       4    /* Interrupted system call */
#define    EIO         5    /* I/O error */
#define    ENXIO       6    /* No such device or address */
#define    E2BIG       7    /* Argument list too long */
#define    ENOEXEC     8    /* Exec format error */
#define    EBADF       9    /* Bad file number */
#define    ECHILD      10    /* No child processes */
#define    EAGAIN      11    /* Try again */
#define    ENOMEM      12    /* Out of memory */
#define    EACCES      13    /* Permission denied */
#define    EFAULT      14    /* Bad address */
#define    ENOTBLK     15    /* Block device required */
#define    EBUSY       16    /* Device or resource busy */
#define    EEXIST      17    /* File exists */
#define    EXDEV       18    /* Cross-device link */
#define    ENODEV      19    /* No such device */
#define    ENOTDIR     20    /* Not a directory */
#define    EISDIR      21    /* Is a directory */
#define    EINVAL      22    /* Invalid argument */
#define    ENFILE      23    /* File table overflow */
#define    EMFILE      24    /* Too many open files */
#define    ENOTTY      25    /* Not a typewriter */
#define    ETXTBSY     26    /* Text file busy */
#define    EFBIG       27    /* File too large */
#define    ENOSPC      28    /* No space left on device */
#define    ESPIPE      29    /* Illegal seek */
#define    EROFS       30    /* Read-only file system */
#define    EMLINK      31    /* Too many links */
#define    EPIPE       32    /* Broken pipe */
#define    EDOM        33    /* Math argument out of domain of func */
#define    ERANGE      34    /* Math result not representable */
#define    ENODATA     35    /* No data available */
#define    EINVALDATA  36    /* Invalid data */
#define    ETIMEOUT    37    /* Time out */

#define LOG_LEVEL_NONE                                (0)
#define LOG_LEVEL_ERROR                               (1)
#define LOG_LEVEL_WARNING                             (2)
#define LOG_LEVEL_INFO                                (3)
#define LOG_LEVEL_DEBUG                               (5)

#define    LOG_LEVEL                                  (6)

#define PRINT_LOG(level, fmt, args...)                \
    if (level < LOG_LEVEL) {                          \
        printf(fmt, ##args);                          \
    }


#define    __I         volatile const
#define    __O         volatile
#define    __IO        volatile

static inline u32 readl(__IO void *addr) {
    return *(__IO u32*)(addr);
}

static inline void writel(u32 val, __IO void *addr) {
    *(__IO u32*)addr = val;
}

static inline void writeb(u8 val, __IO void *addr) {
    *(__IO u8*)addr = val;
}


#endif
