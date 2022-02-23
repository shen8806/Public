//#include "M433_app.h"
#include "public.h"

uRF_uFR 		RF_RX_uFR;
uRF_uFR 		RF_TX_uFR;
tRF_RXBUFF_STR	RF_RXBUFF_STR;
sRF_TYPE_FR		RF_TYPE_FR;

/////////////// 433模块的帧相关的初始化 /////////////// 
void M433_fr_init(void)
{
	uint32_t u32_tmp;
	RF_TYPE_FR.rx_flag 	= 0;
	RF_TYPE_FR.tx_flag		= 0;
	RF_TYPE_FR.up_gap		= 0;
	RF_TYPE_FR.up_tim_cnt = 0;
	RF_TYPE_FR.up_sensor_flag01 = 0;
	RF_TYPE_FR.up_sensor_flag02 = 0;
	RF_TYPE_FR.rx_type		= RF_TYPE_NULL;
	RF_TYPE_FR.txrx_ing_flag	= 0;

	u32_tmp = DataFlash_byte_read( DF_UPFALG_ADDR );
	if(u32_tmp==0xFFFFFFFF)
		RF_TYPE_FR.up_flag	 = 0;
	else if(u32_tmp)
	{
		RF_TYPE_FR.up_flag	 = 1;
		u32_tmp = DataFlash_byte_read( DF_UPFALG_ADDR+4 );
		RF_TYPE_FR.up_gap = u32_tmp & 0xFFFF;
	}
	else
		RF_TYPE_FR.up_flag	 = 0;
}

/////////////// 433模块的UART缓存到接收大缓存区 /////////////// 
void M433_RXbuff_transfer(uint8_t *pdat, uint8_t slen)
{

	uint8_t i;
	
	if( pdat[0]==RF_FIR_B0 && pdat[1]==RF_FIR_B1 )	//检查包头
	{
		if(RF_RXBUFF_STR.rxBuf_num >= RX_BUFF_SIZE)
			RF_RXBUFF_STR.rxBuf_num = 0;

		if( slen>RF_FR_LEN )
			slen = RF_FR_LEN;

		RF_RXBUFF_STR.len[RF_RXBUFF_STR.rxBuf_num]	= slen;
		for(i=0; i<slen; i++)
		{
			RF_RXBUFF_STR.rxBuff[RF_RXBUFF_STR.rxBuf_num][i] = pdat[i];
		}
		RF_RXBUFF_STR.ok_flag[RF_RXBUFF_STR.rxBuf_num] = RXBUFF_OK_FLAG;
		//printf("433** len=%d,cnt=%d\r\n",slen,RF_RXBUFF_STR.rxBuf_num);
		
		
		RF_RXBUFF_STR.rxBuf_num++;
	}
}


/////////////// RF帧检查 /////////////// 
void RF_frRX_check(void)
{
	uint8_t 	i;
	uint8_t 	tmp_len;
	uint8_t 	u8_tmp;
	uint16_t	crc16;
	
	if(RF_RXBUFF_STR.rxRun_cnt >= RX_BUFF_SIZE)
		RF_RXBUFF_STR.rxRun_cnt = 0;
	
	if(	RF_RXBUFF_STR.ok_flag[RF_RXBUFF_STR.rxRun_cnt] ==  RXBUFF_OK_FLAG)		//查询是否有数据并有效
	{
		u8_tmp = RF_FR_LEN - RF_SDAT_LEN;
		tmp_len = u8_tmp + RF_RXBUFF_STR.rxBuff[RF_RXBUFF_STR.rxRun_cnt][3+RF_ADDR_LEN];
		//printf("tmp_len=%d/%d\r\n",tmp_len,RF_RXBUFF_STR.len[RF_RXBUFF_STR.rxRun_cnt]);
		if( (RF_RXBUFF_STR.len[RF_RXBUFF_STR.rxRun_cnt]>=tmp_len) && tmp_len<=RF_FR_LEN )
		{
			for(i=0; i<tmp_len; i++)
			{
				RF_RX_uFR.buff[i]	= RF_RXBUFF_STR.rxBuff[RF_RXBUFF_STR.rxRun_cnt][i];
			}
			tmp_len = u8_tmp+RF_RX_uFR.RFu_fr.length;
			RF_RX_uFR.RFu_fr.checksum[1] = RF_RXBUFF_STR.rxBuff[RF_RXBUFF_STR.rxRun_cnt][tmp_len-3];
			RF_RX_uFR.RFu_fr.checksum[0] = RF_RXBUFF_STR.rxBuff[RF_RXBUFF_STR.rxRun_cnt][tmp_len-4];
			RF_RX_uFR.RFu_fr.fr_end[1]	= RF_RXBUFF_STR.rxBuff[RF_RXBUFF_STR.rxRun_cnt][tmp_len-1];
			RF_RX_uFR.RFu_fr.fr_end[0]	= RF_RXBUFF_STR.rxBuff[RF_RXBUFF_STR.rxRun_cnt][tmp_len-2];

			RF_RXBUFF_STR.rxBuff[RF_RXBUFF_STR.rxRun_cnt][tmp_len-2] = 0;
			RF_RXBUFF_STR.rxBuff[RF_RXBUFF_STR.rxRun_cnt][tmp_len-1] = 0;

			//printf("## RF RX: ");
			//printf("C=%.2X",RF_RX_uFR.RFu_fr.com);
			/*
			for(i=0; i<tmp_len; i++)
			{
				printf("%.2X",RF_RX_uFR.buff[i]);
			}
			*/


			crc16 = Calculate_CRC16(RF_RX_uFR.buff,tmp_len-4);
			if( (crc16>>8)==RF_RX_uFR.RFu_fr.checksum[0] && (crc16&0xFF)==RF_RX_uFR.RFu_fr.checksum[1] )	//校验码检查
			{
				if( RF_RX_uFR.RFu_fr.fr_end[0] == RF_END_B0 &&	RF_RX_uFR.RFu_fr.fr_end[1] == RF_END_B1)	//检查包尾
				{
					RF_TYPE_FR.rx_flag = 1;
					printf(" OK");
				}
			}
			else
			{
				;//printf(" CRC_ERR:0x%.4X",crc16);
			}
			//printf("\r\n");
			
		}
		RF_RXBUFF_STR.ok_flag[RF_RXBUFF_STR.rxRun_cnt] =  RXBUFF_NULL_FLAG; 	//清除标记位
	}

	RF_RXBUFF_STR.rxRun_cnt++;
}

/////////////// 比对目的地址 /////////////// 
uint8_t sRF_AddrFinish_compare(uint8_t *pdat)
//*pdat:	待比对的数据
{
	uint8_t i;
	uint8_t ok_cnt,ok00_cnt;
	
	if(pdat[0] == device_id_str[0])
	{
		if(pdat[1] == device_id_str[1])
		{
			ok_cnt = 2;
			ok00_cnt = 2;
			for(i=2; i<RF_ADDR_LEN; i++)
			{
				if(pdat[i] == device_id_str[i])
					ok_cnt++;
				if(pdat[i] == 0x00)
					ok00_cnt++;
			}
			
			if(ok_cnt>=RF_ADDR_LEN)
				return RF_TYPE_ONESELF;			//自身ID
			else if(ok00_cnt>=RF_ADDR_LEN)
				return RF_TYPE_BROADCAST02;		//该类设备的广播ID
		}
	}
	else if( pdat[0] == 0x00 )
	{
		ok_cnt = 0;
		for(i=0; i<RF_ADDR_LEN; i++)
		{
			if(pdat[i] == 0x00)
				ok_cnt++;
			else
				break;
		}
		if(ok_cnt>=RF_ADDR_LEN)
			return RF_TYPE_BROADCAST01;			//所有设备的广播ID
	}
	return RF_TYPE_NULL;						//无效
}


/////////////// RF发送帧设置 /////////////// 
void RF_TX_set(uint8_t sCom)
{
	RF_TX_uFR.RFu_fr.com = sCom;
	RF_TYPE_FR.tx_flag = 1;
}


/////////////// RF发送帧 /////////////// 
void RF_TX_FR(uint8_t sLen)
//调用该函数前，需要配置 com,sdata
{
	//static uint16_t txOK_cnt = 0;
	uint8_t 	i;
	uint8_t 	u8_len;
	uint16_t	crc16_dat;
	RF_TX_uFR.RFu_fr.fr_first[0] 	= RF_FIR_B0;
	RF_TX_uFR.RFu_fr.fr_first[1] 	= RF_FIR_B1;

	for( i=0; i<RF_ADDR_LEN; i++ )
	{
		RF_TX_uFR.RFu_fr.dev_addr[i] = device_id_str[i];
	}

	//RF_TX_uFR.RFu_fr.dev_addr[RF_ADDR_LEN-1] = txOK_cnt & 0xFF;	//TEST
	//RF_TX_uFR.RFu_fr.dev_addr[RF_ADDR_LEN-2] = txOK_cnt >> 8;	//TEST
	//txOK_cnt++;
	
	RF_TX_uFR.RFu_fr.length			= sLen;

	u8_len = RF_FR_LEN - RF_SDAT_LEN + RF_TX_uFR.RFu_fr.length;
	crc16_dat = Calculate_CRC16( RF_TX_uFR.buff,u8_len-4);

	RF_TX_uFR.buff[u8_len-4] = crc16_dat>>8;
	RF_TX_uFR.buff[u8_len-3] = crc16_dat&0xFF;
	RF_TX_uFR.buff[u8_len-2] = RF_END_B0;
	RF_TX_uFR.buff[u8_len-1] = RF_END_B1;

	UART_SendBuff( M433_UART, RF_TX_uFR.buff, u8_len);

	LED0_on_set(LED_TIME_50MS);
	printf("## RF TX: ");
	printf("C=0x%.2X",RF_TX_uFR.RFu_fr.com);
	/*
	for( i=0; i<u8_len; i++ )
	{
		printf("%.2X",RF_TX_uFR.buff[i]);
	}
	*/
	printf("\r\n");
}

/////////////// RF发送应答帧 /////////////// 
void RF_TX_ACK(uint8_t rxcom,uint8_t status)
//rxcom: 处理中的指令号
//status: 应答状态
{
	RF_TX_uFR.RFu_fr.com		= RF_SET_ACK;
	RF_TX_uFR.RFu_fr.sdata[0] 	= rxcom;
	RF_TX_uFR.RFu_fr.sdata[1] 	= status;

	RF_TX_FR(2);
}

/////////////// RF传感器数据主动定时器轮询 /////////////// 
void RF_tim_poll(void)
{
	if( RF_TYPE_FR.up_flag )
	{
		if( RF_TYPE_FR.up_tim_cnt )
		{
			if( RF_TYPE_FR.up_tim_cnt==100 )	//最后1秒
				RF_TYPE_FR.up_sensor_flag01 = 1;
			RF_TYPE_FR.up_tim_cnt--;
		}
		else
		{
			RF_TYPE_FR.up_tim_cnt = RF_TYPE_FR.up_gap;
			RF_TYPE_FR.up_sensor_flag02 = 1;
		}
	}
}

/////////////// 433模块开机配置 /////////////// 
void M433_COM_set(void)
{
	uint8_t 	i, tx_len;
	uint8_t		str_dat[32];
	uint16_t	crc16;
	const uint8_t str_SETDEV_ID[]={ 0xF0, 0xF0, 0x06, 0x09,  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0x00 };

	tx_len = 4+9+2;;
	for( i=0; i<(tx_len-2); i++)
	{
		str_dat[i] = str_SETDEV_ID[i];
	}
	for( i=0; i<RF_ADDR_LEN; i++)
	{
		str_dat[i+4] = device_id_str[i];
	}
	crc16 = Calculate_CRC16( str_dat, tx_len-2 );
	str_dat[tx_len-2] = crc16>>8;
	str_dat[tx_len-1] = crc16&0xFF;

	UART_SendBuff( M433_UART, str_dat, tx_len);
	delay_ms(200);
	UART_SendBuff( M433_UART, str_dat, tx_len);
	printf("## M433 set dev_id\r\n");
}

/////////////// 433模块恢复出厂设置 /////////////// 
void M433_COM_reset_def(void)
{
	uint8_t 	i, tx_len;
	uint8_t		str_dat[10];
	uint16_t	crc16;
	const uint8_t str_RST_DEF[]={ 0xF0, 0xF0, 0x09, 0x03,  0xA7, 0x5F, 0x99 };	//F0 F0 09 03 A7 5F 99 6B C3

	tx_len = 4+3+2;;
	for( i=0; i<(tx_len-2); i++)
	{
		str_dat[i] = str_RST_DEF[i];
	}
	crc16 = Calculate_CRC16( str_dat, tx_len-2 );
	str_dat[tx_len-2] = crc16>>8;
	str_dat[tx_len-1] = crc16&0xFF;

	UART_SendBuff( M433_UART, str_dat, tx_len);
	delay_ms(200);
	UART_SendBuff( M433_UART, str_dat, tx_len);
	printf("## M433 reset default!!!\r\n");
}

////////////////////////////////////////////////////////////////////////////////////
/////////////// RF接收处理轮询 /////////////// 
void RF_RX_poll(void)
{
	uint8_t ok_flag=0;
	uint16_t  u16_tmp, u16_tmp2;
	uint32_t  u32_tmp;
	if( Fan_DAT.dev_onoff==1 )
	{
		RF_frRX_check();
		if( RF_TYPE_FR.rx_flag )
		{
			RF_TYPE_FR.rx_type = sRF_AddrFinish_compare( RF_RX_uFR.RFu_fr.sdata );
			//printf("## RX_type: %d\r\n",RF_TYPE_FR.rx_type);
			//if( RF_TYPE_FR.rx_type == RF_TYPE_ONESELF ||  RF_TYPE_FR.rx_type == RF_TYPE_BROADCAST02)
			if( RF_TYPE_FR.rx_type != RF_TYPE_NULL)
			{
				RF_TYPE_FR.txrx_ing_flag = 1;
				printf("## RF RX: ");
				printf("C=0x%.2X\r\n",RF_RX_uFR.RFu_fr.com);
				LED0_on_set(LED_TIME_50MS);
				switch( RF_RX_uFR.RFu_fr.com )
				{
					case RF_SET_SPEED_IN:
						if( RF_RX_uFR.RFu_fr.length == 11 )
						{
							RF_TYPE_FR.rx_status = FR_ACK_ERR;
							u16_tmp = RF_RX_uFR.RFu_fr.sdata[10];
							u16_tmp = (u16_tmp<<8) + RF_RX_uFR.RFu_fr.sdata[9];
							
							if( RF_RX_uFR.RFu_fr.sdata[8] == 0x11)			//按照PWM占空比控制
							{
								u16_tmp2 = (u16_tmp*FAN_SPEED_MAX)/100;		//百分比转换成限制的百分比
								
								CTRL_speed_set(u16_tmp2);
								ok_flag = 1;
								RF_TYPE_FR.rx_status = FR_ACK_OK;
							}
							else if( RF_RX_uFR.RFu_fr.sdata[8] == 0x22)		//按照风量来控制
							{
								if( u16_tmp>FAN_M3H_MAX )
									u16_tmp = FAN_M3H_MAX;
								u16_tmp2 = (u16_tmp*FAN_SPEED_MAX)/150 ; //A*FAN_SPEED_MAX/150风量
								
								CTRL_speed_set(u16_tmp2);
								ok_flag = 1;
								RF_TYPE_FR.rx_status = FR_ACK_OK;
							}
							
							if( ok_flag>0 && RF_TYPE_FR.rx_type==RF_TYPE_ONESELF)
							{
								RF_TYPE_FR.u8_dat[0] = RF_RX_uFR.RFu_fr.sdata[9];
								RF_TYPE_FR.u8_dat[1] = RF_RX_uFR.RFu_fr.sdata[10];
								RF_TX_set( RF_SET_SPEED_IN_ACK );
							}
						}
					break;

					case RF_IQU_SENSOR:
						if( RF_RX_uFR.RFu_fr.length==(1+RF_ADDR_LEN) && RF_RX_uFR.RFu_fr.sdata[RF_ADDR_LEN]==0x7E)
						{
							RF_TYPE_FR.tx_cod_cnt = 1;
							RF_TX_set( RF_IQU_SENSOR_ACK );
							RF_TX_poll();
							
							delay_ms( 100 );

							RF_TYPE_FR.tx_cod_cnt = 2;
							RF_TX_set( RF_IQU_SENSOR_ACK );
							RF_TX_poll();
						}
					break;

					case RF_IQU_SPEED:
						if( RF_RX_uFR.RFu_fr.length==(1+RF_ADDR_LEN) && RF_RX_uFR.RFu_fr.sdata[RF_ADDR_LEN]==0x7E)
						{
							RF_TX_set( RF_IQU_SPEED_ACK );
						}
					break;

					case RF_SET_SENSORUP:
						if( RF_RX_uFR.RFu_fr.length==11 )
						{
							if( RF_RX_uFR.RFu_fr.sdata[8]==0xFF )
								RF_TYPE_FR.up_flag = 1;
							else
								RF_TYPE_FR.up_flag = 0;
							
							u16_tmp = RF_RX_uFR.RFu_fr.sdata[10];
							u16_tmp = (u16_tmp<<8) + RF_RX_uFR.RFu_fr.sdata[9];
							if(u16_tmp>600)
								u16_tmp = 600;
							RF_TYPE_FR.up_gap = u16_tmp*100;

							u32_tmp = RF_TYPE_FR.up_flag;
							DataFlash_byte_write( DF_UPFALG_ADDR, u32_tmp );
							u32_tmp = RF_TYPE_FR.up_gap;
							DataFlash_byte_write( DF_UPFALG_ADDR+4, u32_tmp );
							
							if( RF_TYPE_FR.rx_type==RF_TYPE_ONESELF)
							{
								RF_TX_ACK( RF_SET_SENSORUP, 0xFF );
							}
						}
					break;
				}
			}
			RF_TX_poll();
			RF_TYPE_FR.rx_flag = 0;
		}
	}
}

/////////////// RF传感器数据主动上传 /////////////// 
void RF_sensor_up(void)
{
	//uint8_t ok_flag;
	if( RF_TYPE_FR.up_flag && Fan_DAT.dev_onoff==1)
	{
		if( RF_TYPE_FR.up_sensor_flag01 )
		{
			RF_TYPE_FR.up_sensor_flag01 = 0;
			
			RF_TYPE_FR.tx_cod_cnt = 1;
			RF_TX_set( RF_IQU_SENSOR_ACK );
			RF_TX_poll();
		}

		if( RF_TYPE_FR.up_sensor_flag02 )
		{
			RF_TYPE_FR.up_sensor_flag02 = 0;
			
			RF_TYPE_FR.tx_cod_cnt = 2;
			RF_TX_set( RF_IQU_SENSOR_ACK );
			RF_TX_poll();
		}
	}
}



/////////////// RF发送处理轮询 /////////////// 
void RF_TX_poll(void)
{
	uint16_t u16_tmp;
	if( RF_TYPE_FR.tx_flag )
	{
		RF_TYPE_FR.txrx_ing_flag = 1;
		switch( RF_TX_uFR.RFu_fr.com )
		{
			case RF_SET_SPEED_IN_ACK:
				RF_TX_uFR.RFu_fr.sdata[0] = RF_TYPE_FR.rx_status; 
				RF_TX_uFR.RFu_fr.sdata[1] = RF_TYPE_FR.u8_dat[0];
				RF_TX_uFR.RFu_fr.sdata[2] = RF_TYPE_FR.u8_dat[1];
				RF_TX_FR( 3 );
			break; 

			case RF_IQU_SENSOR_ACK:
				if( RF_TYPE_FR.tx_cod_cnt==1 )
				{
					RF_TX_uFR.RFu_fr.sdata[0] = SENSOR_TYPE_PM25;
					RF_TX_uFR.RFu_fr.sdata[1] = Sensor_DAT.PM2_5 & 0xFF;
					RF_TX_uFR.RFu_fr.sdata[2] = Sensor_DAT.PM2_5 >> 8;

					RF_TX_uFR.RFu_fr.sdata[3] = SENSOR_TYPE_CO2;
					RF_TX_uFR.RFu_fr.sdata[4] = Sensor_DAT.CO2 & 0xFF;
					RF_TX_uFR.RFu_fr.sdata[5] = Sensor_DAT.CO2 >> 8;

					RF_TX_uFR.RFu_fr.sdata[6] = SENSOR_TYPE_CH2O;
					RF_TX_uFR.RFu_fr.sdata[7] = Sensor_DAT.CH2O_ug & 0xFF;
					RF_TX_uFR.RFu_fr.sdata[8] = Sensor_DAT.CH2O_ug >> 8;

					RF_TX_uFR.RFu_fr.sdata[9] = SENSOR_TYPE_TEM;
					RF_TX_uFR.RFu_fr.sdata[10] = Sensor_DAT.temperature & 0xFF;
					RF_TX_uFR.RFu_fr.sdata[11] = Sensor_DAT.temperature >> 8;

					RF_TX_uFR.RFu_fr.sdata[12] = SENSOR_TYPE_RH;
					RF_TX_uFR.RFu_fr.sdata[13] = Sensor_DAT.humidity;
					RF_TX_uFR.RFu_fr.sdata[14] = 0;
					RF_TX_FR( 15 );
				}
				else if( RF_TYPE_FR.tx_cod_cnt==2 )
				{
					if( Fan_DAT.loop_mod==0 )
						u16_tmp = (Fan_DAT.ctrl_percen*100)/FAN_SPEED_MAX; //转换成最大为100%的形式
					else
						u16_tmp = (Fan_DAT.ctrl_percen2*100)/FAN_SPEED_MAX;
					if(u16_tmp>100)
						u16_tmp = 100;
					RF_TX_uFR.RFu_fr.sdata[0] = SENSOR_TYPE_PERCEN;
					RF_TX_uFR.RFu_fr.sdata[1] = u16_tmp & 0xFF;
					RF_TX_uFR.RFu_fr.sdata[2] = 0;

					RF_TX_uFR.RFu_fr.sdata[3] = SENSOR_TYPE_HOT;
					RF_TX_uFR.RFu_fr.sdata[4] = Fan_DAT.hot_air;
					RF_TX_uFR.RFu_fr.sdata[5] = 0;

					RF_TX_uFR.RFu_fr.sdata[6] = SENSOR_TYPE_TEM | 0x80;
					RF_TX_uFR.RFu_fr.sdata[7] = Sensor_DAT.temperature_W & 0xFF;
					RF_TX_uFR.RFu_fr.sdata[8] = Sensor_DAT.temperature_W >> 8;

					RF_TX_uFR.RFu_fr.sdata[9] = SENSOR_TYPE_RH | 0x80;
					RF_TX_uFR.RFu_fr.sdata[10] = Sensor_DAT.humidity_W;
					RF_TX_uFR.RFu_fr.sdata[11] = 0;
					
					RF_TX_FR( 12 );
				}
			break;

			case RF_IQU_SPEED_ACK:
				if( Fan_DAT.loop_mod==0 )
					u16_tmp = (Fan_DAT.ctrl_percen*100)/FAN_SPEED_MAX; //转换成最大为100%的形式
				else
					u16_tmp = (Fan_DAT.ctrl_percen2*100)/FAN_SPEED_MAX;
				if(u16_tmp>100)
					u16_tmp = 100;
				RF_TX_uFR.RFu_fr.sdata[0] = u16_tmp & 0xFF;
				RF_TX_uFR.RFu_fr.sdata[1] = 0xFF;
				RF_TX_uFR.RFu_fr.sdata[2] = 0xFF;
				RF_TX_uFR.RFu_fr.sdata[3] = 0xFF;
				RF_TX_uFR.RFu_fr.sdata[4] = 0xFF;
				RF_TX_uFR.RFu_fr.sdata[5] = 0xFF;

				RF_TX_FR( 6 );
			break;
		}
		
		RF_TYPE_FR.tx_flag = 0;
	}
}




