/* /////////////////////////////////////////////////////
**** Software version: V1.1.0       ****
**** Hardware number : P030_V1.1  ****
**** Author          : shensida     ****
**** Date            : 2017-03-29   ****
/////////////////////////////////////////////////////// */

#include "main.h"


uint8_t t1s_flag = 0;

///////////////==== main函数 ====/////////////// 
int  main( void )
{
	all_init();

	while( 1 )
	{
		WDT_poll();
		CTRL_frRX_poll();
		CTRL_TX_poll();
		IR_RXrun_poll();
		key_run_poll();
		WDT_poll();
		CTRL_sensor_get();
		FAN_run_poll();
		RF_RX_poll();
		RF_sensor_up();
		LCD_display_poll();
		BRGHT_run_poll();
	}
}


/////////////// 总初始化 /////////////// 
void all_init(void)
{
	SYS_Init();
	IO_Init();
	UART0_Init();
	TMR1_Init();
	UART_all_Init();
	PWM_Init();
	CTRL_init();
	FAN_init();
	BRGHT_init();
	LED_init();
	WDT_Init();
	key_init();

	printf_boot_info();
	read_uid();
	device_id_get();

	buzz_on_set(BUZZ_TIM_100MS);

	M433_COM_set();
	WDT_CR_set();
	IR_init();
	TM16_init();
	LCD_init();
	M433_fr_init();

	CTRL_speed_set(0);
	CTRL_TX_set(UART_HEATING_SET);
	CTRL_TX_poll();

	printf("## All init OK! \r\n");
	WDT_CR_set();
}

/////////////// 开机信息串口输出 ///////////////
void printf_boot_info(void)
{
	WDT_CR_set();
	printf("\r\n\r\n+===========================================================+\r\n");
	printf("|    <<<       Wall-mounted New FAN           >>>           |\r\n");
	printf("+-----------------------------------------------------------+\r\n");
	printf("|  Author : shensida                                        |\r\n");
	printf("|  Date   : ");
	printf(__DATE__);
	printf(" , ");
	printf(__TIME__);
	WDT_CR_set();
	printf("                          |\r\n");
	printf("|  Version: V1.1.0                                          |\r\n");
	printf("|  Board  : P030_V1.1                                     |\r\n");
	printf("+-----------------------------------------------------------+\n\r\n");
	WDT_CR_set();
}

/////////////// Timer1中断 系统用 ///////////////
//10mS中断1次
void TMR1_IRQHandler(void)
{
	if(TIMER_GetIntFlag(TIMER1) == 1)
	{
	    TIMER_ClearIntFlag(TIMER1);
		
		TMR1_ms_cnt++;
		if(TMR1_ms_cnt>= 60000)
			TMR1_ms_cnt = 0;

		UART_RXtime_poll();
		key_scan_poll();
		LCD_tim_poll();
		FAN_tim_poll();
		CTRL_tim_poll();
		RF_tim_poll();
		LED_tim_poll();
	}
}



