//#include "key.h"
//#include "wdt_user.h"
#include "public.h"


tKey_Information sKey_Information[KEY_MAX_NUM];

uint8_t key_get(uint8_t key_num);

/////////////// 按键初始化 ///////////////
void key_init(void)
{
	uint8_t i;
	//KEY1 = 1;
	//KEY2 = 1;
	//KEY3 = 1;
	//KEY4 = 1;
	//KEY5 = 1;
	//KEY6 = 1;
	for(i=0; i<KEY_MAX_NUM ;i++)
	{
		key_get( i );
		sKey_Information[i].value	= KEY_VALUE_NULL;
		sKey_Information[i].type	= KEY_TYPE_NULL;
		sKey_Information[i].type_old	= sKey_Information[i].type;
		sKey_Information[i].timer = 0;
		key_type_clr(i);
	}
}

/////////////// 获取按键的电平 ///////////////
uint8_t key_get(uint8_t key_num)
//key_num: 0..KEY_MAX_NUM
{
	switch(key_num)
	{
		case 0: sKey_Information[0].bin_10 = KEY1; break;
		case 1: sKey_Information[1].bin_10 = KEY2; break;
		case 2: sKey_Information[2].bin_10 = KEY3; break;
		case 3: sKey_Information[3].bin_10 = KEY4; break;
		case 4: sKey_Information[4].bin_10 = KEY5; break;
		case 5: sKey_Information[5].bin_10 = KEY6; break;
		default: return 0;  
	}
	return 1;
}


/////////////// 获取按键的状态 ///////////////
uint8_t key_type_get(uint8_t key_num)
{
	if(key_num < KEY_MAX_NUM)
		return sKey_Information[key_num].type;
	else
		return KEY_TYPE_NULL;
}

/////////////// 按键状态清除 ///////////////
void key_type_clr(uint8_t key_num)
{
	sKey_Information[key_num].type = KEY_TYPE_NULL;
	sKey_Information[key_num].timer = 0;
	sKey_Information[key_num].value	= KEY_VALUE_NULL;
	sKey_Information[key_num].bin_10= 1;
}



/////////////// 按键扫描轮询 ///////////////
void key_scan_poll(void)
{
	uint8_t i;
	for(i=0; i<KEY_MAX_NUM; i++)
	{
		if(sKey_Information[i].value == KEY_VALUE_NULL)
		{
			key_get( i );
			if(sKey_Information[i].bin_10 == 0)
			{
				sKey_Information[i].value = KEY_VALUE_PRESS;
				sKey_Information[i].timer = 0;
			}
		}
		else
		{
			key_get( i );
	
			if(sKey_Information[i].bin_10)
			{
				if(sKey_Information[i].value == KEY_VALUE_PRESS)
					sKey_Information[i].value = KEY_VALUE_RELEASE;
				else if(sKey_Information[i].value == KEY_VALUE_RELEASE)
					sKey_Information[i].value = KEY_VALUE_NULL;
			}
			
			if(sKey_Information[i].value == KEY_VALUE_PRESS)			//按着
			{
				sKey_Information[i].timer++;
				if(sKey_Information[i].timer >= KEY_TIME_LONG)	//长按，未松手
					sKey_Information[i].type = KEY_TYPE_SLONG;
			}
			else if(sKey_Information[i].value == KEY_VALUE_RELEASE)		//已经松手
			{
				if((sKey_Information[i].timer >= KEY_TIME_SHORT) && (sKey_Information[i].timer < KEY_TIME_LONG))
				{
					sKey_Information[i].type_old = sKey_Information[i].type;
					sKey_Information[i].type = KEY_TYPE_SHORT;	//短按，已经松手
					//printf("## KEY %d TYPE: %d  %dmS\n",i+1,sKey_Information[i].type,sKey_Information[i].timer*10);
				}
				else if(sKey_Information[i].timer >= KEY_TIME_LONG)
				{
					sKey_Information[i].type_old = sKey_Information[i].type;
					sKey_Information[i].type = KEY_TYPE_LONG;	//长按，已经松手
					//printf("## KEY %d TYPE: %d  %dmS\n",i+1,sKey_Information[i].type,sKey_Information[i].timer*10);
				}
			}
		}
	}
}

/////////////// 按键执行轮询 ///////////////
void key_run_poll(void)
{
	int8_t i,ok_flag;
	static int8_t slong_flag[KEY_MAX_NUM] = {0};
	
	
	for(i=0; i<KEY_MAX_NUM; i++)
	{
		if(key_type_get(i) == KEY_TYPE_SHORT)		//短按
		{
			if(Fan_DAT.dev_onoff)
			{
				switch(i)
				{
					case KEY_LOOP:
						;
						/*
						CTRL_speed_set(0);
						if( Fan_DAT.loop_mod==0)
							Fan_DAT.loop_mod = 1;
						else
							Fan_DAT.loop_mod = 0;
						*/
					break;

					case KEY_MOD:
						FAN_mod_next();
					break;

					case KEY_LEVEL:
						FAN_set_level();
					break;

					case KEY_TIM:
						FAN_set_OFFtim();
					break;

					case KEY_HOT:
						FAN_set_HOTair();
					break;
				}
			}

			delay_ms(1);
			key_type_clr(i);
			slong_flag[i] = 0;
		}
		else if(key_type_get(i) == KEY_TYPE_SLONG)	//长按着
		{
			ok_flag = 0;
			if(slong_flag[i] == 0)
			{
				if(i==KEY_ONOFF)
				{
					if(Fan_DAT.dev_onoff==0)
					{
						FANall_on_set();
					}
					else
					{
						FANall_off_set();
					}
					LCD_display_en();
					delay_ms(10);
				}
				else if(i==KEY_TIM)
				{
					if(key_type_get(KEY_HOT) == KEY_TYPE_SLONG)	//先按着KEY_HOT再按着KEY_TIM
					{
						ok_flag = 1;
					}
				}
				else if(i==KEY_HOT)
				{
					if(key_type_get(KEY_TIM) == KEY_TYPE_SLONG)	//先按着KEY_TIM再按着KEY_HOT
					{
						ok_flag = 1;
					}
				}
				
				if(ok_flag)
				{
					if(Fan_DAT.dev_onoff==1)
					{
						M433_COM_reset_def();
						
						buzz_on_set(BUZZ_TIM_70MS);
						delay_ms(BUZZ_TIM_70MS+30);
						buzz_on_set(BUZZ_TIM_70MS);
						delay_ms(20);
						
						M433_COM_set();
					}
					ok_flag = 0;
				}
				slong_flag[i] = 1;
			}
			//else
			//	key_type_clr(i);
		}
		else if(key_type_get(i) == KEY_TYPE_LONG)	//长按放开
		{
			delay_ms(10);
			key_type_clr(i);
			slong_flag[i] = 0;
		}
	}
}


