#ifndef	_M433_APP_H_
#define	_M433_APP_H_

#include "M0518.h"
#include "DataFlash.h"


#define RF_FR_LEN		32						//帧的长度
#define RF_SDAT_LEN	16						//帧中数据的长度

#define RX_BUFF_SIZE   	5						//RF接收收缓冲区最大的包数(一包为一帧数据长度)
#define RX_PACKET_LEN	( RF_FR_LEN + 3 )		//RF接收收缓冲区最大的长度(一帧数据+长度值+CRC+标记)

#define RF_FIR_B0		0xFF					//包头的字节0
#define RF_FIR_B1		0xFA					//包头的字节1
#define RF_END_B0		0xFF					//包尾的字节0
#define RF_END_B1		0xFE					//包尾的字节1


#define RF_TYPE_NULL				0				//RF类型--无效
#define RF_TYPE_ONESELF			1				//RF类型--本身设备
#define RF_TYPE_BROADCAST01		2				//RF类型--全部设备广播
#define RF_TYPE_BROADCAST02		3				//RF类型--该类设备广播


#define RXBUFF_OK_FLAG   		0x0F				//缓存存储 有效 标记位
#define RXBUFF_NULL_FLAG   	0x00				//缓存存储 无效 标记位

#define DF_UPFALG_ADDR		0x400				//主动上报标记FLASH存储地址


typedef struct
{
	uint8_t 	fr_first[2];				//包头
	uint8_t 	dev_addr[RF_ADDR_LEN];		//设备ID(发送端) 8字节
	uint8_t		com;						//指令
	uint8_t		length; 					//数据长度
	uint8_t		sdata[RF_SDAT_LEN];			//数据域
	uint8_t		checksum[2];				//CRC16校验
	uint8_t		fr_end[2];					//包尾
}sRF_FR; ////RF 帧 定义


typedef union
{
	uint8_t 		buff[RF_FR_LEN];
	sRF_FR 			RFu_fr;
}uRF_uFR;


typedef struct
{
	uint8_t 	rawRxBuff[RX_PACKET_LEN];      				//原始接收缓冲区
	uint8_t 	rxBuff[RX_BUFF_SIZE][RX_PACKET_LEN];		//接收最大缓冲 10条数据
	uint8_t 	len[RX_BUFF_SIZE];							//接收的帧长度
	uint8_t 	ok_flag[RX_BUFF_SIZE];						//接收有效标记
	uint8_t 	rxBuf_num;									//缓冲号
	uint8_t 	rxRun_cnt;									//正在处理的接收缓冲号
}tRF_RXBUFF_STR;		//RF接收缓冲相关


typedef struct
{
	uint8_t		rx_flag;			//接收有效标记
	uint8_t		rx_type;			//帧接收的类型
	
	uint8_t 	tx_flag;			//RF发送标记
	uint8_t 	txrx_ing_flag;		//RF收发过程中标记

	uint8_t		rx_status;			//接收的状态
	uint8_t		u8_dat[10];		//存放的临时缓存
	uint8_t		tx_cod_cnt;		//发送的编号

	uint8_t		up_flag;			//主动上报标记	0:被动式 1:主动上报 
	uint16_t	up_gap;				//主动上报时间间隔 1=10mS
	uint16_t	up_tim_cnt;		//主动上传的 定时器计数 向下计数
	uint8_t		up_sensor_flag01;	//主动上报传感器类1标记
	uint8_t		up_sensor_flag02;	//主动上报传感器类2标记

	//
	//uint8_t 	rx_len; 			//接收的RF数据长度
	//uint8_t		Channel;			//433频道号
}sRF_TYPE_FR; ////RF帧状态相关



//// RF帧的无线传输指令定义 ////
typedef enum
{
	RF_SET_SPEED_IN		= 0x92,			//进风风机 风量控制
	RF_SET_SPEED_IN_ACK	= 0x94,			//进风风机 风量控制 应答
	RF_SET_SPEED_OUT		= 0x21,			//排风机 风量控制
	RF_SET_SPEED_OUT_ACK	= 0x22,			//排风风机 风量控制 应答
	RF_IQU_SENSOR			= 0x23,			//查询设备的传感器器数值
	RF_IQU_SENSOR_ACK		= 0x24,			//查询设备的传感器器数值 应答
	RF_IQU_SPEED			= 0x25,			//查询风机的风量数值
	RF_IQU_SPEED_ACK		= 0x26,			//查询风机的风量数值 应答
	RF_SET_SENSORUP		= 0x27,			//配置传感器为主动上报
	
	RF_SET_ACK				= 0xFF,			//应答指令
}RF_COM;

//// 帧的应答状态的定义 ////
typedef enum
{
	FR_ACK_OK			= 0xFF,	//成功 (固定不变)
	FR_ACK_ERR			= 0x00,	//失败 (固定不变)
	FR_ACK_ERR_DAT	= 0x01,	//失败-数据格式错误
	FR_ACK_ERR_COM	= 0x02,	//失败-无效指令
	FR_ACK_ERR_LEN	= 0x03,	//失败-长度不对
	FR_ACK_ERR_CNF	= 0xA0,	//失败-配置失败
}FR_ACK;



extern uRF_uFR 		RF_RX_uFR;
extern uRF_uFR 		RF_TX_uFR;
extern tRF_RXBUFF_STR	RF_RXBUFF_STR;
extern sRF_TYPE_FR	RF_TYPE_FR;


void M433_fr_init(void);
void M433_COM_set(void);
void M433_COM_reset_def(void);
void M433_RXbuff_transfer(uint8_t *pdat, uint8_t slen);
void RF_RX_poll(void);
void RF_TX_poll(void);
void RF_tim_poll(void);
void RF_sensor_up(void);


#endif


