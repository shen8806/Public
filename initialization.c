#include <stdio.h>
#include "M0518.h"
#include "initialization.h"

/////////////// 初始化系统时钟 /////////////// 
void SYS_Init(void)
{
	SYS_UnlockReg();
	
	/* Enable Internal RC 22.1184MHz clock */
	CLK_EnableXtalRC(CLK_PWRCON_OSC22M_EN_Msk);

	/* Waiting for Internal RC clock ready */
	CLK_WaitClockReady(CLK_CLKSTATUS_OSC22M_STB_Msk);

	/* Switch HCLK clock source to Internal RC and HCLK source divide 1 */
	CLK_SetHCLK(CLK_CLKSEL0_HCLK_S_HIRC, CLK_CLKDIV_HCLK(1));

	/* Enable external XTAL 12MHz clock */
	//CLK_EnableXtalRC(CLK_PWRCON_XTL12M_EN_Msk);
	CLK_DisableXtalRC(CLK_PWRCON_XTL12M_EN_Msk);

	/* Waiting for external XTAL clock ready */
	//CLK_WaitClockReady(CLK_CLKSTATUS_XTL12M_STB_Msk);

	/* Set core clock as PLL_CLOCK from PLL */
	CLK_SetCoreClock(PLL_CLOCK);


	
	CLK_EnableModuleClock(UART0_MODULE);
	CLK_EnableModuleClock(UART1_MODULE);
	CLK_EnableModuleClock(UART2_MODULE);
	CLK_EnableModuleClock(UART3_MODULE);
	CLK_EnableModuleClock(UART4_MODULE);
	CLK_EnableModuleClock(TMR1_MODULE);
	CLK_EnableModuleClock(WDT_MODULE);
	//CLK_EnableModuleClock(I2C0_MODULE);
	//CLK_EnableModuleClock(I2C1_MODULE);
	CLK_EnableModuleClock(PWM0_MODULE);
	CLK_EnableModuleClock(BPWM0_MODULE);
	//CLK_EnableModuleClock(PWM1_MODULE);
	CLK_EnableModuleClock(ADC_MODULE);

	
	CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UART_S_PLL, CLK_CLKDIV_UART(1));
	CLK_SetModuleClock(UART1_MODULE, CLK_CLKSEL1_UART_S_PLL, CLK_CLKDIV_UART(1));
	CLK_SetModuleClock(UART2_MODULE, CLK_CLKSEL1_UART_S_PLL, CLK_CLKDIV_UART(1));
	CLK_SetModuleClock(UART3_MODULE, CLK_CLKSEL1_UART_S_PLL, CLK_CLKDIV_UART(1));
	CLK_SetModuleClock(UART4_MODULE, CLK_CLKSEL1_UART_S_PLL, CLK_CLKDIV_UART(1));
	
	
	CLK_SetModuleClock(TMR1_MODULE, CLK_CLKSEL1_TMR1_S_HCLK, 0);
	CLK_SetModuleClock(WDT_MODULE, CLK_CLKSEL1_WDT_S_LIRC, 0);
	CLK_SetModuleClock(PWM0_MODULE, CLK_CLKSEL3_PWM0_S_PLL, 0);
	CLK_SetModuleClock(BPWM0_MODULE, CLK_CLKSEL3_BPWM0_S_PLL, 0);
	//CLK_SetModuleClock(PWM1_MODULE, CLK_CLKSEL3_PWM1_S_PLL, 0);
	CLK_SetModuleClock(ADC_MODULE, 	CLK_CLKSEL1_ADC_S_HIRC, CLK_CLKDIV_ADC(1));


	SYS_LockReg();
}


/////////////// 初始化IO引脚 ///////////////
void IO_Init(void)
{
	//设定引脚复用功能
	SYS->GPA_MFP = SYS_GPA_MFP_PA5_UART3_RXD
				 | SYS_GPA_MFP_PA6_UART3_TXD
				 | SYS_GPA_MFP_PA7_ADC7 ;
	
	SYS->ALT_MFP4 = SYS_ALT_MFP4_PA5_UART3_RXD
				 | SYS_ALT_MFP4_PA6_UART3_TXD ;
	
	SYS->GPB_MFP = SYS_GPB_MFP_PB0_UART0_RXD
				 | SYS_GPB_MFP_PB1_UART0_TXD
				 | SYS_GPB_MFP_PB4_UART1_RXD
				 | SYS_GPB_MFP_PB5_UART1_TXD
				 | SYS_GPB_MFP_PB9_TM1
				 //| SYS_GPB_MFP_PB11_PWM0_CH4
				 | SYS_GPB_MFP_PB15_INT1 ;

	SYS->GPC_MFP = SYS_GPC_MFP_PC3_BPWM0_CH3
				 | SYS_GPC_MFP_PC6_UART4_TXD
				 | SYS_GPC_MFP_PC7_UART4_RXD ;

	SYS->ALT_MFP3 = SYS_ALT_MFP3_PC3_BPWM0_CH3
				 | SYS_ALT_MFP3_PB11_PWM0_CH4 ;

	SYS->GPD_MFP = SYS_GPD_MFP_PD14_UART2_RXD
				 | SYS_GPD_MFP_PD15_UART2_TXD ;

	SYS->GPF_MFP = SYS_GPF_MFP_PF0_XT1_OUT
				 | SYS_GPF_MFP_PF1_XT1_IN ;

	//配置引脚输入输出模式
	PA->PMD = PIN0_PMD_OUT
			| PIN1_PMD_IN
			| PIN4_PMD_OUT
			| PIN5_PMD_IN
			| PIN6_PMD_OUT
			| PIN7_PMD_IN
			| PIN8_PMD_IN
			| PIN9_PMD_IN
			| PIN10_PMD_IN
			| PIN11_PMD_IN
			| PIN12_PMD_OUT
			| PIN13_PMD_OUT
			| PIN14_PMD_OUT
			| PIN15_PMD_OUT ;
			
	PB->PMD = PIN0_PMD_OUT
			| PIN1_PMD_IN
			| PIN3_PMD_IN
			| PIN4_PMD_IN
			| PIN5_PMD_OUT
			| PIN6_PMD_OUT
			| PIN7_PMD_QUA
			| PIN8_PMD_OUT
			| PIN9_PMD_IN
			| PIN11_PMD_OUT
			| PIN12_PMD_OUT
			| PIN13_PMD_OUT
			| PIN14_PMD_OUT
			| PIN15_PMD_IN ;

	PC->PMD = PIN0_PMD_OUT
			| PIN1_PMD_OUT
			| PIN2_PMD_OUT
			| PIN3_PMD_OUT
			| PIN6_PMD_OUT
			| PIN7_PMD_IN ;


	PD->PMD = PIN14_PMD_IN
			| PIN15_PMD_OUT ;

	PF->PMD = PIN0_PMD_OUT
			| PIN1_PMD_IN
			| PIN4_PMD_IN
			| PIN5_PMD_IN ;
}


/////////////// 初始化UART0 ///////////////
void UART0_Init(void)
{
    /* Reset IP */
    SYS_ResetModule(UART0_RST);

    /* Configure UART0 and set UART0 Baudrate */
    UART_Open(UART0, 115200);
}


/////////////// 初始化Timer1, 并启动 /////////////// 
void TMR1_Init(void)
{
	TIMER_Open(TIMER1, TIMER_PERIODIC_MODE, 100);
    TIMER_EnableInt(TIMER1);

	NVIC_EnableIRQ(TMR1_IRQn);
	
	TIMER_Start(TIMER1);
}


/////////////// PWM0初始化 /////////////// 
void PWM_Init(void)
{
	SYS_ResetModule(PWM0_RST);
	SYS_ResetModule(BPWM0_RST);
}



