#include <sys.h>

/* the size of chunk in MEMALLOC_DYNAMIC */
#define CHUNK_SIZE                    (1024)
#define ALLOC_SIZE                    (60 * 1024)
#define CHUNK_NUM                     (ALLOC_SIZE / CHUNK_SIZE)

typedef struct {
    uint32_t addr;
    uint32_t reserved;
} chunk_t;

static chunk_t chunks[CHUNK_NUM];
static char reserved_mem[ALLOC_SIZE];
static uint32_t total;

void init_mem(void)
{
    int i;
    uint32_t addr;

    addr = (uint32_t)reserved_mem;
    for (i = 0; i < CHUNK_NUM; i++) {
        chunks[i].addr = addr;
        chunks[i].reserved = 0;
        addr += CHUNK_SIZE;
    }
}

/* Cycle through the buffers we have, give the first free one */
void* alloc_mem(uint32_t size)
{
    int i = 0;
    int j = 0;
    uint32_t skip_chunks = 0, alloc_chunks, addr = 0;

    /* calculate how many chunks we need; round up to chunk boundary */
    alloc_chunks = (size + CHUNK_SIZE - 1) / CHUNK_SIZE;

    total += alloc_chunks;
    /* run through the chunk table */
    for (i = 0; i < CHUNK_NUM;) {
        skip_chunks = 0;
        /* if this chunk is available */
        if (!chunks[i].reserved) {
            /* check that there is enough memory left */
            if (i + alloc_chunks > CHUNK_NUM)
                break;

            /* check that there is enough consecutive chunks available */
            for (j = i; j < i + alloc_chunks; j++) {
                if (chunks[j].reserved) {
                    skip_chunks = 1;
                    /* skip the used chunks */
                    i = j + chunks[j].reserved;
                    break;
                }
            }

            /* if enough free memory found */
            if (!skip_chunks) {
                addr = chunks[i].addr;
                chunks[i].reserved = alloc_chunks;
                break;
            }
        } else {
             /* skip the used chunks */
            i += chunks[i].reserved;
        }
    }
#ifdef UNIT_TEST
    PRINT_LOG(LOG_LEVEL_DEBUG, "alloc: %d\n", total);
#endif

#ifdef HEAP_TEST
    printf("alloc: %d\r", total);
#endif

    if (addr == 0) {
        while (1) {}
#ifdef UNIT_TEST
        PRINT_LOG(LOG_LEVEL_ERROR, "memalloc failed: size = %d\n", size);
#endif
        return 0;
    }

    return (void *)addr;
}

void free_mem(void   * addr)
{
    int i = 0;

    for (i = 0; i < CHUNK_NUM; i++) {
        if (chunks[i].addr == (uint32_t)addr) {
            total -= chunks[i].reserved;
            chunks[i].reserved = 0;
            break;
        }
    }

#ifdef UNIT_TEST
    PRINT_LOG(LOG_LEVEL_DEBUG, "free: %d\n", total);
#endif

#ifdef HEAP_TEST
    printf("free: %d\r", total);
#endif
}

