#ifndef	_KEY_H_
#define	_KEY_H_

#include "M0518.h"


#define KEY1      	PF5
#define KEY2      	PF4
#define KEY3      	PA11
#define KEY4      	PA10
#define KEY5      	PA9
#define KEY6      	PA8

//�����Ŷ�Ӧ��ϵ 0..5
#define KEY_ONOFF	5	//����
#define KEY_LOOP	4	//ѭ��(����ѭ��)
#define KEY_MOD		3	//ģʽ
#define KEY_LEVEL	2	//����(��λ)
#define KEY_TIM		1	//��ʱ
#define KEY_HOT		0	//�ȷ�


#define KEY_MAX_NUM		6		//��������

#define KEY_VALUE_NULL		0		//δ����
#define KEY_VALUE_PRESS		1		//����
#define KEY_VALUE_RELEASE	2		//�ɿ�

#define KEY_TYPE_NULL		0		//ƽʱ״̬
#define KEY_TYPE_SHORT		1		//�̰�
#define KEY_TYPE_LONG		2		//����
#define KEY_TYPE_SLONG		3		//�����ţ�δ�ɿ�

#define KEY_TIME_SHORT		2		//�̰�����ʱ�� X*10mS
#define KEY_TIME_LONG		300		//��������ʱ�� X*10mS


typedef struct
{
	uint8_t 	value;		//����״̬�����¡��ɿ�
	uint8_t 	type;		//��ǰ״̬�� �������ͣ��̰�������
	uint8_t 	type_old;	//��һ״̬�� ��������
	uint16_t	timer;		//���µ�ʱ��
	uint8_t		bin_10;		//�����ĵ�ƽ״̬
}tKey_Information;

extern tKey_Information sKey_Information[KEY_MAX_NUM];


void key_init(void);
uint8_t key_type_get(uint8_t key_num);
void key_type_clr(uint8_t key_num);
void key_scan_poll(void);
void key_run_poll(void);



#endif

