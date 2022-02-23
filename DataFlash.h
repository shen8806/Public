#ifndef	_DATAFLASH_H_
#define	_DATAFLASH_H_

#include "M0518.h"


//// Data Flash: 0x0001F000~0x0001FFFF(4KB), Page:0x200 512�ֽ�					////
//// Flash��Ĵ洢�ṹ��4321����ʽ�洢����ʽ��:0x12345678�洢Ϊ 78 56 34 12  	////
//// 0x0001FE00~0x0001FFFF Ϊ��ʱת���ַ

#define DF_REF_ADDR			0x0001F000			//DataFlash����ʼ��ַ
#define DF_PAGE_LEN			512					//ҳ�ĳ��ȣ�xx�ֽ� 512/4=128


////���µ�ַ����DF_REF_ADDRΪ��ʼ��ַ����
#define SSD_NUM_FlashADDR0	0x000	//�ϵ��ȡ�ļ����ַ���
#define DEVICE_ID_ADDR0		0x040	//�豸�����ID�ţ�����оƬID���ɣ�2��u32
#define DF_TMP_ADDR			0xE00	//DataFlash����ʱת���ַ


#define RF_ADDR_LEN			8		//Ŀ��/ԭ�˵�ַ�ĳ��� xx�ֽ�
#define DEVICE_ID_SDAT 		0x47,0x42,0x00,0x00, 0x00,0x00,0x00,0x00
//Ĭ��ID�ţ�2���ֽ��豸�ţ�1���ֽ����ͣ�5���ֽ�ID��ȡ



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


