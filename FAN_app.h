#ifndef	_FAN_APP_H_
#define	_FAN_APP_H_

#include "M0518.h"


#define FAN_SPEED_MIN		15		//最小的运行风量-百分比 25=25%
#define FAN_SPEED_MAX		45		//最大的运行风量-百分比 25=25%
#define FAN_LEVEL_MAX		4		//总共的档位数
#define FAN_M3H_MAX			150		//风机限制的最大风量 m3/h

#define FAN_SPEED_SLEEP		FAN_SPEED_MIN		//睡眠模式的百分比
#define FAN_SPEED_STRONG		FAN_SPEED_MAX		//强力模式的百分比

#define FAN_DIFF_ADD		5		//PWM每一调整一次的PWM百分比 增加的值
#define FAN_DIFF_CUT		7		//PWM每一调整一次的PWM百分比 减少的值

#define SENSOR_MEM_NUM	5		//用于自动调节存储传感器数据的个数

typedef struct
{
	uint8_t 	dev_onoff;		//新风机开关机标记 0=关闭，1=开启
	uint8_t 	hot_air;		//热风的档位 0表示关闭
	uint8_t 	dev_mod;		//设备的模式
	//uint32_t	timing_cnt;		//定时关机向下计数 1=10mS
	uint8_t		loop_mod;		//内外循坏模式 0=外循坏 1=内循坏
	uint8_t		fan_level;		//外循环的档位 0表示关闭
	uint8_t		fan_level2;		//内循环的档位 0表示关闭
	uint8_t		bw_flag;		//光照传感器 白天1/晚上0 标记
	
	uint8_t		ctrl_percen;	//进风风机 需要控制的当前百分比
	uint8_t		ctrl_percen2;	//内循环风机 需要控制的当前百分比
	uint8_t		speed_gap;		//每一个档位的间隔 (FAN_SPEED_MAX-FAN_SPEED_MIN)/(FAN_LEVEL_MAX-1)得到
	uint8_t		target_percen;	//自动调整时的目标 风机百分比
	//uint8_t		fan_flag;		//需要调整风量的标记
	uint16_t	fan_tim_cnt;	//定时器计数 向下计数
	uint32_t	off_tim_cnt;	//定时关机计数 向下计数 1=10mS
	uint8_t		off_flag;		//定时关机标记
}sFan_DAT;

extern sFan_DAT Fan_DAT;


//// 新风机的模式定义 ////
typedef enum
{
	FANMOD_MANUAL		= 0xA0,			//手动模式
	FANMOD_AUTO			= 0xA1,			//自动/智能模式
	FANMOD_SLEEP		= 0xA2,			//睡眠模式
	FANMOD_STRONG		= 0xA3,			//强力模式
}FAN_MOD;



void FAN_init(void);
void FAN_mod_set(uint8_t mod_dat);
void FAN_mod_next(void);
uint8_t FAN_percen_to_level(uint8_t percen);
uint8_t FAN_level_to_percen(uint8_t level);
void FAN_run_poll(void);
void FAN_adj_sensor(uint16_t pm25_dat, uint16_t co2_dat, uint16_t ch2o_dat);
void FAN_tim_poll(void);
void FANall_on_set(void);
void FANall_off_set(void);

void FAN_set_level(void);
void FAN_set_OFFtim(void);
void FAN_set_HOTair(void);


#endif



