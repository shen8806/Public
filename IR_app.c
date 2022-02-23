//#include "IR_app.h"
//#include <stdio.h>
//#include "FAN_app.h"
#include "public.h"

sIR_datFR IR_datFR;

/////////////// 红外相关的初始化 /////////////// 
void IR_init(void)
{
	IR_MOD = 1;
	IR_datFR.rx_flag = 0;
}


/////////////// 红外遥控的UART缓存到接收大缓存区 /////////////// 
void IR_RXbuff_transfer(uint8_t *pdat, uint8_t slen)
{
	uint8_t i;

	if( pdat[0]==0x00 && pdat[1]==0xFF )	//检查用户码
	{
		if( slen>= 3 && slen<=IR_FR_LEN)
		{
			if( IR_datFR.rx_flag==0 )
			{
				for(i=0; i<slen; i++)
				{
					IR_datFR.buf_dat[i] = pdat[i];
				}
				IR_datFR.rx_flag = 1;
			}
		}
	}
}

/////////////// 红外接收运行轮询 /////////////// 
void IR_RXrun_poll(void)
{
	if(IR_datFR.rx_flag)
	{
		printf("## IR RX:0x%.2X",IR_datFR.buf_dat[2]);
		switch( IR_datFR.buf_dat[2] )
		{
			case IR_KEY_ONOFF:
				if(Fan_DAT.dev_onoff==0)
				{
					FANall_on_set();
				}
				else
				{
					FANall_off_set();
				}
				LCD_display_en();
				printf(" KEY_ONOFF");
				delay_ms(50);
			break;

			case IR_KEY_MOD:
				FAN_mod_next();
				printf(" KEY_MOD");
			break;

			case IR_KEY_SPEED:
				FAN_set_level();
				printf(" KEY_SPEED");
			break;

			case IR_KEY_TIM:
				FAN_set_OFFtim();
				printf(" KEY_TIM");
			break;
			
			case IR_KEY_HOT:
				FAN_set_HOTair();
				printf(" KEY_HOT");
			break;
		}
		IR_datFR.rx_flag = 0;
		printf("\r\n");
	}
}


