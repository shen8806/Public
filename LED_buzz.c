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


/////////////// LED���ȳ�ʼ�� ///////////////
void LED_init(void)
{
	if( Fan_DAT.dev_onoff )
		LED_showONOFF( 1 );
	else
		LED_showONOFF( 0 );
}

/////////////// LED���ȵ��� ///////////////
void LED_light_set(uint8_t percen)
{
	if( percen>100 )
		percen = 100;
	PWM_ConfigOutputChannel(LED_LIGHT_PWM, LED_LIGHT_CH, 1000, 100-percen);	//1KHz
	PWM_EnableOutput(LED_LIGHT_PWM, (0x01<<LED_LIGHT_CH) );
	PWM_Start( LED_LIGHT_PWM, (0x01<<LED_LIGHT_CH) );
}

/////////////// ���ؼ��ĵ���ʾ ///////////////
void LED_showONOFF(uint8_t onoff_flag)
{
	if(onoff_flag) //����ʱָʾ��״̬
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

/////////////// LED1����-����ʱ��ر� ///////////////
void LED0_on_set(uint16_t ms_dat)
{
	LED0_tim_cnt = ms_dat/10;
	LED0_ON();
}


/////////////// LED�ڶ�ʱ������ѯ ///////////////
void LED_tim_poll(void)
//10mS��ѯһ��
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
		BUZZ = 1;	//�ر�
	}
}



/////////////// ���������AD��ʼ�� ///////////////
void BRGHT_init(void)
{
	ADC_Open(ADC, ADC_ADCR_DIFFEN_SINGLE_END, ADC_ADCR_ADMD_CONTINUOUS, 0x01<<BRGHT_AD_CH);
	ADC_POWER_ON(ADC);
	NVIC_DisableIRQ(ADC_IRQn);
	ADC_CLR_INT_FLAG(ADC, ADC_ADF_INT);
	Fan_DAT.bw_flag = 1;
}

/////////////// ���������ȡAD��ֵ ///////////////
void BRGHT_get_ad(void)
{
	uint32_t u32_ad_dat;
	ADC_CLR_INT_FLAG(ADC, ADC_ADF_INT);
	ADC_START_CONV(ADC);
	while(!ADC_GET_INT_FLAG(ADC, ADC_ADF_INT));
	ADC_CLR_INT_FLAG(ADC, ADC_ADF_INT);

	//12λADֵ�����0xFFF;
	u32_ad_dat = ADC_GET_CONVERSION_DATA(ADC, BRGHT_AD_CH)&0xFFF;

	if(u32_ad_dat <= 0x300)		//����
		Fan_DAT.bw_flag = 1;
	else if(u32_ad_dat >= 0x350)	//����
		Fan_DAT.bw_flag = 0;

	//printf("## BRGHT dat=0x%.8X, BW_FLAG=%d\r\n",u32_ad_dat, Fan_DAT.bw_flag);
}

/////////////// ����ִ����ѯ ///////////////
void BRGHT_run_poll(void)
{
	if(BRGHT_get_flag)
	{
		BRGHT_get_flag = 0;
		BRGHT_get_ad();		//��Խ��ֵԽС
	}
}

/////////////// �������������� ///////////////
void buzz_on_set(uint16_t ms_dat)
//ms_dat: ������ʱ�� 1=1mS
{
	buzz_on_flag = 1;
	buzz_tim_cnt = ms_dat/10;
	BUZZ = 0;	//����
}

/////////////// ����������Ĭ�ϵ���ʽ���� ///////////////
void buzz_on_def(void)
{
	buzz_on_set(BUZZ_TIM_DEF);
}


