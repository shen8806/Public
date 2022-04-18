#include "DataFlash.h"
#include <stdio.h>

uint32_t 		M0_uid[3] 			= {0};
const uint32_t 	str_ssd_num[2]		= {0x4B123456,0x31123456};
uint8_t 		device_id_str[RF_ADDR_LEN]	= {DEVICE_ID_SDAT};

/////////////// 读数据 /////////////// 
uint32_t u32FMC_Read(uint16_t addr)
{
	return FMC_Read( DF_REF_ADDR + addr );
}

/////////////// 写数据 /////////////// 
void u32FMC_Write(uint16_t addr, uint32_t u32_sdat)
{
	FMC_Write( DF_REF_ADDR + addr, u32_sdat );
}

/////////////// 擦除数据 /////////////// 
void u32FMC_Erase(uint16_t addr)
{
	FMC_Erase( DF_REF_ADDR + addr );
}


/////////////// 读取MCU的唯一ID号 /////////////// 
void read_uid(void)
{
	uint32_t pllclk;
	SYS_UnlockReg();
	FMC_ENABLE_ISP();
	
	M0_uid[0] = FMC_ReadUID(0);
	M0_uid[1] = FMC_ReadUID(1);
	M0_uid[2] = FMC_ReadUID(2);
	
	FMC_DISABLE_ISP();
	SYS_LockReg();

	printf("#### MCU UID: %.8X %.8X %.8X\n", M0_uid[0], M0_uid[1], M0_uid[2]); 
	
	pllclk = SystemCoreClock;
	//pllclk = PllClock;
	printf("#### MCU CLK: %d.%dMHz\n",pllclk/1000000,pllclk%1000000);
}

/////////////// 获取设备ID /////////////// 
void device_id_get(void)
{
	uint32_t 	u32_tmp[2];
	//uint8_t		u8_tmp;
	uint8_t i = 0;

	DataFlash_str_read(SSD_NUM_FlashADDR0,u32_tmp,2);
	if(u32_tmp[0] !=str_ssd_num[0] || u32_tmp[1] !=str_ssd_num[1])
	{
		//while(1) ;
	}
	printf("#### SSD NUM: %.8X%.8X\r\n",u32_tmp[0],u32_tmp[1]);

	u32_tmp[0] = DataFlash_byte_read(DEVICE_ID_ADDR0);

	printf("#### Device ID: ");
	if((u32_tmp[0]>>24)!=device_id_str[0] || ((u32_tmp[0]>>16)&0xFF)!=device_id_str[1])		//未配置ID前
	{
		u32_tmp[0] = M0_uid[0]^M0_uid[1]^M0_uid[2];	//异或
		
		device_id_str[2] = (uint8_t)( ((u32_tmp[0]>>24)&0xFF) + ((u32_tmp[0]>>16)&0xFF) + ((u32_tmp[0]>>8)&0xFF) + (u32_tmp[0]&0xFF));
		device_id_str[3] = M0_uid[1]&0xFF;
		device_id_str[4] = (M0_uid[2]>>24)&0xFF;
		device_id_str[5] = (M0_uid[2]>>16)&0xFF;
		device_id_str[6] = (M0_uid[2]>>8)&0xFF;
		device_id_str[7] = M0_uid[2]&0xFF;
		
		u32_tmp[0] = (device_id_str[0]<<24) + (device_id_str[1]<<16) + (device_id_str[2]<<8) + device_id_str[3];
		u32_tmp[1] = (device_id_str[4]<<24) + (device_id_str[5]<<16) + (device_id_str[6]<<8) + device_id_str[7];;

		DataFlash_str_write(DEVICE_ID_ADDR0,u32_tmp,2);
		printf("DEF ");
	}

	DataFlash_str_read(DEVICE_ID_ADDR0,u32_tmp,2);
	device_id_str[0] = (uint8_t)(( u32_tmp[0] >>24) & 0xFF);
	device_id_str[1] = (uint8_t)((u32_tmp[0]>>16) & 0xFF);
	device_id_str[2] = (uint8_t)((u32_tmp[0]>>8) & 0xFF);
	device_id_str[3] = (uint8_t)( u32_tmp[0] & 0xFF);
	device_id_str[4] = (uint8_t)((u32_tmp[1]>>24) & 0xFF);
	device_id_str[5] = (uint8_t)((u32_tmp[1]>>16) & 0xFF);
	device_id_str[6] = (uint8_t)((u32_tmp[1]>>8) & 0xFF);
	device_id_str[7] = (uint8_t)( u32_tmp[1] & 0xFF);

	for(i=0; i<RF_ADDR_LEN; i++)
		printf("%.2X",device_id_str[i]);
	printf("\r\n");
}

/////////////// 写1个4Byted的数据 /////////////// 
uint8_t DataFlash_byte_write(uint16_t addr,uint32_t sbyte)
{
	uint16_t i;
	//uint8_t  tmp_dat;
	uint32_t  u32tmp_dat;
	uint8_t  ok_flag = 0;
	uint16_t start_addr0 = (addr/DF_PAGE_LEN)*DF_PAGE_LEN;

	addr = (addr/4)*4;

	SYS_UnlockReg();
	FMC_ENABLE_ISP();
	
	u32FMC_Erase(DF_TMP_ADDR);
	for(i=0; i<DF_PAGE_LEN; i=i+4)
	{
		u32tmp_dat = u32FMC_Read( start_addr0 + i );
		u32FMC_Write( DF_TMP_ADDR + i, u32tmp_dat );
	}
	
	u32FMC_Erase(start_addr0);
	for(i=0; i<DF_PAGE_LEN; i=i+4)
	{
		if( (start_addr0+i)==addr)
			u32FMC_Write( addr, sbyte );
		else
		{
			u32tmp_dat = u32FMC_Read( DF_TMP_ADDR + i );
			u32FMC_Write( start_addr0 + i, u32tmp_dat );
		}
	}
	u32tmp_dat = u32FMC_Read( addr );
	if( u32tmp_dat == sbyte)
		ok_flag = 1;

	FMC_DISABLE_ISP();
	SYS_LockReg();
	
	return ok_flag;
}

/////////////// 读1个4Byted的数据 /////////////// 
uint32_t DataFlash_byte_read(uint16_t addr)
{
	uint32_t sbyte = 0;

	addr = (addr/4)*4;
	
	SYS_UnlockReg();
	FMC_ENABLE_ISP();

	sbyte = u32FMC_Read( addr );
	
	FMC_DISABLE_ISP();
	SYS_LockReg();

	return sbyte;
}

/////////////// 写N个4Byted的数据 /////////////// 
uint8_t DataFlash_str_write(uint16_t addr0, uint32_t *pbyte, uint16_t slen)
//addr0: 起始地址
//pbyte: 32位的数据串
//slen:  数据长度(slen个4Byte的数据)
{
	uint16_t i;
	uint32_t  tmp_dat;
	uint8_t  ok_flag = 0;
	uint16_t start_addr0 = (addr0/DF_PAGE_LEN)*DF_PAGE_LEN;
	uint16_t end_addr;

	addr0 = (addr0/4)*4; 

	if( (addr0-start_addr0+(slen<<2))>DF_PAGE_LEN)
	{
		end_addr = start_addr0+DF_PAGE_LEN-1;
		return 0;
	}
	else
		end_addr = addr0 + ((slen-1)<<2);

	SYS_UnlockReg();
	FMC_ENABLE_ISP();

	u32FMC_Erase(DF_TMP_ADDR);
	for(i=0; i<DF_PAGE_LEN; i=i+4)
	{
		tmp_dat = u32FMC_Read( start_addr0 + i );
		u32FMC_Write( DF_TMP_ADDR+i, tmp_dat );
	}
	
	u32FMC_Erase(start_addr0);
	for(i=addr0; i<=end_addr; i=i+4)
	{
		u32FMC_Write( i, *pbyte );
		pbyte++;
	}
	
	for(i=0; i<DF_PAGE_LEN; i=i+4)
	{
		if( i<(addr0-start_addr0) || i>(end_addr-start_addr0))
		{
			tmp_dat = u32FMC_Read( DF_TMP_ADDR + i );
			u32FMC_Write( start_addr0 + i, tmp_dat );
		}
	}
	ok_flag = 1;

	FMC_DISABLE_ISP();
	SYS_LockReg();
	
	return ok_flag;
}

/////////////// 读N个4Byted的数据 /////////////// 
void DataFlash_str_read(uint16_t addr0, uint32_t *pbyte, uint16_t slen)
{
	uint16_t i;
	//uint8_t  tmp_dat;
	//uint8_t  ok_flag = 0;
	
	addr0 = (addr0/4)*4;
	
	SYS_UnlockReg();
	FMC_ENABLE_ISP();

	for(i=0; i<slen; i++)
	{
		*pbyte = u32FMC_Read( addr0 + (i<<2) );
		pbyte++;
	}

	FMC_DISABLE_ISP();
	SYS_LockReg();
}

/////////////// DataFlash读写测试 /////////////// 
uint8_t DataFlash_test(void)
{
	uint16_t  tmp_addr = 0xC00;
	uint16_t  i;
	uint32_t  tmp_strdat[15] = {0};
	uint32_t  tmp_strdat2[15] = {0};
	uint32_t  u32_dat;
	static uint16_t cnt_addr = 0;

	DataFlash_byte_write(tmp_addr+cnt_addr, 0xAA);
	DataFlash_byte_write(tmp_addr+cnt_addr+4, 0xBB);
	DataFlash_byte_write(tmp_addr+cnt_addr+8, 0xCC);
	for(i=0;i<15;i++)
	{
		tmp_strdat[i] = 0x300+i;
		DataFlash_byte_write(tmp_addr+cnt_addr+12+(i<<2),i+1);
	}

	printf("## DataFlash Read PAGE:0x%.3X  ",tmp_addr);
	for(i=0;i<DF_PAGE_LEN;i=i+4)
	{
		u32_dat = DataFlash_byte_read(tmp_addr+i);
		printf("%.8X",u32_dat);
	}
	printf("\r\n");

	DataFlash_str_write(tmp_addr+cnt_addr+60,tmp_strdat,15);
	printf("## DataFlash Write:0x%.3X  ",tmp_addr+cnt_addr+60);
	for(i=0;i<15;i++)
	{
		printf("%.8X",tmp_strdat[i]);
	}
	printf("\r\n");

	DataFlash_str_read(tmp_addr+cnt_addr+60,tmp_strdat2,15);
	printf("## DataFlash Read: 0x%.3X  ",tmp_addr+cnt_addr+60);
	for(i=0;i<15;i++)
	{
		printf("%.8X",tmp_strdat2[i]);
	}
	printf("\r\n");


	printf("## DataFlash Read PAGE:0x%.3X  ",tmp_addr);
	for(i=0;i<DF_PAGE_LEN;i=i+4)
	{
		u32_dat = DataFlash_byte_read(tmp_addr+i);
		printf("%.8X",u32_dat);
	}
	printf("\r\n");

	cnt_addr = cnt_addr+4;
	if(cnt_addr>400)
		cnt_addr = 0;
	return 1;
}


