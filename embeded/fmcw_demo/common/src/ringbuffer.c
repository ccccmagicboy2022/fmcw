#include "ringbuffer.h"
#include "string.h"
#if defined(CUSTOMER_BLUE_WAY) || defined(GONGNIU)
#include "presense_detection.h"
#endif
int ring_buffer_put(ring_buf_t *ring_buf)
{
    if (NULL == ring_buf) {
        PRINT_LOG(LOG_LEVEL_ERROR, "ring buf address is null\n");
        return -EINVAL;
    }

    if (ring_buf_full(ring_buf)) {
        PRINT_LOG(LOG_LEVEL_ERROR, "buffer full\n");
        return -ENOMEM;
    }

    ring_buf->wr = (ring_buf->wr + 1) % ring_buf->max_count;

    return 0;
}

#if defined(CUSTOMER_BLUE_WAY) || defined(GONGNIU)
static u8 first_power_on = 1;
int check_jump_condition(void)
{
    u8 ret = 0;
    u8 current_status = 0;
    static u8 flag = 0;
    static u8 last_status = 0;
    static u8 delay_num = 0;

    if (first_power_on) {
        last_status = get_current_output_state();
        first_power_on = 0;
    } else {
        if (flag) {
            ret = 1;
            delay_num++;
            if (delay_num >= 10) {
                delay_num = 0;
                flag = 0;
            }
        } else {
           current_status = get_current_output_state();
           if (last_status && !current_status) {
              delay_num++;
              flag = 1;
              ret = 1;
           }
           last_status = current_status;
        }
    }
    return ret;
}
#endif

int ring_buffer_get(ring_buf_t *ring_buf, u8 *data)
{
    u16 rd;
    u8 *ptr;
#if defined(CUSTOMER_BLUE_WAY) || defined(GONGNIU)
    u8 status = 0;
#endif

    if (NULL == ring_buf) {
        PRINT_LOG(LOG_LEVEL_ERROR, "ring buf address is null\n");
        return -EINVAL;
    }

    if (ring_buf_empty(ring_buf))
        return 0;

    rd = ring_buf ->rd;
    ptr = (u8 *)ring_buf->buf + rd * ring_buf->elem_size;

    // for (i = 0; i < ring_buf->elem_size; i++)
#if defined(CUSTOMER_BLUE_WAY) || defined(GONGNIU)
    status = check_jump_condition();
    if (!status) {
        memcpy(data, ptr, ring_buf->elem_size);
    }
#else
    memcpy(data, ptr, ring_buf->elem_size);
#endif

//#define UNIT_TEST
#ifdef UNIT_TEST
    PRINT_LOG(LOG_LEVEL_DEBUG, "%d, %d\n", rd, *((u16 *)ptr + 1));
#endif

    rd = (rd + 1) % ring_buf->max_count;
    ring_buf->rd = rd;

    return ring_buf->elem_size;
}
