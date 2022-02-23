//#include <stdio.h>
//#include "LCD_app.h"
//#include "TM1622_M0.h"
#include "public.h"

sLCD_fr LCD_fr;

uint8_t lcd_seg32_buff[32] = {0};

void LCD_backlight_onoff(uint8_t onoff_flag);


const uint8_t LCD_8SEG_table1[11] ={ 
			// xBGC AFED  SEG0-11,13-16(PIN9-20,22-25)
			//D0123 0123
	0x5F,	// 0101 1111, // 0
	0x50,	// 0101 0000, // 1
	0x6B,	// 0110 1011, // 2
	0x79,	// 0111 1001, // 3
	0x74,	// 0111 0100, // 4
	0x3D,	// 0011 1101, // 5
	0x3F,	// 0011 1111, // 6
	0x58,	// 0101 1000, // 7
	0x7F,	// 0111 1111, // 8
	0x7D,	// 0111 1101  // 9
	0x00	// 0000 0000  // NULL
};

const uint8_t LCD_8SEG_table2[11] ={
			// xCGB DEFA  SEG17-29(PIN26-38)
	0x5F,	// 0101 1111, // 0
	0x50,	// 0101 0000, // 1
	0x3D,	// 0011 1101, // 2
	0x79,	// 0111 1001, // 3
	0x72,	// 0111 0010, // 4
	0x6B,	// 0110 1011, // 5
	0x6F,	// 0110 1111, // 6
	0x51,	// 0101 0001, // 7
	0x7F,	// 0111 1111, // 8
	0x7B,	// 0111 1011  // 9
	0x00	// 0000 0000  // NULL

};


/////////////// LCD初始化 ///////////////
void LCD_init(void)
{
	LCD_fr.tim_cnt = 0;
	LCD_fr.show_flag = 0;
	LCD_backlight_onoff( 0 );
	TM16_display_all( LCD_SHOW_NULL );
	//TM16_display_all( LCD_SHOW_ALL);
}

/////////////// LCD背光开启/关闭 ///////////////
void LCD_backlight_onoff(uint8_t onoff_flag)
{
	if(onoff_flag)
		LCD_PIN_BACKLIGHT = 0;	//开启
	else
		LCD_PIN_BACKLIGHT = 1;	//关闭
}


/////////////// 显示数据更新 ///////////////
void LCD_display_change(void)
{
//bit7-COM5, bit6-COM6, bit5-COM7, bit4-COM8,  bit3-COM1, bit2-COM2, bit1-COM3, bit0-COM4 
	uint8_t 	u8_tmp1,u8_tmp2, u8_dat;
	uint8_t 	temp_dat;
	uint16_t 	u16_tmp;

	/*
	// TEST //
	static uint16_t slcd_cnt = 0;
	Sensor_DAT.PM2_5 = slcd_cnt;
	Sensor_DAT.temperature_W = slcd_cnt<<8;
	slcd_cnt++;
	if(slcd_cnt>100)
		slcd_cnt = 0;
	//////
	*/

	//printf("## Sensor:PM2.5=%d,CO2=%d,CH2O=%d,N:%d℃ %d%%,W:%d℃ %d%%,AQI=%d\r\n",Sensor_DAT.PM2_5, Sensor_DAT.CO2, Sensor_DAT.CH2O_ug, 
	//	(Sensor_DAT.temperature>>8)&0x7F, Sensor_DAT.humidity, (Sensor_DAT.temperature_W>>8)&0x7F, Sensor_DAT.humidity_W, Sensor_DAT.AQI_dat );

	//- - PIN9/10 ( SEG0/1 ) - -//
	u8_tmp1 = LCD_8SEG_table1[Sensor_DAT.CO2/1000%10];						//bit7..4: 13AFED
	u8_tmp1 = u8_tmp1<<4;
	u8_tmp1 = u8_tmp1 | (LCD_8SEG_table1[Sensor_DAT.PM2_5/1000%10]&0x0F);	//bit3..0: 9AFED
	lcd_seg32_buff[0] =  u8_tmp1;

	u8_tmp2 = (LCD_8SEG_table1[Sensor_DAT.CO2/1000%10]&0xF0)<<1;			//bit7..4: 13BGC+S6
	if(Fan_DAT.hot_air)
		u8_tmp2 = u8_tmp2 | 0x10;
	u8_tmp2 = u8_tmp2 | (LCD_8SEG_table1[Sensor_DAT.PM2_5/1000%10]>>4);		//bit3..0: 9xBGC
	lcd_seg32_buff[1] = u8_tmp2;

	//- - PIN11/12 ( SEG2/3 ) - -//
	u8_tmp1 = LCD_8SEG_table1[Sensor_DAT.CO2/100%10];						//bit7..4: 14AFED
	u8_tmp1 = u8_tmp1<<4;
	u8_tmp1 = u8_tmp1 | (LCD_8SEG_table1[Sensor_DAT.PM2_5/100%10]&0x0F);	//bit3..0: 10AFED
	lcd_seg32_buff[2] =  u8_tmp1;

	u8_tmp2 = (LCD_8SEG_table1[Sensor_DAT.CO2/100%10]&0xF0)<<1;				//bit7..4: 14BGC+S10
	if(Fan_DAT.hot_air)
		u8_tmp2 = u8_tmp2 | 0x10;
	u8_tmp2 = u8_tmp2 | ((LCD_8SEG_table1[Sensor_DAT.PM2_5/100%10]>>4)|0x08);//bit3..0: S5+10BGC
	lcd_seg32_buff[3] = u8_tmp2;

	//- - PIN13/14 ( SEG4/5 ) - -//
	u8_tmp1 = LCD_8SEG_table1[Sensor_DAT.CO2/10%10];						//bit7..4: 15AFED
	u8_tmp1 = u8_tmp1<<4;
	u8_tmp1 = u8_tmp1 | (LCD_8SEG_table1[Sensor_DAT.PM2_5/10%10]&0x0F);		//bit3..0: 11AFED
	lcd_seg32_buff[4] =  u8_tmp1;

	u8_tmp2 = (LCD_8SEG_table1[Sensor_DAT.CO2/10%10]&0xF0)<<1;				//bit7..4: 15BGC+S11
	if(Fan_DAT.hot_air>=2)
		u8_tmp2 = u8_tmp2 | 0x10;
	u8_tmp2 = u8_tmp2 | ((LCD_8SEG_table1[Sensor_DAT.PM2_5/10%10]>>4)|0x08);//bit3..0: S8+11BGC
	lcd_seg32_buff[5] = u8_tmp2;

	//- - PIN15/16 ( SEG6/7 ) - -//
	u8_tmp1 = LCD_8SEG_table1[Sensor_DAT.CO2%10];							//bit7..4: 16AFED
	u8_tmp1 = u8_tmp1<<4;
	u8_tmp1 = u8_tmp1 | (LCD_8SEG_table1[Sensor_DAT.PM2_5%10]&0x0F);		//bit3..0: 12AFED
	lcd_seg32_buff[6] =  u8_tmp1;

	u8_tmp2 = (LCD_8SEG_table1[Sensor_DAT.CO2%10]&0xF0)<<1;					//bit7..4: 16BGC+S12
	if(Fan_DAT.hot_air>=3)
		u8_tmp2 = u8_tmp2 | 0x10;
	u8_tmp2 = u8_tmp2 | ((LCD_8SEG_table1[Sensor_DAT.PM2_5%10]>>4)|0x08);	//bit3..0: S9+12BGC
	lcd_seg32_buff[7] = u8_tmp2;

	//- - PIN17/18 ( SEG8/9 ) - -//
	u8_tmp1 = LCD_8SEG_table1[Sensor_DAT.humidity/10%10];					//bit7..4: 19AFED
	u8_tmp1 = u8_tmp1<<4;
	temp_dat = (Sensor_DAT.temperature>>8)&0x7F;
	if((Sensor_DAT.temperature&0x00FF)>=50)		//温度 四舍五入
		temp_dat = temp_dat+1;
	u8_tmp1 = u8_tmp1 | (LCD_8SEG_table1[temp_dat/10%10]&0x0F);				//bit3..0: 17AFED
	lcd_seg32_buff[8] =  u8_tmp1;

	u8_tmp2 = LCD_8SEG_table1[Sensor_DAT.humidity/10%10]&0xF0;				//bit7..4: 19xBGC
	u8_tmp2 = u8_tmp2 | (LCD_8SEG_table1[temp_dat/10%10]>>4);				//bit3..0: S19+17BGC
	if(Sensor_DAT.temperature&0x8000)
		u8_tmp2 = u8_tmp2 | 0x08;
	lcd_seg32_buff[9] = u8_tmp2;

	//- - PIN19/20 ( SEG10/11 ) - -//
	u8_tmp1 = LCD_8SEG_table1[Sensor_DAT.humidity%10];						//bit7..4: 20AFED
	u8_tmp1 = u8_tmp1<<4;
	temp_dat = (Sensor_DAT.temperature>>8)&0x7F;
	if((Sensor_DAT.temperature&0x00FF)>=50)		//温度 四舍五入
		temp_dat = temp_dat+1;
	u8_tmp1 = u8_tmp1 | (LCD_8SEG_table1[temp_dat%10]&0x0F);				//bit3..0: 18AFED
	lcd_seg32_buff[10] =  u8_tmp1;

	u8_tmp2 = LCD_8SEG_table1[Sensor_DAT.humidity%10]&0xF0 | 0x80;			//bit7..4: S21+20BGC
	u8_tmp2 = u8_tmp2 | ((LCD_8SEG_table1[temp_dat%10]|0x80)>>4);			//bit3..0: S20+18BGC
	lcd_seg32_buff[11] = u8_tmp2;

	//- - PIN21 ( SEG12 ) - -//
	//Fan_DAT.off_tim_cnt
	u8_tmp1 = 0xCD;															//bit7..4: S24,S25,S32,x.   bit3..0:S17,S18,S22,S23
	if(Fan_DAT.off_tim_cnt)
		u8_tmp1 = u8_tmp1 | 0x20;											//bit5: S32
	if(Fan_DAT.loop_mod)
		u8_tmp1 = u8_tmp1 | 0x02;											//bit1: S22
	lcd_seg32_buff[12] = u8_tmp1;

	//- - PIN22/23 ( SEG13/14 ) - -//
	if(Fan_DAT.off_tim_cnt>=360000)
		u16_tmp = Fan_DAT.off_tim_cnt/3600/100;
	else	//小于1小时
		u16_tmp = Fan_DAT.off_tim_cnt/60/100;
	if(u16_tmp>99)
		u16_tmp = 99;
	if(Fan_DAT.off_tim_cnt)
		u8_tmp1 = LCD_8SEG_table1[u16_tmp/10];								//bit7..4: 27AFED
	else
		u8_tmp1 = LCD_8SEG_table1[10];	
	u8_tmp1 = u8_tmp1<<4;
	u8_tmp1 = u8_tmp1 | (LCD_8SEG_table1[Sensor_DAT.humidity_W/10%10]&0x0F);//bit3..0: 25AFED
	lcd_seg32_buff[13] =  u8_tmp1;
	
	if(Fan_DAT.off_tim_cnt)
		u8_tmp2 = LCD_8SEG_table1[u16_tmp/10]&0xF0;							//bit7..4: 27xBGC
	else
		u8_tmp2 = LCD_8SEG_table1[10]&0xF0;		
	u8_tmp2 = u8_tmp2 | ((LCD_8SEG_table1[Sensor_DAT.humidity_W/10%10])>>4);//bit3..0: 25xBGC
	lcd_seg32_buff[14] = u8_tmp2;

	//- - PIN24/25 ( SEG15/16 ) - -//
	if(Fan_DAT.off_tim_cnt>=360000)
		u16_tmp = Fan_DAT.off_tim_cnt/3600/100;
	else	//小于1小时
		u16_tmp = Fan_DAT.off_tim_cnt/60/100;
	if(u16_tmp>99)
		u16_tmp = 99;
	if(Fan_DAT.off_tim_cnt)
		u8_tmp1 = LCD_8SEG_table1[u16_tmp%10];								//bit7..4: 28AFED
	else
		u8_tmp1 = LCD_8SEG_table1[10];	
	u8_tmp1 = u8_tmp1<<4;
	u8_tmp1 = u8_tmp1 | (LCD_8SEG_table1[Sensor_DAT.humidity_W%10]&0x0F);	//bit3..0: 26AFED
	lcd_seg32_buff[15] =  u8_tmp1;

	if(Fan_DAT.off_tim_cnt)
		u8_tmp2 = LCD_8SEG_table1[u16_tmp%10]&0xF0;							//bit7..4: S33+28BGC
	else
		u8_tmp2 = LCD_8SEG_table1[10]&0xF0;
	if(Fan_DAT.off_tim_cnt>=360000)
		u8_tmp2 = u8_tmp2 | 0x80;
	u8_tmp2 = u8_tmp2 | ((LCD_8SEG_table1[Sensor_DAT.humidity_W%10]|0x80)>>4);//bit3..0: S31+26BGC
	lcd_seg32_buff[16] = u8_tmp2;

	
	//- - PIN26/27 ( SEG17/18 ) - -//
	u8_tmp1 = LCD_8SEG_table2[10]&0xF0;										//bit7..4: S27+22CGB
	temp_dat = (Sensor_DAT.temperature_W>>8)&0x7F;
	if((Sensor_DAT.temperature_W&0x00FF)>=50)	//温度 四舍五入
		temp_dat = temp_dat+1;
	if( RF_TYPE_FR.txrx_ing_flag )
	{
		u8_tmp1 = u8_tmp1 | 0x80;
		RF_TYPE_FR.txrx_ing_flag = 0;
	}
	u8_tmp1 = u8_tmp1 | (LCD_8SEG_table2[temp_dat%10]>>4) | 0x08;			//bit3..0: S30+24CGB
	lcd_seg32_buff[17] =  u8_tmp1;

	u8_tmp2 = LCD_8SEG_table2[10]&0x0F; 									//bit7..4: 22DEFA
	u8_tmp2 = u8_tmp2<<4;
	u8_tmp2 = u8_tmp2 | (LCD_8SEG_table2[temp_dat%10]&0x0F);				//bit3..0: 24DEFA
	lcd_seg32_buff[18] = u8_tmp2;

	//- - PIN28/29 ( SEG19/20 ) - -//
	u8_tmp1 = (LCD_8SEG_table2[10]&0xF0) | 0x80;							//bit7..4: S28+21CGB
	temp_dat = (Sensor_DAT.temperature_W>>8)&0x7F;
	if((Sensor_DAT.temperature_W&0x00FF)>=50)	//温度 四舍五入
		temp_dat = temp_dat+1;
	u8_tmp1 = u8_tmp1 | (LCD_8SEG_table2[temp_dat/10%10]>>4);				//bit3..0: S29+23CGB
	if(Sensor_DAT.temperature_W&0x8000)
		u8_tmp1 = u8_tmp1 | 0x08;
	lcd_seg32_buff[19] =  u8_tmp1;

	u8_tmp2 = LCD_8SEG_table2[10]&0x0F; 									//bit7..4: 21DEFA
	u8_tmp2 = u8_tmp2<<4;
	u8_tmp2 = u8_tmp2 | (LCD_8SEG_table2[temp_dat/10%10]&0x0F);				//bit3..0: 23DEFA
	lcd_seg32_buff[20] = u8_tmp2;

	//- - PIN30 ( SEG21 ) - -//
	switch( Fan_DAT.dev_mod )
	{
		case FANMOD_MANUAL: 	u8_tmp1 = 0x01; break;		//bit0: S13
		case FANMOD_AUTO: 		u8_tmp1 = 0x80; break;		//bit7: S14
		case FANMOD_SLEEP: 		u8_tmp1 = 0x40; break;		//bit6: S15
		case FANMOD_STRONG: 	u8_tmp1 = 0x20; break;		//bit5: S16
		default: u8_tmp1 = 0x00;
	} 														
	u8_tmp1 = u8_tmp1 | 0x10;												//bit4: S26
	lcd_seg32_buff[21] = u8_tmp1;											//bit7..4: S14,S15,S16,S26	bit3..0:x,x,x,S13

	//- - PIN31/32 ( SEG22/23 ) - -//
	u8_tmp1 = LCD_8SEG_table2[Sensor_DAT.AQI_dat%10]&0xF0;					//bit7..4: 4xCGB
	u8_tmp1 = u8_tmp1 | (LCD_8SEG_table2[Sensor_DAT.CH2O_ug%10]>>4) | 0x08;	//bit3..0: S7+8CGB
	lcd_seg32_buff[22] =  u8_tmp1;
	
	u8_tmp2 = LCD_8SEG_table2[Sensor_DAT.AQI_dat%10]&0x0F; 					//bit7..4: 4DEFA
	u8_tmp2 = u8_tmp2<<4;
	u8_tmp2 = u8_tmp2 | (LCD_8SEG_table2[Sensor_DAT.CH2O_ug%10]&0x0F); 		//bit3..0: 8DEFA
	lcd_seg32_buff[23] = u8_tmp2;

	//- - PIN33/34 ( SEG24/25 ) - -//
	u8_tmp1 = LCD_8SEG_table2[Sensor_DAT.AQI_dat/10%10]&0xF0;				//bit7..4: 3xCGB
	u8_tmp1 = u8_tmp1 | (LCD_8SEG_table2[Sensor_DAT.CH2O_ug/10%10]>>4); 	//bit3..0: 7xCGB
	lcd_seg32_buff[24] =  u8_tmp1;

	u8_tmp2 = LCD_8SEG_table2[Sensor_DAT.AQI_dat/10%10]&0x0F;				//bit7..4: 3DEFA
	u8_tmp2 = u8_tmp2<<4;
	u8_tmp2 = u8_tmp2 | (LCD_8SEG_table2[Sensor_DAT.CH2O_ug/10%10]&0x0F);	//bit3..0: 7DEFA
	lcd_seg32_buff[25] = u8_tmp2;

	//- - PIN35/36 ( SEG26/27 ) - -//
	u8_tmp1 = LCD_8SEG_table2[Sensor_DAT.AQI_dat/100%10]&0xF0;				//bit7..4: 2xCGB
	u8_tmp1 = u8_tmp1 | (LCD_8SEG_table2[Sensor_DAT.CH2O_ug/100%10]>>4); 	//bit3..0: 6xCGB
	lcd_seg32_buff[26] =  u8_tmp1;

	u8_tmp2 = LCD_8SEG_table2[Sensor_DAT.AQI_dat/100%10]&0x0F;				//bit7..4: 2DEFA
	u8_tmp2 = u8_tmp2<<4;
	u8_tmp2 = u8_tmp2 | (LCD_8SEG_table2[Sensor_DAT.CH2O_ug/100%10]&0x0F);	//bit3..0: 6DEFA
	lcd_seg32_buff[27] = u8_tmp2;

	//- - PIN37/38 ( SEG28/29 ) - -//
	u8_tmp1 = LCD_8SEG_table2[Fan_DAT.fan_level%10]&0xF0;					//bit7..4: 1xCGB
	u8_tmp1 = u8_tmp1 | (LCD_8SEG_table2[Sensor_DAT.CH2O_ug/1000%10]>>4) | 0x08; //bit3..0: P1+5CGB
	lcd_seg32_buff[28] =  u8_tmp1;

	u8_tmp2 = LCD_8SEG_table2[Fan_DAT.fan_level%10]&0x0F;					//bit7..4: 1DEFA
	u8_tmp2 = u8_tmp2<<4;
	u8_tmp2 = u8_tmp2 | (LCD_8SEG_table2[Sensor_DAT.CH2O_ug/1000%10]&0x0F);	//bit3..0: 5DEFA
	lcd_seg32_buff[29] = u8_tmp2;

	//- - PIN39 ( SEG30 ) - -//
	if( Fan_DAT.loop_mod==0)	//外循环，进风
		u8_dat = Fan_DAT.fan_level;
	else
		u8_dat = Fan_DAT.fan_level2;
	switch( u8_dat )
	{
			//bit7..4: L6,L5,L4,L3
		case 0:	u8_tmp1 = 0x00; break;
		case 1:	u8_tmp1 = 0xC0; break;	//L6,L5
		case 2: u8_tmp1 = 0xC0; break;	//L6,L5,L8,L7
		case 3:	u8_tmp1 = 0xC0; break;	//L6,L5,L8,L7,L2,L1
		case 4:	u8_tmp1 = 0xF0; break;	//L6,L5,L8,L7,L2,L1,L4,L3
		default: u8_tmp1 = 0x00;
	}														
	u8_tmp1 = u8_tmp1 | 0x0F;												//bit3..0:S4,S3,S2,S1 
	lcd_seg32_buff[30] = u8_tmp1;											//bit7..4: L6,L5,L4,L3	bit3..0:S4,S3,S2,S1

	//- - PIN40 ( SEG31 ) - -//
	switch( u8_dat )
	{
			//bit7..4: L7,L8,L1,L2
		case 0:	u8_tmp1 = 0x00; break;
		case 1:	u8_tmp1 = 0x00; break;	//L6,L5
		case 2: u8_tmp1 = 0xC0; break;	//L6,L5,L8,L7
		case 3:	u8_tmp1 = 0xF0; break;	//L6,L5,L8,L7,L2,L1
		case 4:	u8_tmp1 = 0xF0; break;	//L6,L5,L8,L7,L2,L1,L4,L3
		default: u8_tmp1 = 0x00;
	}														
	u8_tmp1 = u8_tmp1 | 0x08;												//L9
	lcd_seg32_buff[31] = u8_tmp1;											//bit7..4: L7,L8,L1,L2	bit3..0:L9,x,x,x


	TM16_wirteDAT_8N( 0x00, lcd_seg32_buff, 32 );
	if( Fan_DAT.dev_mod == FANMOD_SLEEP )
	{
		if( Fan_DAT.bw_flag == 1 )		//白天
			LCD_backlight_onoff(1);
		else
			LCD_backlight_onoff(0);
	}
	else
		LCD_backlight_onoff(1);
}

/////////////// LCD计时器轮询 10ms间隔 ///////////////
void LCD_tim_poll(void)
{
	if( LCD_fr.tim_cnt )
		LCD_fr.tim_cnt--;
	else
	{
		LCD_fr.show_flag = 1;
		LCD_fr.tim_cnt = LCD_SHOW_GAP;
	}
}

/////////////// 显示使能 ///////////////
void LCD_display_en(void)
{
	LCD_fr.show_flag = 1;
	LCD_fr.tim_cnt = LCD_SHOW_GAP;
}

/////////////// 显示执行轮询 ///////////////
void LCD_display_poll(void)
{
	if( LCD_fr.show_flag )
	{
		LCD_fr.show_flag = 0;
		if( Fan_DAT.dev_onoff )
			LCD_display_change();
		else
		{
			LCD_backlight_onoff( 0 );
			TM16_display_all( LCD_SHOW_NULL );
			//TM16_display_all( LCD_SHOW_ALL );
		}
		
		//printf("## LCD show all!\r\n");
	}
}



