#ifndef	_KEY_H_
#define	_KEY_H_

#include "M0518.h"


#define KEY1      	PF5
#define KEY2      	PF4
#define KEY3      	PA11
#define KEY4      	PA10
#define KEY5      	PA9
#define KEY6      	PA8

//按键号对应关系 0..5
#define KEY_ONOFF	5	//开关
#define KEY_LOOP	4	//循环(内外循环)
#define KEY_MOD		3	//模式
#define KEY_LEVEL	2	//风量(档位)
#define KEY_TIM		1	//定时
#define KEY_HOT		0	//热风


#define KEY_MAX_NUM		6		//按键数量

#define KEY_VALUE_NULL		0		//未按下
#define KEY_VALUE_PRESS		1		//按下
#define KEY_VALUE_RELEASE	2		//松开

#define KEY_TYPE_NULL		0		//平时状态
#define KEY_TYPE_SHORT		1		//短按
#define KEY_TYPE_LONG		2		//长按
#define KEY_TYPE_SLONG		3		//长按着，未松开

#define KEY_TIME_SHORT		2		//短按最少时间 X*10mS
#define KEY_TIME_LONG		300		//长按最少时间 X*10mS


typedef struct
{
	uint8_t 	value;		//按键状态，按下、松开
	uint8_t 	type;		//当前状态的 按键类型，短按、长按
	uint8_t 	type_old;	//上一状态的 按键类型
	uint16_t	timer;		//按下的时间
	uint8_t		bin_10;		//按键的电平状态
}tKey_Information;

extern tKey_Information sKey_Information[KEY_MAX_NUM];


void key_init(void);
uint8_t key_type_get(uint8_t key_num);
void key_type_clr(uint8_t key_num);
void key_scan_poll(void);
void key_run_poll(void);



#endif

