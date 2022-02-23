#ifndef	_FAN_APP_H_
#define	_FAN_APP_H_

#include "M0518.h"


#define FAN_SPEED_MIN		15		//��С�����з���-�ٷֱ� 25=25%
#define FAN_SPEED_MAX		45		//�������з���-�ٷֱ� 25=25%
#define FAN_LEVEL_MAX		4		//�ܹ��ĵ�λ��
#define FAN_M3H_MAX			150		//������Ƶ������� m3/h

#define FAN_SPEED_SLEEP		FAN_SPEED_MIN		//˯��ģʽ�İٷֱ�
#define FAN_SPEED_STRONG		FAN_SPEED_MAX		//ǿ��ģʽ�İٷֱ�

#define FAN_DIFF_ADD		5		//PWMÿһ����һ�ε�PWM�ٷֱ� ���ӵ�ֵ
#define FAN_DIFF_CUT		7		//PWMÿһ����һ�ε�PWM�ٷֱ� ���ٵ�ֵ

#define SENSOR_MEM_NUM	5		//�����Զ����ڴ洢���������ݵĸ���

typedef struct
{
	uint8_t 	dev_onoff;		//�·�����ػ���� 0=�رգ�1=����
	uint8_t 	hot_air;		//�ȷ�ĵ�λ 0��ʾ�ر�
	uint8_t 	dev_mod;		//�豸��ģʽ
	//uint32_t	timing_cnt;		//��ʱ�ػ����¼��� 1=10mS
	uint8_t		loop_mod;		//����ѭ��ģʽ 0=��ѭ�� 1=��ѭ��
	uint8_t		fan_level;		//��ѭ���ĵ�λ 0��ʾ�ر�
	uint8_t		fan_level2;		//��ѭ���ĵ�λ 0��ʾ�ر�
	uint8_t		bw_flag;		//���մ����� ����1/����0 ���
	
	uint8_t		ctrl_percen;	//������ ��Ҫ���Ƶĵ�ǰ�ٷֱ�
	uint8_t		ctrl_percen2;	//��ѭ����� ��Ҫ���Ƶĵ�ǰ�ٷֱ�
	uint8_t		speed_gap;		//ÿһ����λ�ļ�� (FAN_SPEED_MAX-FAN_SPEED_MIN)/(FAN_LEVEL_MAX-1)�õ�
	uint8_t		target_percen;	//�Զ�����ʱ��Ŀ�� ����ٷֱ�
	//uint8_t		fan_flag;		//��Ҫ���������ı��
	uint16_t	fan_tim_cnt;	//��ʱ������ ���¼���
	uint32_t	off_tim_cnt;	//��ʱ�ػ����� ���¼��� 1=10mS
	uint8_t		off_flag;		//��ʱ�ػ����
}sFan_DAT;

extern sFan_DAT Fan_DAT;


//// �·����ģʽ���� ////
typedef enum
{
	FANMOD_MANUAL		= 0xA0,			//�ֶ�ģʽ
	FANMOD_AUTO			= 0xA1,			//�Զ�/����ģʽ
	FANMOD_SLEEP		= 0xA2,			//˯��ģʽ
	FANMOD_STRONG		= 0xA3,			//ǿ��ģʽ
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



