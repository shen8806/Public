#ifndef	_UART_CTRL_H_
#define	_UART_CTRL_H_

#include "M0518.h"


#define UART_FIR_B0		0xAA			//��ͷ���ֽ�0
#define UART_FIR_B1		0x55			//��ͷ���ֽ�1
#define UART_FIR_ADDR	0x11			//��ʾ�� Դ�˵�ַ

#define UART_FR_LEN		32				//֡�ĳ���
#define UART_SDAT_LEN	25				//֡�����ݵĳ���

#define UART_BUFF_SIZE   	10					//RF�����ջ��������İ���(һ��Ϊһ֡���ݳ���)
#define UART_PACKET_LEN	( UART_FR_LEN + 2 )		//RF�����ջ��������ĳ���

#define UARTBUFF_OK_FLAG   	0x0F				//����洢 ��Ч ���λ
#define UARTBUFF_NULL_FLAG  0x00				//����洢 ��Ч ���λ


typedef struct
{
	uint8_t 	fr_first[2];				//��ͷ
	uint8_t 	dev_addr;					//Դ�˵�ַ
	uint8_t		com;						//ָ��
	uint8_t		length; 					//���ݳ���
	uint8_t		sdata[UART_SDAT_LEN];		//������
	uint8_t		checksum[2];				//CRC16У��
}sUART_FR; ////UART֡ ����

typedef union
{
	uint8_t 	buff[UART_FR_LEN];
	sUART_FR 	UART_FR;
}uUART_uFR;


typedef struct
{
	//uint8_t 	rawRxBuff[UART_PACKET_LEN];      			//ԭʼ���ջ�����
	uint8_t 	rxBuff[UART_BUFF_SIZE][UART_PACKET_LEN];	//������󻺳�
	uint8_t  	len[UART_BUFF_SIZE];						//֡����
	uint8_t  	ok_flag[UART_BUFF_SIZE];					//������

	uint8_t 	rxBuf_num;									//�����
	uint8_t 	rxRun_cnt;									//���ڴ���Ľ��ջ����
}tUART_RXBUFF_STR;		//UART���ջ������


typedef struct
{
	uint8_t		rx_flag;			//������Ч���
	uint8_t		rx_ack_com;			//ACKӦ���ָ���뱣��
	uint8_t		rx_ack_ok;			//ACKӦ��ɹ����
	uint8_t		tx_flag;			//���ͱ��
	uint8_t		u8_dat[8];			//Ӧ��ʱ��Ҫ��ʱ�洢�Ļ���
	uint8_t		ack_status;			//Ӧ���״̬��
	uint16_t	tim_cnt;			//��ʱ���� ���¼���
}sCTRL_TYPE_FR; ////֡״̬���


typedef struct
{
	uint16_t 	temperature;	//���� �¶�,���λ1Ϊ������7λΪ��������8λ��С��
	uint8_t		humidity;		//���� ʪ��  	��λ:%
	uint16_t 	temperature_W;	//���� �¶�,���λ1Ϊ������7λΪ��������8λ��С��
	uint8_t		humidity_W;		//���� ʪ��  	��λ:%
	uint16_t	PM2_5;			//PM2.5		��λ:ug/m3
	uint16_t	CO2;			//CO2		��λ:ppm
	uint16_t	CH2O_ug;		//��ȩ		��λ:ug/m3:
	//uint8_t		fan1_percen;	//�������İٷֱ� %
	uint16_t	AQI_dat;		//�ó�������AQIֵ
}sSensor_DAT;


//// UART֡�Ĵ���ָ��� ////
typedef enum
{
	UART_SENSOR_IQU		= 0x20,			//��ѯ����������ֵ
	UART_SENSOR_IQU_ACK	= 0x21,			//��ѯ����������ֵ Ӧ��
	UART_SPEED_SET		= 0x22,			//�������
	UART_SPEED_SET_ACK	= 0x23,			//������� Ӧ��
	UART_HEATING_SET	= 0x24,			//�ȷ���ƣ����ȹ��ܣ�
	UART_SET_ACK		= 0xFF,			//Ӧ��ָ��
}UART_COM;


//// ���������͵Ķ��� ////
typedef enum
{
//bit7λ�����������ǣ�0:���ڣ�1:����
	SENSOR_TYPE_PM25	= 0x01,		//PM2.5
	SENSOR_TYPE_CO2		= 0x02,		//CO2
	SENSOR_TYPE_CH2O	= 0x03,		//CH2O ��ȩ
	SENSOR_TYPE_TEM		= 0x04,		//�¶�
	SENSOR_TYPE_RH		= 0x05,		//ʪ��
	SENSOR_TYPE_SPEED	= 0x06,		//����
	SENSOR_TYPE_PERCEN	= 0x07,		//����ٷֱ�
	SENSOR_TYPE_HOT		= 0x08,		//�ȷ�/����
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

