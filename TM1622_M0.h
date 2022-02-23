#ifndef	_TM1622_M0_H_
#define	_TM1622_M0_H_

#include "M0518.h"


#define TM16_CS     PB13
#define TM16_RD     PB12
#define TM16_WR     PB6
#define TM16_DATA   PB7


//// 段码屏驱动芯片指令类型的定义 ////
typedef enum
{
	TM16_TYPE_READ		= 0x06,		//读 110B
	TM16_TYPE_WRITE		= 0x05,		//写 101B
	TM16_TYPE_W_R		= 0x05,		//读和写 101B
	TM16_TYPE_COMMAND	= 0x04,		//命令 100B
}TM16_TYPE_COM;

//// 段码屏驱动芯片指令的定义 ////
typedef enum
{
	TM16COM_SYS_DISD		= 0x00,		//关闭系统振荡器和 LCD 偏压发生器
	TM16COM_SYS_EN		= 0x01,		//打开系统振荡器
	TM16COM_LCD_OFF		= 0x02,		//关闭 LCD 显示
	TM16COM_LCD_ON		= 0x03,		//开启 LCD 显示
	TM16COM_RC32K			= 0x18,		//系统时钟源: 片内 RC 振荡器
	TM16COM_NORMAL		= 0xE3,		//正常模式
	TM16COM_F1				= 0xA0,		//时基本输出频率：1Hz
	TM16COM_F32			= 0xA5,		//时基本输出频率：32Hz
	TM16COM_F64			= 0xA6,		//时基本输出频率：64Hz
	TM16COM_F128			= 0xA7,		//时基本输出频率：128Hz
}TM16_COM;


void TM16_init(void);
void TM16_wirteDAT_4N(uint8_t addr, uint8_t *pdat, uint8_t slen);
void TM16_wirteDAT_8N(uint8_t addr, uint8_t *pdat, uint8_t slen);
void TM16_display_all(uint8_t sdat);


#endif


