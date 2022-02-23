#ifndef	_UART_CTRL_H_
#define	_UART_CTRL_H_

#include "M0518.h"


#define UART_FIR_B0		0xAA			//包头的字节0
#define UART_FIR_B1		0x55			//包头的字节1
#define UART_FIR_ADDR	0x11			//显示端 源端地址

#define UART_FR_LEN		32				//帧的长度
#define UART_SDAT_LEN	25				//帧中数据的长度

#define UART_BUFF_SIZE   	10					//RF接收收缓冲区最大的包数(一包为一帧数据长度)
#define UART_PACKET_LEN	( UART_FR_LEN + 2 )		//RF接收收缓冲区最大的长度

#define UARTBUFF_OK_FLAG   	0x0F				//缓存存储 有效 标记位
#define UARTBUFF_NULL_FLAG  0x00				//缓存存储 无效 标记位


typedef struct
{
	uint8_t 	fr_first[2];				//包头
	uint8_t 	dev_addr;					//源端地址
	uint8_t		com;						//指令
	uint8_t		length; 					//数据长度
	uint8_t		sdata[UART_SDAT_LEN];		//数据域
	uint8_t		checksum[2];				//CRC16校验
}sUART_FR; ////UART帧 定义

typedef union
{
	uint8_t 	buff[UART_FR_LEN];
	sUART_FR 	UART_FR;
}uUART_uFR;


typedef struct
{
	//uint8_t 	rawRxBuff[UART_PACKET_LEN];      			//原始接收缓冲区
	uint8_t 	rxBuff[UART_BUFF_SIZE][UART_PACKET_LEN];	//接收最大缓冲
	uint8_t  	len[UART_BUFF_SIZE];						//帧长度
	uint8_t  	ok_flag[UART_BUFF_SIZE];					//缓存标记

	uint8_t 	rxBuf_num;									//缓冲号
	uint8_t 	rxRun_cnt;									//正在处理的接收缓冲号
}tUART_RXBUFF_STR;		//UART接收缓冲相关


typedef struct
{
	uint8_t		rx_flag;			//接收有效标记
	uint8_t		rx_ack_com;			//ACK应答的指令码保存
	uint8_t		rx_ack_ok;			//ACK应答成功标记
	uint8_t		tx_flag;			//发送标记
	uint8_t		u8_dat[8];			//应答时需要临时存储的缓存
	uint8_t		ack_status;			//应答的状态号
	uint16_t	tim_cnt;			//定时计数 向下计数
}sCTRL_TYPE_FR; ////帧状态相关


typedef struct
{
	uint16_t 	temperature;	//室内 温度,最高位1为负数，7位为整数，低8位是小数
	uint8_t		humidity;		//室内 湿度  	单位:%
	uint16_t 	temperature_W;	//室外 温度,最高位1为负数，7位为整数，低8位是小数
	uint8_t		humidity_W;		//室外 湿度  	单位:%
	uint16_t	PM2_5;			//PM2.5		单位:ug/m3
	uint16_t	CO2;			//CO2		单位:ppm
	uint16_t	CH2O_ug;		//甲醛		单位:ug/m3:
	//uint8_t		fan1_percen;	//进风风机的百分比 %
	uint16_t	AQI_dat;		//得出的室内AQI值
}sSensor_DAT;


//// UART帧的传输指令定义 ////
typedef enum
{
	UART_SENSOR_IQU		= 0x20,			//查询传感器的数值
	UART_SENSOR_IQU_ACK	= 0x21,			//查询传感器的数值 应答
	UART_SPEED_SET		= 0x22,			//风机控制
	UART_SPEED_SET_ACK	= 0x23,			//风机控制 应答
	UART_HEATING_SET	= 0x24,			//热风控制（加热功能）
	UART_SET_ACK		= 0xFF,			//应答指令
}UART_COM;


//// 传感器类型的定义 ////
typedef enum
{
//bit7位是室内室外标记，0:室内，1:室外
	SENSOR_TYPE_PM25	= 0x01,		//PM2.5
	SENSOR_TYPE_CO2		= 0x02,		//CO2
	SENSOR_TYPE_CH2O	= 0x03,		//CH2O 甲醛
	SENSOR_TYPE_TEM		= 0x04,		//温度
	SENSOR_TYPE_RH		= 0x05,		//湿度
	SENSOR_TYPE_SPEED	= 0x06,		//风量
	SENSOR_TYPE_PERCEN	= 0x07,		//风机百分比
	SENSOR_TYPE_HOT		= 0x08,		//热风/加热
}SENSOR_TYPE_COM;




extern tUART_RXBUFF_STR CTRL_RXBUFF_STR;
extern uUART_uFR UART_uFR_RX;
extern uUART_uFR UART_uFR_TX;
extern sCTRL_TYPE_FR CTRL_TYPE_FR;
extern sSensor_DAT 	Sensor_DAT;

uint16_t Calculate_CRC16( uint8_t *pdat, uint8_t slen);
void CTRL_RXbuff_transfer(uint8_t *pdat, uint8_t slen);
void CTRL_init(void);
void CTRL_frRX_poll(void);
void CTRL_TX_poll(void);
void CTRL_speed_set(uint8_t percen);
void CTRL_tim_poll(void);
void CTRL_sensor_get(void);


#endif

