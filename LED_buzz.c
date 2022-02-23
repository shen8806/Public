#include "LED_buzz.h"
#include "FAN_app.h"
#include <stdio.h>

#define LED0_ON()		{ LED0 = 0; LED0_b01=1;}
#define LED0_OFF()		{ LED0 = 1; LED0_b01=0;}



uint16_t LED0_tim_cnt = 0;
uint16_t LED0_def_cnt = 0;
uint8_t LED0_b01;

uint8_t BRGHT_get_flag = 0;

uint16_t buzz_tim_cnt = 0;
uint8_t  buzz_on_flag = 0;


/////////////// LED亮度初始化 ///////////////
void LED_init(void)
{
	if( Fan_DAT.dev_onoff )
		LED_showONOFF( 1 );
	else
		LED_showONOFF( 0 );
}

/////////////// LED亮度调整 ///////////////
void LED_light_set(uint8_t percen)
{
	if( percen>100 )
		percen = 100;
	PWM_ConfigOutputChannel(LED_LIGHT_PWM, LED_LIGHT_CH, 1000, 100-percen);	//1KHz
	PWM_EnableOutput(LED_LIGHT_PWM, (0x01<<LED_LIGHT_CH) );
	PWM_Start( LED_LIGHT_PWM, (0x01<<LED_LIGHT_CH) );
}

/////////////// 开关键的灯显示 ///////////////
void LED_showONOFF(uint8_t onoff_flag)
{
	if(onoff_flag) //开机时指示灯状态
	{
		LED6 = 0;
		LED7 = 1;

		LED1 = 0;
		LED2 = 0;
		LED3 = 0;
		LED4 = 0;
		LED5 = 1;
		if(Fan_DAT.bw_flag)
			LED_light_set(LED_ON_LIGHT);
		else
			LED_light_set(LED_ON_LIGHT/2);
	}
	else
	{
		LED6 = 1;
		LED7 = 0;
		
		LED1 = 1;
		LED2 = 1;
		LED3 = 1;
		LED4 = 1;
		LED5 = 1;
		if(Fan_DAT.bw_flag)
			LED_light_set(LED_OFF_LIGHT);
		else
			LED_light_set(LED_OFF_LIGHT/2);
	}
}

/////////////// LED1点亮-根据时间关闭 ///////////////
void LED0_on_set(uint16_t ms_dat)
{
	LED0_tim_cnt = ms_dat/10;
	LED0_ON();
}


/////////////// LED在定时器里轮询 ///////////////
void LED_tim_poll(void)
//10mS轮询一次
{
	if(LED0_tim_cnt)
		LED0_tim_cnt--;
	else if(LED0_b01)
		LED0_OFF();

	if(LED0_def_cnt)
		LED0_def_cnt--;
	else
	{
		LED0_def_cnt = LED_DEF_TIMGAP;
		LED0_on_set(LED_TIME_1S);
		BRGHT_get_flag = 1;
	}

	if(buzz_tim_cnt)
		buzz_tim_cnt--;
	else if(buzz_on_flag)
	{
		buzz_on_flag = 0;
		BUZZ = 1;	//关闭
	}
}



/////////////// 光敏电阻的AD初始化 ///////////////
void BRGHT_init(void)
{
	ADC_Open(ADC, ADC_ADCR_DIFFEN_SINGLE_END, ADC_ADCR_ADMD_CONTINUOUS, 0x01<<BRGHT_AD_CH);
	ADC_POWER_ON(ADC);
	NVIC_DisableIRQ(ADC_IRQn);
	ADC_CLR_INT_FLAG(ADC, ADC_ADF_INT);
	Fan_DAT.bw_flag = 1;
}

/////////////// 光敏电阻获取AD数值 ///////////////
void BRGHT_get_ad(void)
{
	uint32_t u32_ad_dat;
	ADC_CLR_INT_FLAG(ADC, ADC_ADF_INT);
	ADC_START_CONV(ADC);
	while(!ADC_GET_INT_FLAG(ADC, ADC_ADF_INT));
	ADC_CLR_INT_FLAG(ADC, ADC_ADF_INT);

	//12位AD值，最大0xFFF;
	u32_ad_dat = ADC_GET_CONVERSION_DATA(ADC, BRGHT_AD_CH)&0xFFF;

	if(u32_ad_dat <= 0x300)		//白天
		Fan_DAT.bw_flag = 1;
	else if(u32_ad_dat >= 0x350)	//晚上
		Fan_DAT.bw_flag = 0;

	//printf("## BRGHT dat=0x%.8X, BW_FLAG=%d\r\n",u32_ad_dat, Fan_DAT.bw_flag);
}

/////////////// 光敏执行轮询 ///////////////
void BRGHT_run_poll(void)
{
	if(BRGHT_get_flag)
	{
		BRGHT_get_flag = 0;
		BRGHT_get_ad();		//光越亮值越小
	}
}

/////////////// 蜂鸣器开启设置 ///////////////
void buzz_on_set(uint16_t ms_dat)
//ms_dat: 开启的时间 1=1mS
{
	buzz_on_flag = 1;
	buzz_tim_cnt = ms_dat/10;
	BUZZ = 0;	//开启
}

/////////////// 蜂鸣器按照默认的形式蜂鸣 ///////////////
void buzz_on_def(void)
{
	buzz_on_set(BUZZ_TIM_DEF);
}


