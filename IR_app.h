#ifndef	_IR_APP_H_
#define	_IR_APP_H_

#include "M0518.h"


#define IR_MOD		PA4		//������յ�ģʽ RC5/NEC
#define IR_ACT		PB15	

#define IR_FR_LEN	10		//�������֡�ĳ���


typedef struct
{
	uint8_t  rx_flag;			//����������Ч���
	uint8_t  buf_dat[IR_FR_LEN];		//���յ�����
}sIR_datFR;	//����ң�ؽ������


//// ����ң���������Ķ��� ////
typedef enum
{
	IR_KEY_ONOFF	= 0x47,		//���ػ�
	IR_KEY_MOD		= 0x44,		//ģʽ
	IR_KEY_SPEED	= 0x07,		//��������
	IR_KEY_TIM		= 0x16,		//��ʱ
	IR_KEY_HOT		= 0x0C,		//�ȷ�
}IR_KEY_COM;



extern sIR_datFR IR_datFR;


void IR_init(void);
void IR_RXbuff_transfer(uint8_t *pdat, uint8_t slen);
void IR_RXrun_poll(void);


#endif


