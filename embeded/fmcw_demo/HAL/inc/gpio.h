#ifndef __GPIO_H
#define __GPIO_H

#ifdef LONGQUAN

#define GPIO4_HIGH()        (PORT_SetBits(PortA, Pin00))
#define GPIO4_LOW()         (PORT_ResetBits(PortA, Pin00))

#define GPIO5_HIGH()        (PORT_SetBits(PortA, Pin04))
#define GPIO5_LOW()         (PORT_ResetBits(PortA, Pin04))

#endif

void gpio_init(void);

#endif
