#ifndef	_IR_APP_H_
#define	_IR_APP_H_

#include "M0518.h"


#define IR_MOD		PA4		//红外接收的模式 RC5/NEC
#define IR_ACT		PB15	

#define IR_FR_LEN	10		//红外接收帧的长度


typedef struct
{
	uint8_t  rx_flag;			//接收数据有效标记
	uint8_t  buf_dat[IR_FR_LEN];		//接收的数据
}sIR_datFR;	//红外遥控接收相关


//// 红外遥控器按键的定义 ////
typedef enum
{
	IR_KEY_ONOFF	= 0x47,		//开关机
	IR_KEY_MOD		= 0x44,		//模式
	IR_KEY_SPEED	= 0x07,		//风量调节
	IR_KEY_TIM		= 0x16,		//定时
	IR_KEY_HOT		= 0x0C,		//热风
}IR_KEY_COM;



extern sIR_datFR IR_datFR;


void IR_init(void);
void IR_RXbuff_transfer(uint8_t *pdat, uint8_t slen);
void IR_RXrun_poll(void);


#endif


