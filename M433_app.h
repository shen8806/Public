#ifndef	_M433_APP_H_
#define	_M433_APP_H_

#include "M0518.h"
#include "DataFlash.h"


#define RF_FR_LEN		32						//֡�ĳ���
#define RF_SDAT_LEN	16						//֡�����ݵĳ���

#define RX_BUFF_SIZE   	5						//RF�����ջ��������İ���(һ��Ϊһ֡���ݳ���)
#define RX_PACKET_LEN	( RF_FR_LEN + 3 )		//RF�����ջ��������ĳ���(һ֡����+����ֵ+CRC+���)

#define RF_FIR_B0		0xFF					//��ͷ���ֽ�0
#define RF_FIR_B1		0xFA					//��ͷ���ֽ�1
#define RF_END_B0		0xFF					//��β���ֽ�0
#define RF_END_B1		0xFE					//��β���ֽ�1


#define RF_TYPE_NULL				0				//RF����--��Ч
#define RF_TYPE_ONESELF			1				//RF����--�����豸
#define RF_TYPE_BROADCAST01		2				//RF����--ȫ���豸�㲥
#define RF_TYPE_BROADCAST02		3				//RF����--�����豸�㲥


#define RXBUFF_OK_FLAG   		0x0F				//����洢 ��Ч ���λ
#define RXBUFF_NULL_FLAG   	0x00				//����洢 ��Ч ���λ

#define DF_UPFALG_ADDR		0x400				//�����ϱ����FLASH�洢��ַ


typedef struct
{
	uint8_t 	fr_first[2];				//��ͷ
	uint8_t 	dev_addr[RF_ADDR_LEN];		//�豸ID(���Ͷ�) 8�ֽ�
	uint8_t		com;						//ָ��
	uint8_t		length; 					//���ݳ���
	uint8_t		sdata[RF_SDAT_LEN];			//������
	uint8_t		checksum[2];				//CRC16У��
	uint8_t		fr_end[2];					//��β
}sRF_FR; ////RF ֡ ����


typedef union
{
	uint8_t 		buff[RF_FR_LEN];
	sRF_FR 			RFu_fr;
}uRF_uFR;


typedef struct
{
	uint8_t 	rawRxBuff[RX_PACKET_LEN];      				//ԭʼ���ջ�����
	uint8_t 	rxBuff[RX_BUFF_SIZE][RX_PACKET_LEN];		//������󻺳� 10������
	uint8_t 	len[RX_BUFF_SIZE];							//���յ�֡����
	uint8_t 	ok_flag[RX_BUFF_SIZE];						//������Ч���
	uint8_t 	rxBuf_num;									//�����
	uint8_t 	rxRun_cnt;									//���ڴ���Ľ��ջ����
}tRF_RXBUFF_STR;		//RF���ջ������


typedef struct
{
	uint8_t		rx_flag;			//������Ч���
	uint8_t		rx_type;			//֡���յ�����
	
	uint8_t 	tx_flag;			//RF���ͱ��
	uint8_t 	txrx_ing_flag;		//RF�շ������б��

	uint8_t		rx_status;			//���յ�״̬
	uint8_t		u8_dat[10];		//��ŵ���ʱ����
	uint8_t		tx_cod_cnt;		//���͵ı��

	uint8_t		up_flag;			//�����ϱ����	0:����ʽ 1:�����ϱ� 
	uint16_t	up_gap;				//�����ϱ�ʱ���� 1=10mS
	uint16_t	up_tim_cnt;		//�����ϴ��� ��ʱ������ ���¼���
	uint8_t		up_sensor_flag01;	//�����ϱ���������1���
	uint8_t		up_sensor_flag02;	//�����ϱ���������2���

	//
	//uint8_t 	rx_len; 			//���յ�RF���ݳ���
	//uint8_t		Channel;			//433Ƶ����
}sRF_TYPE_FR; ////RF֡״̬���



//// RF֡�����ߴ���ָ��� ////
typedef enum
{
	RF_SET_SPEED_IN		= 0x92,			//������ ��������
	RF_SET_SPEED_IN_ACK	= 0x94,			//������ �������� Ӧ��
	RF_SET_SPEED_OUT		= 0x21,			//�ŷ�� ��������
	RF_SET_SPEED_OUT_ACK	= 0x22,			//�ŷ��� �������� Ӧ��
	RF_IQU_SENSOR			= 0x23,			//��ѯ�豸�Ĵ���������ֵ
	RF_IQU_SENSOR_ACK		= 0x24,			//��ѯ�豸�Ĵ���������ֵ Ӧ��
	RF_IQU_SPEED			= 0x25,			//��ѯ����ķ�����ֵ
	RF_IQU_SPEED_ACK		= 0x26,			//��ѯ����ķ�����ֵ Ӧ��
	RF_SET_SENSORUP		= 0x27,			//���ô�����Ϊ�����ϱ�
	
	RF_SET_ACK				= 0xFF,			//Ӧ��ָ��
}RF_COM;

//// ֡��Ӧ��״̬�Ķ��� ////
typedef enum
{
	FR_ACK_OK			= 0xFF,	//�ɹ� (�̶�����)
	FR_ACK_ERR			= 0x00,	//ʧ�� (�̶�����)
	FR_ACK_ERR_DAT	= 0x01,	//ʧ��-���ݸ�ʽ����
	FR_ACK_ERR_COM	= 0x02,	//ʧ��-��Чָ��
	FR_ACK_ERR_LEN	= 0x03,	//ʧ��-���Ȳ���
	FR_ACK_ERR_CNF	= 0xA0,	//ʧ��-����ʧ��
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


