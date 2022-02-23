#include "public.h"

//#include "uart_ctrl.h"
//#include <stdio.h>

tUART_RXBUFF_STR  CTRL_RXBUFF_STR;
uUART_uFR  UART_uFR_RX;
uUART_uFR  UART_uFR_TX;
sCTRL_TYPE_FR  CTRL_TYPE_FR;
sSensor_DAT  Sensor_DAT;

const uint8_t auchCRCLo[];
const uint8_t auchCRCHi[];



/////////////// CTRL的帧初始化 ///////////////
void CTRL_init(void)
{
	uint8_t i;
	CTRL_RXBUFF_STR.rxBuf_num = 0;
	CTRL_RXBUFF_STR.rxRun_cnt = 0;
	for(i=0; i<UART_BUFF_SIZE; i++)
	{
		CTRL_RXBUFF_STR.len[i] 		= 0;
		CTRL_RXBUFF_STR.ok_flag[i] 	= UARTBUFF_NULL_FLAG;
	}
	CTRL_TYPE_FR.rx_flag = 0;
	CTRL_TYPE_FR.tx_flag = 0;
	CTRL_TYPE_FR.ack_status = 0;
	CTRL_TYPE_FR.rx_ack_ok 	= 0;
	CTRL_TYPE_FR.tim_cnt	= 0;

	Sensor_DAT.PM2_5 	= 0;
	Sensor_DAT.CO2		= 0;
	Sensor_DAT.CH2O_ug	= 0;
	Sensor_DAT.temperature	= 0;
	Sensor_DAT.temperature_W= 0;
	Sensor_DAT.humidity		= 0;
	Sensor_DAT.humidity_W	= 0;
	Sensor_DAT.AQI_dat		= 0;
	//Sensor_DAT.fan1_percen  = 0;
	
}

/////////////// CRC校验和计算 /////////////// 
uint16_t Calculate_CRC16( uint8_t *pdat, uint8_t slen)
//pdat: 数据
//slen:	数据域长度
{
	uint8_t 	crcHi = 0xFF;  // 高位初始化
	uint8_t 	crcLo = 0xFF;  // 低位初始化
	uint16_t 	crcIndex;
	uint16_t	crc16;
	uint8_t 	i;
	for (i = 0; i<slen; i++)
	{
		crcIndex = crcHi ^ pdat[i]; 			//查找crc表值

		crcHi = (uint8_t)(crcLo ^ auchCRCHi[crcIndex]);
		crcLo = auchCRCLo[crcIndex];
	}
	crc16 = (uint16_t)((crcHi << 8) | crcLo);	//小端模式(低字节在前) 0x1234-->0x34,0x12
	//crc16 = (uint16_t)((crcLo << 8) | crcHi);	//大端模式(高字节在前)

	return crc16;
}


/////////////// 控制接口的UART缓存到接收大缓存区 /////////////// 
void CTRL_RXbuff_transfer(uint8_t *pdat, uint8_t slen)
{
	uint8_t i;

	if( pdat[0]==UART_FIR_B0 && pdat[1]==UART_FIR_B1 )	//检查包头
	{
		if(CTRL_RXBUFF_STR.rxBuf_num >= UART_BUFF_SIZE)
			CTRL_RXBUFF_STR.rxBuf_num = 0;

		CTRL_RXBUFF_STR.len[CTRL_RXBUFF_STR.rxBuf_num]	= slen;
		for(i=0; i<slen; i++)
		{
			CTRL_RXBUFF_STR.rxBuff[CTRL_RXBUFF_STR.rxBuf_num][i] = pdat[i];
		}
		CTRL_RXBUFF_STR.ok_flag[CTRL_RXBUFF_STR.rxBuf_num] = UARTBUFF_OK_FLAG;
		//printf("** %d\r\n",CTRL_RXBUFF_STR.rxBuf_num);
		
		
		CTRL_RXBUFF_STR.rxBuf_num++;
	}
}

/////////////// CTRL的UART帧检查 /////////////// 
void CTRL_frRX_check(void)
{
	uint8_t  len1;
	//uint8_t  i;
	uint16_t crc16;
	uint8_t  run_cnt = CTRL_RXBUFF_STR.rxRun_cnt;


	if( CTRL_RXBUFF_STR.ok_flag[run_cnt] == UARTBUFF_OK_FLAG )
	{
		//printf("## UART1 RX...\r\n");
		if( CTRL_RXBUFF_STR.len[run_cnt]>=(UART_FR_LEN-UART_SDAT_LEN) && CTRL_RXBUFF_STR.len[run_cnt]<=UART_FR_LEN )
		{
			memcpy( UART_uFR_RX.buff, CTRL_RXBUFF_STR.rxBuff[run_cnt] , CTRL_RXBUFF_STR.len[run_cnt] );

			len1 = UART_uFR_RX.UART_FR.length+(UART_FR_LEN-UART_SDAT_LEN);
			CTRL_RXBUFF_STR.rxBuff[run_cnt][0] 			= 0;

			
			if( CTRL_RXBUFF_STR.len[run_cnt] >= len1 )
			{
					//UART_uFR_RX.UART_FR.checksum[1] = UART_uFR_RX.UART_FR.buff[len1-1];
					//UART_uFR_RX.UART_FR.checksum[0] = UART_uFR_RX.UART_FR.buff[len1-2];
					
					crc16 = Calculate_CRC16( UART_uFR_RX.buff, len1-2);

					/*
					printf("## RS485 RX: ");
					for(i=0; i<len1; i++)
						printf("%.2X",UART_uFR_RX.buff[i]);
					*/

					if( UART_uFR_RX.buff[len1-2]==(crc16 >> 8) && UART_uFR_RX.buff[len1-1]==(crc16 & 0xFF) )	//校验成功
					{
						CTRL_TYPE_FR.rx_flag = 1;
						//printf(" OK");
					}
					else
					{
						;
						//printf(" CRC:%.4X",crc16);
					}

					//printf("\r\n");
			}
		}

		if( CTRL_TYPE_FR.rx_flag==0 )
			;//printf("## RS485 RX ERROR!!!!\r\n");
		
		CTRL_RXBUFF_STR.ok_flag[run_cnt] = UARTBUFF_NULL_FLAG;
	}
}


/////////////// CTRL的UART发送帧 /////////////// 
void CTRL_TX_FR(uint8_t sLen)
//调用该函数前，需要配置 com,sdata
{
	uint8_t 	u8_len;
//	uint8_t 	i;
	uint16_t	crc16_dat;
	
	UART_uFR_TX.UART_FR.fr_first[0] 	= UART_FIR_B0;
	UART_uFR_TX.UART_FR.fr_first[1] 	= UART_FIR_B1;
	UART_uFR_TX.UART_FR.dev_addr 		= UART_FIR_ADDR;

	
	UART_uFR_TX.UART_FR.length			= sLen;

	u8_len = UART_FR_LEN - UART_SDAT_LEN + UART_uFR_TX.UART_FR.length;
	crc16_dat = Calculate_CRC16( UART_uFR_TX.buff, u8_len-2 );

	UART_uFR_TX.buff[u8_len-2] = crc16_dat>>8;
	UART_uFR_TX.buff[u8_len-1] = crc16_dat&0xFF;

	UART_SendBuff( CTRL_UART, UART_uFR_TX.buff, u8_len );
	LED0_on_set(LED_TIME_100MS);

	printf( "## CTRL TX:" );
	printf( "C=0x%.2X",UART_uFR_TX.UART_FR.com );
	/*
	for(i=0; i<u8_len; i++)
	{
		printf( "%.2X",UART_uFR_TX.buff[i] );
	}
	*/
	printf( "\r\n" );
}


/////////////// 风机控制-按照PWM /////////////// 
void CTRL_speed_set(uint8_t percen)
{
	uint8_t i,ok_flag=0;
	if( Fan_DAT.loop_mod==0)	//外循环，进风
	{
		//if(percen>100)
		//	Fan_DAT.ctrl_percen = 100;
		if(percen>FAN_SPEED_MAX)
			Fan_DAT.ctrl_percen = FAN_SPEED_MAX;
		else
			Fan_DAT.ctrl_percen = percen;

		Fan_DAT.fan_level = FAN_percen_to_level(Fan_DAT.ctrl_percen);
	}
	else
	{
		if(percen>FAN_SPEED_MAX)
			Fan_DAT.ctrl_percen2 = FAN_SPEED_MAX;
		else
			Fan_DAT.ctrl_percen2 = percen;

		Fan_DAT.fan_level2 = FAN_percen_to_level(Fan_DAT.ctrl_percen2);
	}

	CTRL_TX_set(UART_SPEED_SET);
	CTRL_TX_poll();
	for(i=0; i<10; i++)
	{
		delay_ms(10);
		if( (CTRL_TYPE_FR.rx_ack_com==UART_SPEED_SET_ACK) && (CTRL_TYPE_FR.rx_ack_ok==1))
		{
			ok_flag = 1;
			break;
		}
	}
	
	if(ok_flag==0)
	{
		CTRL_TX_set(UART_SPEED_SET);
		CTRL_TX_poll();
	}

	if( Fan_DAT.loop_mod==0)	//外循环，进风
	{
		printf("## Speed set:W %d%%,Level=%d\r\n",Fan_DAT.ctrl_percen,Fan_DAT.fan_level);
	}
	else
	{
		printf("## Speed set:N %d%%,Level=%d\r\n",Fan_DAT.ctrl_percen2,Fan_DAT.fan_level2);
	}
}

/////////////// CTRL的UART发送帧设置 /////////////// 
void CTRL_TX_set(uint8_t sCom)
{
	UART_uFR_TX.UART_FR.com = sCom;
	CTRL_TYPE_FR.tx_flag = 1;
}

/////////////// AQI的计算-PM2.5方面 /////////////// 
uint16_t AQI_PM25_calculation(uint16_t pm25_dat) //根据HJ633-2012标准计算
{
	const uint16_t PM25_Level_num[] = {0,35,75,115,150,250,350,500};
	const uint16_t AQI_Level_num[] 	= {0,50,100,150,200,300,400,500};
	uint16_t IAQI_lo=0, IAQI_hi=0, BP_lo=0, BP_hi=0;
	uint8_t	 i, u8_cnt=0;
	uint16_t  AQI_dat,u16_tmp,u16_tmp2,u16_tmp3;

	if(pm25_dat>=500)
	{
		AQI_dat = 500;
	}
	else if(pm25_dat==0)
	{
		AQI_dat = 0;
	}
	else
	{
		for(i=0;i<8;i++)
		{
			if( pm25_dat>=PM25_Level_num[i] && pm25_dat<PM25_Level_num[i+1] )
			{
				u8_cnt = i;
				break;
			}
		}
		BP_lo	= PM25_Level_num[u8_cnt];
		BP_hi	= PM25_Level_num[u8_cnt+1];
		IAQI_lo = AQI_Level_num[u8_cnt];
		IAQI_hi = AQI_Level_num[u8_cnt+1];
		
		u16_tmp = (IAQI_hi-IAQI_lo)/(BP_hi-BP_lo);
		u16_tmp2 = (IAQI_hi-IAQI_lo)%(BP_hi-BP_lo);
		u16_tmp3 = (u16_tmp2*(pm25_dat-BP_lo))/(BP_hi-BP_lo);
		AQI_dat = u16_tmp*(pm25_dat-BP_lo) + IAQI_lo + u16_tmp3;
	}
	return AQI_dat;
}


/////////////// CTRL定时器轮询 /////////////// 
void CTRL_tim_poll(void)
{
	if(CTRL_TYPE_FR.tim_cnt)
		CTRL_TYPE_FR.tim_cnt--;
}


/////////////// 传感器数据获取 /////////////// 
void CTRL_sensor_get(void)
{
	if(CTRL_TYPE_FR.tim_cnt==0 && Fan_DAT.dev_onoff==1)
	{
		CTRL_TYPE_FR.tim_cnt = 300;
		CTRL_TX_set(UART_SENSOR_IQU);
		CTRL_TX_poll();
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////
/////////////// CTRL发送处理轮询 /////////////// 
void CTRL_TX_poll(void)
{
	//static uint8_t  tx_cnt=0;
	if( CTRL_TYPE_FR.tx_flag )
	{
		switch( UART_uFR_TX.UART_FR.com )
		{
			case UART_SENSOR_IQU:
				CTRL_TX_FR( 0 );
			break;

			case UART_SPEED_SET:
				if( Fan_DAT.loop_mod==0)	//外循环，进风
				{
					UART_uFR_TX.UART_FR.sdata[0] = 0x00;
					UART_uFR_TX.UART_FR.sdata[2] = Fan_DAT.ctrl_percen;
				}
				else						//内循环
				{
					UART_uFR_TX.UART_FR.sdata[0] = 0x70;
					UART_uFR_TX.UART_FR.sdata[2] = Fan_DAT.ctrl_percen2;
				}
				UART_uFR_TX.UART_FR.sdata[1] = 0x11;	//按照PWM占空比控制
				UART_uFR_TX.UART_FR.sdata[3] = 0;
				CTRL_TX_FR( 4 );
			break;

			case UART_HEATING_SET:
				UART_uFR_TX.UART_FR.sdata[0] = 0x43;
				UART_uFR_TX.UART_FR.sdata[1] = Fan_DAT.hot_air;
				UART_uFR_TX.UART_FR.sdata[2] = 0x00;
				CTRL_TX_FR( 3 );
			break;
		}
		CTRL_TYPE_FR.tx_flag = 0;
	}
}

/////////////// CTRL接收处理轮询 ///////////////
void CTRL_frRX_poll(void)
{
	uint8_t i, u8_tmp;
	//uint8_t , ok_flag;
	//uint16_t u16_tmp2;
	uint16_t u16_tmp;
	
	if(CTRL_RXBUFF_STR.rxRun_cnt >= UART_BUFF_SIZE)
		CTRL_RXBUFF_STR.rxRun_cnt = 0;
	CTRL_frRX_check();
	if( CTRL_TYPE_FR.rx_flag )
	{
		CTRL_TYPE_FR.rx_flag = 0;
		LED0_on_set(LED_TIME_100MS);
		CTRL_TYPE_FR.rx_ack_com = UART_uFR_RX.UART_FR.com;
		CTRL_TYPE_FR.rx_ack_ok  = 0;
		
		switch( UART_uFR_RX.UART_FR.com )
		{
			case UART_SENSOR_IQU_ACK:
				if( (UART_uFR_RX.UART_FR.length%3)==0 )
				{
					for(i=0; i<UART_uFR_RX.UART_FR.length; i=i+3)
					{
						u16_tmp =  UART_uFR_RX.UART_FR.sdata[i+2];
						u16_tmp =  (u16_tmp<<8) | UART_uFR_RX.UART_FR.sdata[i+1];
						if( (UART_uFR_RX.UART_FR.sdata[i]&0x80)==0)	//室内
						{
							switch( UART_uFR_RX.UART_FR.sdata[i] )
							{
								case SENSOR_TYPE_PM25: 	
									Sensor_DAT.PM2_5 	= u16_tmp; 
									if(Sensor_DAT.PM2_5==0)
										Sensor_DAT.PM2_5 = 1;
									break;
									
								case SENSOR_TYPE_CO2: 	
									Sensor_DAT.CO2		= u16_tmp; 
									if(Sensor_DAT.CO2>=5000)
										Sensor_DAT.CO2 = 400;
									else if(Sensor_DAT.CO2<300)
										Sensor_DAT.CO2 = 300;
									break;
									
								case SENSOR_TYPE_CH2O: 	
									Sensor_DAT.CH2O_ug	= u16_tmp;
									if(Sensor_DAT.CH2O_ug==0)
										Sensor_DAT.CH2O_ug = 1;
									break;
									
								case SENSOR_TYPE_TEM:
									u8_tmp= (u16_tmp&0x7F)>>8;
									if(u8_tmp<=85)
										Sensor_DAT.temperature 	= u16_tmp;
									break;
									
								case SENSOR_TYPE_RH: 
									if(u16_tmp<=100)
										Sensor_DAT.humidity			= u16_tmp & 0xFF;
									break;
								case SENSOR_TYPE_PERCEN:
									if( Fan_DAT.loop_mod==0)
									{
										if( (u16_tmp & 0xFF)!= Fan_DAT.ctrl_percen )
										{
											CTRL_speed_set(Fan_DAT.ctrl_percen & 0xFF);
										}
									}
									else
									{
										if( (u16_tmp & 0xFF)!= Fan_DAT.ctrl_percen2 )
										{
											CTRL_speed_set(Fan_DAT.ctrl_percen2 & 0xFF);
										}
									} 
									break;
							}
						}
						else	//室外
						{
							u8_tmp = UART_uFR_RX.UART_FR.sdata[i]&0x7F;
							switch( u8_tmp )
							{
								case SENSOR_TYPE_TEM:
									u8_tmp= (u16_tmp&0x7F)>>8;
									if(u8_tmp<=85)
										Sensor_DAT.temperature_W 	= u16_tmp;
									break;
									
								case SENSOR_TYPE_RH:
									if(u16_tmp<=100)
										Sensor_DAT.humidity_W		= u16_tmp & 0xFF;
									break;
							}
						}
						Sensor_DAT.AQI_dat = AQI_PM25_calculation(Sensor_DAT.PM2_5);
						FAN_adj_sensor( Sensor_DAT.PM2_5, Sensor_DAT.CO2, Sensor_DAT.CH2O_ug );
					}
				}
			break;

			case UART_SPEED_SET_ACK:
				if( UART_uFR_RX.UART_FR.length==3 )
				{
					if( UART_uFR_RX.UART_FR.sdata[0]==0xFF )
					{
						CTRL_TYPE_FR.rx_ack_ok = 1;
					}
				}
			break;
			
		}


		printf( "## CTRL RX:" );
		printf( "C=0x%.2X",UART_uFR_RX.UART_FR.com );
		/*
		for(i=0;i<(UART_uFR_RX.UART_FR.length+UART_FR_LEN-UART_SDAT_LEN);i++)
		{
			printf( "%.2X",UART_uFR_RX.buff[i] );
		}
		*/
		printf( "\r\n" );

		CTRL_TX_poll();
	}
	CTRL_RXBUFF_STR.rxRun_cnt++;
}



///////////////////////////////////// CRC定义 /////////////////////////////////////
const uint8_t auchCRCLo[] = //CRC16低位表
{
    0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06,
    0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD,
    0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
    0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A,
    0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC, 0x14, 0xD4,
    0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
    0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3,
    0xF2, 0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4,
    0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
    0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29,
    0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED,
    0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
    0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60,
    0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67,
    0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
    0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68,
    0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E,
    0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
    0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71,
    0x70, 0xB0, 0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92,
    0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
    0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B,
    0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B,
    0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
    0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42,
    0x43, 0x83, 0x41, 0x81, 0x80, 0x40
};

const uint8_t auchCRCHi[] = //CRC16高位表
{
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40
};



