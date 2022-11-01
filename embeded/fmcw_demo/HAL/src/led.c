#include "led.h"
#include "sys.h"
#include "timer.h"

volatile led_cfg_t led_cfg;
volatile uint32_t blinking_count = 0;

extern void start_timer2(void);
extern void start_timer3(void);

void led_init(void)
{
    //pa11 - red
    //pa12 - green

    GPIO_InitType GPIO_InitStructure;

    GPIO_InitStructure.Pin        = GPIO_PIN_11 | GPIO_PIN_12;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitPeripheral(GPIOA, &GPIO_InitStructure);

    GPIO_SetBits(GPIOA, GPIO_PIN_11);
    GPIO_SetBits(GPIOA, GPIO_PIN_12);

    //GPIO_ResetBits(GPIOA, GPIO_PIN_11);
    //GPIO_ResetBits(GPIOA, GPIO_PIN_12);
}

void gpio_output_init(void)
{
    //pb5 - O
    //pa9 - O1

    GPIO_InitType GPIO_InitStructure;

    GPIO_InitStructure.Pin        = GPIO_PIN_5;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitPeripheral(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.Pin        = GPIO_PIN_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitPeripheral(GPIOA, &GPIO_InitStructure);

    GPIO_ResetBits(GPIOB, GPIO_PIN_5);
    GPIO_ResetBits(GPIOA, GPIO_PIN_9);
}

void LED_RED(void)
{
    GPIO_ResetBits(GPIOA, GPIO_PIN_11);
    GPIO_SetBits(GPIOA, GPIO_PIN_12);
}
void LED_OFF(void)
{
    GPIO_SetBits(GPIOA, GPIO_PIN_11);
    GPIO_SetBits(GPIOA, GPIO_PIN_12);
}

void LED_ON(void)
{
    GPIO_ResetBits(GPIOA, GPIO_PIN_11);
    GPIO_ResetBits(GPIOA, GPIO_PIN_12);
}

void OUT_OUT1_OFF(void)
{
    GPIO_ResetBits(GPIOB, GPIO_PIN_5);  //OUT
    GPIO_ResetBits(GPIOA, GPIO_PIN_9);  //OUT1
}

void OUT_OUT1_ON(void)
{
    GPIO_SetBits(GPIOB, GPIO_PIN_5);  //OUT
    GPIO_SetBits(GPIOA, GPIO_PIN_9);  //OUT1
}

void OUTON_OUT1OFF(void)
{
    GPIO_SetBits(GPIOB, GPIO_PIN_5);  //OUT
    GPIO_ResetBits(GPIOA, GPIO_PIN_9);  //OUT1
}

void LED_BLUE(void) //actually green
{
    GPIO_SetBits(GPIOA, GPIO_PIN_11);
    GPIO_ResetBits(GPIOA, GPIO_PIN_12);
}

void LED_GREEN(void) //actually yellow
{
    GPIO_ResetBits(GPIOA, GPIO_PIN_11);
    GPIO_ResetBits(GPIOA, GPIO_PIN_12);
}

void RED_GREEN_OFF(void)
{
    GPIO_SetBits(GPIOA, GPIO_PIN_11);
    GPIO_SetBits(GPIOA, GPIO_PIN_12);
}

void turnoff_led_blinking(void)
{
    TIM_Enable(TIM3, DISABLE);
    blinking_count = 0;
}

void trigger_led_blinking(led_cfg_t arg)
{
    led_cfg = arg;
    blinking_count = 0;
    start_timer3();
}

void trigger_findme_led_blinking(void)
{
    start_timer2();
}
