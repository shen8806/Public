#ifndef	_TM1622_M0_H_
#define	_TM1622_M0_H_

#include "M0518.h"


#define TM16_CS     PB13
#define TM16_RD     PB12
#define TM16_WR     PB6
#define TM16_DATA   PB7


//// ����������оƬָ�����͵Ķ��� ////
typedef enum
{
	TM16_TYPE_READ		= 0x06,		//�� 110B
	TM16_TYPE_WRITE		= 0x05,		//д 101B
	TM16_TYPE_W_R		= 0x05,		//����д 101B
	TM16_TYPE_COMMAND	= 0x04,		//���� 100B
}TM16_TYPE_COM;

//// ����������оƬָ��Ķ��� ////
typedef enum
{
	TM16COM_SYS_DISD		= 0x00,		//�ر�ϵͳ������ LCD ƫѹ������
	TM16COM_SYS_EN		= 0x01,		//��ϵͳ����
	TM16COM_LCD_OFF		= 0x02,		//�ر� LCD ��ʾ
	TM16COM_LCD_ON		= 0x03,		//���� LCD ��ʾ
	TM16COM_RC32K			= 0x18,		//ϵͳʱ��Դ: Ƭ�� RC ����
	TM16COM_NORMAL		= 0xE3,		//����ģʽ
	TM16COM_F1				= 0xA0,		//ʱ�������Ƶ�ʣ�1Hz
	TM16COM_F32			= 0xA5,		//ʱ�������Ƶ�ʣ�32Hz
	TM16COM_F64			= 0xA6,		//ʱ�������Ƶ�ʣ�64Hz
	TM16COM_F128			= 0xA7,		//ʱ�������Ƶ�ʣ�128Hz
}TM16_COM;


void TM16_init(void);
void TM16_wirteDAT_4N(uint8_t addr, uint8_t *pdat, uint8_t slen);
void TM16_wirteDAT_8N(uint8_t addr, uint8_t *pdat, uint8_t slen);
void TM16_display_all(uint8_t sdat);


#endif


