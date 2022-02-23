#include "TM1622_M0.h"
#include <stdio.h>

#define delay_us(x)		CLK_SysTickDelay(x)


void TM16_io_rst(void);
void TM16_wirte_com(uint8_t com_dat);


/////////////// 初始化 ///////////////
void TM16_init(void)
{
	TM16_io_rst();
	//TM16_wirte_com( TM16COM_F128 );
	TM16_wirte_com( TM16COM_SYS_EN );
	TM16_wirte_com( TM16COM_LCD_ON );
	//TM16_wirte_com( TM16COM_RC32K );
	//TM16_wirte_com( TM16COM_NORMAL );
}

/////////////// 按照位数写 ///////////////
void TM16_wirte_Nbit( uint8_t u8_dat, uint8_t bit_num )
//bit_num最大为8
//高字节开始写起
{
	uint8_t i;
	uint8_t bit01 = 0x80;
	
	for(i=0; i<bit_num; i++)
	{
		TM16_WR = 0;
		delay_us(3);
		if( u8_dat&bit01 )
			TM16_DATA = 1;
		else
			TM16_DATA = 0;
		bit01>>=1;
		delay_us(3);
		TM16_WR = 1;
		delay_us(6);
	}
}


/////////////// IO恢复到起始状态 ///////////////
void TM16_io_rst(void)
{
	TM16_RD = 1;
	TM16_WR = 1;
	TM16_DATA = 1;
	TM16_CS = 1;
}

/////////////// CS配置 ///////////////
void TM16_CS_set( uint8_t bit_dat )
{
	if(bit_dat)
		TM16_CS = 1;
	else
		TM16_CS = 0;
	delay_us(10);
}

/////////////// 写1位 ///////////////
void TM16_wirte_1bit( uint8_t bit_dat )
{
	TM16_WR = 0;
	delay_us(3);
	if( bit_dat )
		TM16_DATA = 1;
	else
		TM16_DATA = 0;
	delay_us(3);
	TM16_WR = 1;
	delay_us(6);
}

/////////////// 写命令 ///////////////
void TM16_wirte_com(uint8_t com_dat)
{
	uint8_t type_com = TM16_TYPE_COMMAND;
	
	TM16_io_rst();
	TM16_CS_set( 0 );
	
	type_com = type_com<<5;
	TM16_wirte_Nbit( type_com, 3 );
	TM16_wirte_Nbit( com_dat, 8 );
	TM16_wirte_1bit( 0 );

	TM16_CS_set( 1 );
}

/////////////// 写数据 ///////////////
void TM16_wirteDAT_byte(uint8_t addr, uint8_t wdat)
//addr: 0,0,A5,A4,A3,A2,A1,A0 (bit7..bit0)
//wdat: 0,0, 0, 0,D0,D1,D2,D3 (bit7..bit0)
{
	uint8_t type_com = TM16_TYPE_WRITE;
	
	TM16_io_rst();
	TM16_CS_set( 0 );
	
	type_com = type_com<<5;
	TM16_wirte_Nbit( type_com, 3 );
	
	addr = addr<<2;
	TM16_wirte_Nbit( addr, 6 );

	wdat = wdat<<4;
	TM16_wirte_Nbit( wdat, 4 );

	TM16_CS_set( 1 );
}


/////////////// 连续写数据 4bit格式 ///////////////
void TM16_wirteDAT_4N(uint8_t addr, uint8_t *pdat, uint8_t slen)
//addr: 		0,0,A5,A4,A3,A2,A1,A0 (bit7..bit0)
//pdat[x]格式:  0,0, 0, 0,D0,D1,D2,D3 (bit7..bit0)
//slen: 写4bit数据的数量
{
	uint8_t i,wdat;
	uint8_t type_com = TM16_TYPE_WRITE;
	
	TM16_io_rst();
	TM16_CS_set( 0 );
	
	type_com = type_com<<5;
	TM16_wirte_Nbit( type_com, 3 );
	
	addr = addr<<2;
	TM16_wirte_Nbit( addr, 6 );

	for(i=0; i<slen; i++)
	{
		wdat = pdat[i];
		wdat = wdat<<4;
		TM16_wirte_Nbit( wdat, 4 );
	}

	TM16_CS_set( 1 );
}

/////////////// 连续写数据 8bit格式 ///////////////
void TM16_wirteDAT_8N(uint8_t addr, uint8_t *pdat, uint8_t slen)
//addr: 		0,0,A5,A4,A3,A2,A1,A0 (bit7..bit0)
//pdat[x]格式:  D0,D1,D2,D3,D0,D1,D2,D3 (bit7..bit0)(COM4..7,COM0..3))
//slen: 写8bit数据的数量
{
	uint8_t i,wdat;
	uint8_t type_com = TM16_TYPE_WRITE;
	
	TM16_io_rst();
	TM16_CS_set( 0 );
	
	type_com = type_com<<5;
	TM16_wirte_Nbit( type_com, 3 );
	
	addr = addr<<2;
	TM16_wirte_Nbit( addr, 6 );

	for(i=0; i<slen; i++)
	{
		wdat = pdat[i];
		
		TM16_wirte_Nbit( wdat<<4, 4 );
		TM16_wirte_Nbit( wdat&0xF0, 4 );
	}

	TM16_CS_set( 1 );
}



/////////////// 所有段码按照一个方式显示 ///////////////
void TM16_display_all(uint8_t sdat)
{
	uint8_t i;
	uint8_t type_com = TM16_TYPE_WRITE;
	
	TM16_io_rst();
	TM16_CS_set( 0 );
	
	type_com = type_com<<5;
	TM16_wirte_Nbit( type_com, 3 );
	
	TM16_wirte_Nbit( 0x00, 6 );

	for(i=0; i<64; i++)
	{
		TM16_wirte_Nbit( sdat, 4 );
	}

	TM16_CS_set( 1 );
}




