#ifndef STMLIB_H
#define STMLIB_H
#include "zb_common.h"
#include "zb_scheduler.h"
#include "zb_bufpool.h"
#include "zb_nwk.h"
#include "zb_aps.h"
#include "zb_zdo.h"
#include "stm32f4xx.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_tim.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_exti.h"
#include "stm32f4xx_syscfg.h"
#include "math.h"

#define period 30000
#define prescaler 100
#define brightness_step 5000

typedef struct color_e
{
	int red;
	int green;
	int blue;
}color_t;

void init_leds(void);

void set_color(int red, int green, int blue);

void init_buttons(void);

void turn_off(void);

#endif
