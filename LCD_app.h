#ifndef	_LCD_APP_H_
#define	_LCD_APP_H_

#include "M0518.h"

#define LCD_PIN_BACKLIGHT	PB11	//LCD��������

#define LCD_SHOW_GAP	300			//LCDˢ��ʱ���� 100=1S

#define LCD_SHOW_ALL	0xFF
#define LCD_SHOW_NULL	0x00



typedef struct
{
	uint16_t 	tim_cnt;		//��ʱ����
	uint8_t 	show_flag;		//��ʾ���
}sLCD_fr;

extern sLCD_fr LCD_fr;


void LCD_init(void);
void LCD_tim_poll(void);
void LCD_display_en(void);
void LCD_display_poll(void);
void LCD_backlight_onoff(uint8_t onoff_flag);



#endif


