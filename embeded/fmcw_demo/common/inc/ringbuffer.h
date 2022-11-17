#ifndef __RINGBUFFER_H__
#define __RINGBUFFER_H__

#include "sys.h"
#include "common.h"
#define ELEMENT_SIZE         (SAMPLE_NUM_PER_CHIRP * NUM_CHIRPS_PER_FRAME * 2U)
#define ELEMENT_COUNT        (2U)


typedef struct {
    __IO u16 rd;
    __IO u16 wr;
    u16 max_count;
    u16 elem_size;
    __IO u8 *buf;
} ring_buf_t;

static inline int ring_buf_empty(ring_buf_t *ring_buf)
{
    if (NULL == ring_buf)
        return -EINVAL;

    if (ring_buf->rd == ring_buf->wr)
        return 1;

    return 0;
}

static inline int ring_buf_full(ring_buf_t *ring_buf)
{
    int rd, wr;

    if (NULL == ring_buf) {
        PRINT_LOG(LOG_LEVEL_ERROR, "buffer full\n");
        return -EINVAL;
    }

    rd = ring_buf->rd;
    wr = ring_buf->wr;
    if ((wr + 1) % ring_buf->max_count == rd)
        return 1;

    return 0;
}

int ring_buffer_put(ring_buf_t *ring_buf);
int ring_buffer_get(ring_buf_t *ring_buf, u8 *data);

#endif
