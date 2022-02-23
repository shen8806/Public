#ifndef	_UART_TXRX_H_
#define	_UART_TXRX_H_

#include "M0518.h"


#define CTRL_UART 		UART1
#define M433_UART 		UART2
#define WIFI_UART 		UART3
#define IR_UART 		UART4


#define UART_FRAME_LEN		40		//UART临时缓冲的最大长度
#define UART_TIMEOUT_50MS	5		//超时时间--30mS


typedef struct
{
	uint8_t 	rx_cnt;
	uint8_t		rx_buff[UART_FRAME_LEN];
	uint16_t 	rx_time;
}sUART_RX_fr;


void UART_all_Init(void);
void UART_SendBuff( UART_T* uart, uint8_t *buff, uint8_t slen );
void UART_RXtime_poll(void);
void CTRL_TX_set(uint8_t sCom);



#endif


