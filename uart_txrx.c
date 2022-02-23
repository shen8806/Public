#include "public.h"

//#include "uart_txrx.h"
//#include <stdio.h>


sUART_RX_fr UART1_RX_fr;
sUART_RX_fr UART2_RX_fr;
sUART_RX_fr UART3_RX_fr;
sUART_RX_fr UART4_RX_fr;


/////////////// UART1初始化 ///////////////
void UART1_Init(void)
{
	SYS_ResetModule(UART1_RST);
	UART_Open(UART1, 38400);
	UART_EnableInt(UART1, UART_IER_RDA_IEN_Msk);
}

/////////////// UART2初始化 ///////////////
void UART2_Init(void)
{
	SYS_ResetModule(UART2_RST);
	UART_Open(UART2, 38400);
	UART_EnableInt(UART2, UART_IER_RDA_IEN_Msk);
}

/////////////// UART3初始化 /////////////// 
void UART3_Init(void)
{
	SYS_ResetModule(UART3_RST);
	UART_Open(UART3, 9600);
	UART_EnableInt(UART3, UART_IER_RDA_IEN_Msk);
}

/////////////// UART4初始化 /////////////// 
void UART4_Init(void)
{
	SYS_ResetModule(UART4_RST);
	UART_Open(UART4, 9600);
	UART_EnableInt(UART4, UART_IER_RDA_IEN_Msk);
}

/////////////// 用到所有UART初始化 /////////////// 
void UART_all_Init(void)
{
	UART1_Init();
	UART2_Init();
	UART3_Init();
	UART4_Init();
}

/////////////// UART发送N个字节数据 /////////////// 
void UART_SendBuff( UART_T* uart, uint8_t *buff, uint8_t slen )
{
	//uint8_t i;
	UART_Write(uart, buff, slen);
	//UART_Write(UART3, buff, slen);

	/*
	printf("## UART TX: ");
	for(i=0; i<slen; i++)
	{
		printf("%.2X",buff[i]);
	}
	printf("\r\n");
	*/
}


/////////////// UART超时计数 10ms间隔 /////////////// 
void UART_RXtime_poll(void)
{
	if( UART1_RX_fr.rx_time )
		UART1_RX_fr.rx_time--;
	else if( UART1_RX_fr.rx_cnt>0)
	{
		CTRL_RXbuff_transfer( UART1_RX_fr.rx_buff ,UART1_RX_fr.rx_cnt );
		UART1_RX_fr.rx_cnt = 0;
	}


	if( UART2_RX_fr.rx_time )
		UART2_RX_fr.rx_time--;
	else if( UART2_RX_fr.rx_cnt>0)
	{
		M433_RXbuff_transfer( UART2_RX_fr.rx_buff ,UART2_RX_fr.rx_cnt );
		UART2_RX_fr.rx_cnt = 0;
	}


	if( UART3_RX_fr.rx_time )
		UART3_RX_fr.rx_time--;
	else if( UART3_RX_fr.rx_cnt>0)
	{
		//WIFI_RXbuff_transfer( UART3_RX_fr.rx_buff ,UART3_RX_fr.rx_cnt );
		UART3_RX_fr.rx_cnt = 0;
	}

	if( UART4_RX_fr.rx_time )
		UART4_RX_fr.rx_time--;
	else if( UART4_RX_fr.rx_cnt>0)
	{
		IR_RXbuff_transfer( UART4_RX_fr.rx_buff ,UART4_RX_fr.rx_cnt );
		UART4_RX_fr.rx_cnt = 0;
	}
}

/////////////// UART1中断入口 (函数由startup_M0518.s引入)  /////////////// 
void UART1_IRQHandler(void)
{
	if(UART_GET_INT_FLAG(UART1,UART_ISR_RDA_INT_Msk))
	{
		while(UART_IS_RX_READY(UART1)) 
        { 
			UART1_RX_fr.rx_buff[UART1_RX_fr.rx_cnt] = UART_READ(UART1);
		}
		
		UART1_RX_fr.rx_cnt++;
		if(UART1_RX_fr.rx_cnt>=UART_FRAME_LEN)
			UART1_RX_fr.rx_cnt = UART_FRAME_LEN-1;
		UART1_RX_fr.rx_time = UART_TIMEOUT_50MS;
	}
}

/////////////// UART2中断入口 /////////////// 
void UART02_IRQHandler(void)
{
	if(UART_GET_INT_FLAG(UART2,UART_ISR_RDA_INT_Msk))
	{
		while(UART_IS_RX_READY(UART2)) 
        { 
			UART2_RX_fr.rx_buff[UART2_RX_fr.rx_cnt] = UART_READ(UART2);
		}
		
		UART2_RX_fr.rx_cnt++;
		if(UART2_RX_fr.rx_cnt>=UART_FRAME_LEN)
			UART2_RX_fr.rx_cnt = UART_FRAME_LEN-1;
		UART2_RX_fr.rx_time = UART_TIMEOUT_50MS;
	}
}

/////////////// UART3中断入口 /////////////// 
void UART3_IRQHandler(void)
{
	if(UART_GET_INT_FLAG(UART3,UART_ISR_RDA_INT_Msk))
	{
		while(UART_IS_RX_READY(UART3)) 
        { 
			UART3_RX_fr.rx_buff[UART3_RX_fr.rx_cnt] = UART_READ(UART3);
		}
		
		UART3_RX_fr.rx_cnt++;
		if(UART3_RX_fr.rx_cnt>=UART_FRAME_LEN)
			UART3_RX_fr.rx_cnt = UART_FRAME_LEN-1;
		UART3_RX_fr.rx_time = UART_TIMEOUT_50MS;
	}
}

/////////////// UART4中断入口 /////////////// 
void UART4_IRQHandler(void)
{
	if(UART_GET_INT_FLAG(UART4,UART_ISR_RDA_INT_Msk))
	{
		while(UART_IS_RX_READY(UART4)) 
        { 
			UART4_RX_fr.rx_buff[UART4_RX_fr.rx_cnt] = UART_READ(UART4);
		}
		
		UART4_RX_fr.rx_cnt++;
		if(UART4_RX_fr.rx_cnt>=UART_FRAME_LEN)
			UART4_RX_fr.rx_cnt = UART_FRAME_LEN-1;
		UART4_RX_fr.rx_time = UART_TIMEOUT_50MS;
	}
}






