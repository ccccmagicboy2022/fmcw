#ifndef __MYLED_H
#define __MYLED_H

extern void led_init(void);

extern void LED_RED(void);
extern void LED_GREEN(void);
extern void LED_BLUE(void);
extern void RED_GREEN_OFF(void);
extern void LED_OFF(void);
extern void LED_ON(void);
extern void OUT_OUT1_OFF(void);
extern void OUT_OUT1_ON(void);
extern void OUTON_OUT1OFF(void);
void led_blinking(void);

typedef enum {
    RED_LED = 0,
    GREEN_LED
} led_cfg_t;

void turnoff_led_blinking(void);
void trigger_led_blinking(led_cfg_t arg);
void trigger_findme_led_blinking(void);

void gpio_output_init(void);

#endif
