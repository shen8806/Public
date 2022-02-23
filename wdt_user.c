#include "wdt_user.h"
#include <stdio.h>

extern  uint16_t 	TMR1_ms_cnt;



///////// ��ʼ�����Ź� /////////
void WDT_Init(void)
{
    SYS_UnlockReg();

    //WDT_Open(WDT_TIMEOUT_2POW14, WDT_RESET_DELAY_18CLK, TRUE, FALSE);
	WDT_Open(WDT_TIMEOUT_2POW16, WDT_RESET_DELAY_18CLK, TRUE, FALSE);
    //WDT_EnableInt();
    //NVIC_EnableIRQ(WDT_IRQn);

    SYS_LockReg();
}

///////// ִ��ι����������ֹMCU��λ /////////
void WDT_CR_set(void)
{
	SYS_UnlockReg();
	
	WDT_RESET_COUNTER(); 
	
	SYS_LockReg();
	//printf(" WDT:%d ",TMR0_ms_cnt);
}

///////// ι��--��ѯ /////////
void WDT_poll(void)
{
	static uint16_t WDTtim_cnt = 0;
	if( TMR1_ms_cnt >= WDTtim_cnt )
	{
		if( (TMR1_ms_cnt-WDTtim_cnt)>= 100)	//1S
		{
			WDTtim_cnt = TMR1_ms_cnt;
			WDT_CR_set();
		}
	}
	else if( (TMR1_ms_cnt+60000-WDTtim_cnt)>= 100)	//1S
	{
		WDTtim_cnt = TMR1_ms_cnt;
		WDT_CR_set();
	}
}

///////// ��λCPU /////////
void CPU_rst(void)
{
	SYS_UnlockReg();

	SYS_ResetCPU();		//����λCPU�ں˺�Flash�洢������(FMC)
	//SYS_ResetChip(); 	//���ϵ�ʱһ��Ч��

	SYS_LockReg();
}


/////////////// mS��ʱ ///////////////
void delay_ms(uint16_t ms_dat)
{
	while(ms_dat--)
	{
		delay_us(1000);
	}
}



