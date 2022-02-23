#ifndef	_LED_BUZZ_H_
#define	_LED_BUZZ_H_

#include "M0518.h"


#define LED0      	PB14

#define LED1      	PA15
#define LED2      	PA14
#define LED3      	PA13
#define LED4      	PA12
#define LED5      	PC0
#define LED6      	PC2
#define LED7      	PC1

#define BUZZ		PB8
#define BUZZ_TIM_DEF		30		//1=1mS
#define BUZZ_TIM_70MS		70
#define BUZZ_TIM_100MS	100
#define BUZZ_TIM_200MS	200


#define BRGHT_AD_CH		7 	//光敏电阻的AD脚通道号



#define LED_LIGHT_PWM	PWM0
#define LED_LIGHT_CH	4

#define LED_ON_LIGHT	100	//开机时的LED亮度
#define LED_OFF_LIGHT	40	//关机/休眠时的LED亮度


#define LED_TIME_50MS		50   
#define LED_TIME_100MS	100
#define LED_TIME_200MS	200 
#define LED_TIME_1S		1000
#define LED_TIME_3S		3000


#define LED_DEF_TIMGAP  300		//平时运行时闪烁的间隔


void LED_init(void);
void LED_tim_poll(void);
void LED0_on_set(uint16_t ms_dat);
//void LED2_on_set(uint16_t ms_dat);
void LED_light_set(uint8_t percen);
void LED_showONOFF(uint8_t onoff_flag);


void BRGHT_init(void);
void BRGHT_run_poll(void);


void buzz_on_set(uint16_t ms_dat);
void buzz_on_def(void);



#endif



