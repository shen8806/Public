#ifndef	_DATAFLASH_H_
#define	_DATAFLASH_H_

#include "M0518.h"


//// Data Flash: 0x0001F000~0x0001FFFF(4KB), Page:0x200 512字节					////
//// Flash里的存储结构是4321的形式存储，格式如:0x12345678存储为 78 56 34 12  	////
//// 0x0001FE00~0x0001FFFF 为临时转存地址

#define DF_REF_ADDR			0x0001F000			//DataFlash的起始地址
#define DF_PAGE_LEN			512					//页的长度，xx字节 512/4=128


////以下地址是以DF_REF_ADDR为起始地址算起
#define SSD_NUM_FlashADDR0	0x000	//上电读取的加密字符串
#define DEVICE_ID_ADDR0		0x040	//设备本身的ID号，根据芯片ID生成，2个u32
#define DF_TMP_ADDR			0xE00	//DataFlash的临时转存地址


#define RF_ADDR_LEN			8		//目的/原端地址的长度 xx字节
#define DEVICE_ID_SDAT 		0x47,0x42,0x00,0x00, 0x00,0x00,0x00,0x00
//默认ID号，2个字节设备号，1个字节异或和，5个字节ID截取



extern  const uint32_t str_ssd_num[];	
extern  uint8_t device_id_str[];

void read_uid(void);
void device_id_get(void);
uint8_t DataFlash_byte_write(uint16_t addr,uint32_t sbyte);
uint32_t DataFlash_byte_read(uint16_t addr);
uint8_t DataFlash_str_write(uint16_t addr0, uint32_t *pbyte, uint16_t slen);
void DataFlash_str_read(uint16_t addr0, uint32_t *pbyte, uint16_t slen);
uint8_t DataFlash_test(void);


#endif


