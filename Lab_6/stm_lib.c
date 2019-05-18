#include "stm_lib.h"

void init_led(void);
void init_TIM1(void);
void init_PWM(void);
void init_button_inter(void);
void init_button(void);

void init_leds(void)
{
  init_led();
  init_TIM1();
  init_PWM();
}

void init_buttons(void)
{
  init_button();
  init_button_inter();
}

void set_color(int red, int green, int blue)
{
  TIM_SetCompare1(TIM1, red / 255);
  TIM_SetCompare2(TIM1, green / 255);
  TIM_SetCompare3(TIM1, blue / 255);
}

void turn_off(void)
{
  TIM_SetCompare3(TIM1, 0);
  TIM_SetCompare1(TIM1, 0);
  TIM_SetCompare2(TIM1, 0);
}

void init_led(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource8, GPIO_AF_TIM1);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_TIM1);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_TIM1);
  GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void init_TIM1(void)
{
  TIM_TimeBaseInitTypeDef tim_struct;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
  tim_struct.TIM_Period = period - 1;
  tim_struct.TIM_Prescaler = prescaler - 1;
  tim_struct.TIM_ClockDivision = 0;
  tim_struct.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM1, &tim_struct);
}

 void init_PWM(void)
{
  TIM_OCInitTypeDef timerPWM;
  timerPWM.TIM_OCMode = TIM_OCMode_PWM1;
  timerPWM.TIM_OutputState = TIM_OutputState_Enable;
  timerPWM.TIM_Pulse = 0;
  timerPWM.TIM_OCPolarity = TIM_OCPolarity_Low;
  TIM_OC1Init(TIM1, &timerPWM);
  TIM_OC2Init(TIM1, &timerPWM);
  TIM_OC3Init(TIM1, &timerPWM);
  TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);
  TIM_OC2PreloadConfig(TIM1, TIM_OCPreload_Enable);
  TIM_OC3PreloadConfig(TIM1, TIM_OCPreload_Enable);
  TIM_Cmd(TIM1, ENABLE);
  TIM_CtrlPWMOutputs(TIM1, ENABLE);
} 

void init_button(void)
{
  GPIO_InitTypeDef GPIO_InitStructureBN;
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
  GPIO_InitStructureBN.GPIO_Pin   = GPIO_Pin_0 | GPIO_Pin_1;
  GPIO_InitStructureBN.GPIO_Mode  = GPIO_Mode_IN;
  GPIO_InitStructureBN.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructureBN.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructureBN.GPIO_PuPd  = GPIO_PuPd_UP;
  GPIO_Init(GPIOE, &GPIO_InitStructureBN);
}
void init_button_inter(void)
{
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE,EXTI_PinSource0);
  EXTI_InitTypeDef EXTI_InitStruct;
  EXTI_InitStruct.EXTI_Line= EXTI_Line0;
  EXTI_InitStruct.EXTI_LineCmd= ENABLE;
  EXTI_InitStruct.EXTI_Mode= EXTI_Mode_Interrupt;
  EXTI_InitStruct.EXTI_Trigger= EXTI_Trigger_Rising;
  EXTI_Init(&EXTI_InitStruct);
  NVIC_InitTypeDef nvic_struct_for_EXTI;
  nvic_struct_for_EXTI.NVIC_IRQChannel= EXTI0_IRQn;
  nvic_struct_for_EXTI.NVIC_IRQChannelPreemptionPriority= 0;
  nvic_struct_for_EXTI.NVIC_IRQChannelSubPriority= 0;
  nvic_struct_for_EXTI.NVIC_IRQChannelCmd= ENABLE;
  NVIC_Init(&nvic_struct_for_EXTI);

  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE,EXTI_PinSource1);
  EXTI_InitTypeDef EXTI_InitStruct1;
  EXTI_InitStruct1.EXTI_Line= EXTI_Line1;
  EXTI_InitStruct1.EXTI_LineCmd= ENABLE;
  EXTI_InitStruct1.EXTI_Mode= EXTI_Mode_Interrupt;
  EXTI_InitStruct1.EXTI_Trigger= EXTI_Trigger_Rising;
  EXTI_Init(&EXTI_InitStruct1);
  NVIC_InitTypeDef nvic_struct_for_EXTI1;
  nvic_struct_for_EXTI1.NVIC_IRQChannel= EXTI1_IRQn;
  nvic_struct_for_EXTI1.NVIC_IRQChannelPreemptionPriority= 0;
  nvic_struct_for_EXTI1.NVIC_IRQChannelSubPriority= 0;
  nvic_struct_for_EXTI1.NVIC_IRQChannelCmd= ENABLE;
  NVIC_Init(&nvic_struct_for_EXTI1);
}