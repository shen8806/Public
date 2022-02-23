//#include "FAN_app.h"
#include "public.h"

sFan_DAT Fan_DAT;


/////////////// 风机控制的初始化 ///////////////
void FAN_init(void)
{
	Fan_DAT.dev_onoff 		= 0;
	Fan_DAT.hot_air			= 0;
	Fan_DAT.loop_mod		= 0;
	Fan_DAT.fan_level		= 0;
	Fan_DAT.fan_level2		= 0;
	Fan_DAT.ctrl_percen		= 0;
	Fan_DAT.ctrl_percen2	= 0;
	Fan_DAT.speed_gap 		= (FAN_SPEED_MAX-FAN_SPEED_MIN)/(FAN_LEVEL_MAX-1);
	Fan_DAT.target_percen	= 0;
	//Fan_DAT.fan_flag		= 0;
	Fan_DAT.fan_tim_cnt		= 0;
	Fan_DAT.off_tim_cnt		= 0;
	Fan_DAT.off_flag		= 0;
	FAN_mod_set(FANMOD_AUTO);
}

/////////////// 风机控制的模式设置 ///////////////
void FAN_mod_set(uint8_t mod_dat)
//mod_dat: 模式 FANMOD_MANUAL、FANMOD_AUTO..
{
	static uint8_t mod_old = 0xFF;
	if( mod_dat!=mod_old )
	{
		mod_old = Fan_DAT.dev_mod;
		LCD_display_en();
	}
	Fan_DAT.dev_mod = mod_dat;
}

/////////////// 风机控制的模式切换到下一个 ///////////////
void FAN_mod_next(void)
{
	uint8_t u8_tmp;
	switch(Fan_DAT.dev_mod)
	{
		case FANMOD_MANUAL:
			Fan_DAT.dev_mod = FANMOD_AUTO;
			u8_tmp = FAN_level_to_percen(2);
			CTRL_speed_set(u8_tmp);	
			break;
			
		case FANMOD_AUTO: 		
			Fan_DAT.dev_mod = FANMOD_SLEEP; 
			CTRL_speed_set(FAN_SPEED_SLEEP);
			break;
		case FANMOD_SLEEP: 		
			Fan_DAT.dev_mod = FANMOD_STRONG; 
			CTRL_speed_set(FAN_SPEED_STRONG);
			break;
		
		case FANMOD_STRONG: 
			Fan_DAT.dev_mod = FANMOD_MANUAL; 
			u8_tmp = FAN_level_to_percen(2);
			CTRL_speed_set(u8_tmp);		
			break;
		
		default: 				
			Fan_DAT.dev_mod = FANMOD_AUTO;
			u8_tmp = FAN_level_to_percen(2);
			CTRL_speed_set(u8_tmp);
	}
	LCD_display_en();
	buzz_on_def();
}

/////////////// 根据百分比得出档位 ///////////////
uint8_t FAN_percen_to_level(uint8_t percen)
{
	uint8_t level;
	uint8_t gap_2 = Fan_DAT.speed_gap/2;
	if(percen==0)
		level = 0;
	else if(percen<(FAN_SPEED_MIN+gap_2))
		level = 1;
	else if(percen<(FAN_SPEED_MIN+Fan_DAT.speed_gap+gap_2))
		level = 2;
	else if(percen<(FAN_SPEED_MIN+Fan_DAT.speed_gap+Fan_DAT.speed_gap+gap_2))
		level = 3;
	else
		level = 4;
	return level;
}

/////////////// 根据档位转换成百分比 ///////////////
uint8_t FAN_level_to_percen(uint8_t level)
{
	uint8_t percen = 0;;
	switch(level)
	{
		case 0: percen = 0; break;
		case 1: percen = FAN_SPEED_MIN; 										break;
		case 2: percen = FAN_SPEED_MIN+Fan_DAT.speed_gap; 					break;
		case 3: percen = FAN_SPEED_MIN+Fan_DAT.speed_gap+Fan_DAT.speed_gap; 	break;
		case 4: percen = FAN_SPEED_MAX; 										break;
		default: percen = FAN_SPEED_MAX;
	}
	return percen;
}

/////////////// 风机根据传感器的百分比调整 ///////////////
void FAN_adj_sensor(uint16_t pm25_dat, uint16_t co2_dat, uint16_t ch2o_dat)
{
	uint8_t  flag=0;
	uint16_t u16_dat,u16_tmp1,u16_tmp2;
	static uint16_t SENSOR_dat_str[SENSOR_MEM_NUM]={0};
	static int16_t sen_sum=0;	//和
	static uint16_t sen_ave=0;	//平均值
	static uint8_t 	sen_str_cnt = 0;

	const uint16_t SEN_SUM_MIN = 440;		//10+400+20
	const uint16_t SEN_SUM_MAX = 1250;	//150+1000+100

	if( sen_str_cnt>=SENSOR_MEM_NUM )
		sen_str_cnt = 0;
	u16_dat = pm25_dat+co2_dat+ch2o_dat;
	sen_sum = sen_sum + u16_dat - SENSOR_dat_str[sen_str_cnt];
	SENSOR_dat_str[sen_str_cnt] = u16_dat;
	sen_str_cnt++;

	if(sen_sum>0)
		sen_ave = sen_sum/SENSOR_MEM_NUM;
	else
		sen_ave = 0;

	

	if(pm25_dat>150 || co2_dat>1000 || ch2o_dat>100)			//达到中度污染时
	{
		Fan_DAT.target_percen = FAN_SPEED_MAX;
		flag = 1;
	}
	else if(pm25_dat<35 && co2_dat<450 && ch2o_dat<30)			//处于 优 时
	{
		Fan_DAT.target_percen = FAN_SPEED_MIN;
		flag = 1;
	}

	if(flag==0)
	{
		if( sen_ave>SEN_SUM_MAX )
			Fan_DAT.target_percen = FAN_SPEED_MAX;
		else if( sen_ave<SEN_SUM_MIN )
			Fan_DAT.target_percen = FAN_SPEED_MIN;
		else
		{
			u16_tmp1 = (sen_ave - SEN_SUM_MIN)*(FAN_SPEED_MAX-FAN_SPEED_MIN);
			u16_tmp2 = u16_tmp1/(SEN_SUM_MAX-SEN_SUM_MIN);
			if( u16_tmp2>(FAN_SPEED_MAX-FAN_SPEED_MIN) )
				u16_tmp2 = FAN_SPEED_MAX-FAN_SPEED_MIN;
			Fan_DAT.target_percen = FAN_SPEED_MIN+u16_tmp2;
		}
	}

	/*
	if(flag==0)
	{
		if(pm25_dat>pm25_old)
		{
			if( (pm25_dat-pm25_old)>3	)// && pm25_dat>55)
			{
				Fan_DAT.target_percen += 7;
				if( Fan_DAT.target_percen > FAN_SPEED_MAX )
					Fan_DAT.target_percen = FAN_SPEED_MAX;
				flag = 1;
			}
		}
		else
		{
			if( (pm25_old-pm25_dat)>3 ) //&& pm25_dat<50)
			{
				Fan_DAT.target_percen -= 6;
				if( Fan_DAT.target_percen < FAN_SPEED_MIN )
					Fan_DAT.target_percen = FAN_SPEED_MIN;
				
				flag = 1;
			}
		}

		if(flag==0)
		{
			if(co2_dat > co2_old)
			{
				if( (co2_dat-co2_old)>6 ) //&& co2_dat>800)
				{
					Fan_DAT.target_percen += 7;
					if( Fan_DAT.target_percen > FAN_SPEED_MAX )
						Fan_DAT.target_percen = FAN_SPEED_MAX;
					flag = 1;
				}
			}
			else
			{
				if( (co2_old-co2_dat)>6 ) //&& co2_dat<750 )
				{
					Fan_DAT.target_percen -= 6;
					if( Fan_DAT.target_percen < FAN_SPEED_MIN )
						Fan_DAT.target_percen = FAN_SPEED_MIN;
					flag = 1;
				}
			}
		}

		if(flag==0)
		{
			if(ch2o_dat > ch2o_old)
			{
				if( (ch2o_dat-ch2o_old)>3 ) //&& ch2o_dat>60)
				{
					Fan_DAT.target_percen += 7;
					if( Fan_DAT.target_percen > FAN_SPEED_MAX )
						Fan_DAT.target_percen = FAN_SPEED_MAX;
					flag = 1;
				}
			}
			else
			{
				if( (ch2o_old-ch2o_dat)>3 ) //&& ch2o_dat<55)
				{
					Fan_DAT.target_percen -= 6;
					if( Fan_DAT.target_percen < FAN_SPEED_MIN )
						Fan_DAT.target_percen = FAN_SPEED_MIN;
					flag = 1;
				}
			}
		}
	}
	pm25_old 	= pm25_dat;
	co2_old 	= co2_dat;
	ch2o_old 	= ch2o_dat;
	*/
}

/////////////// 风机相关的定时器轮询 ///////////////
void FAN_tim_poll(void)	//10mS轮询一次
{
	if(Fan_DAT.dev_mod==FANMOD_AUTO)
	{
		if( Fan_DAT.fan_tim_cnt )
			Fan_DAT.fan_tim_cnt--;
	}
	
	if(Fan_DAT.off_flag)
	{
		if( Fan_DAT.off_tim_cnt	)
			Fan_DAT.off_tim_cnt--;
	}
}

///////////// 风机调速函数 ////////////////
void FAN_set_val_run(uint8_t percen_dat)
{
	//uint8_t u8_tmp;
	if( percen_dat <= Fan_DAT.target_percen )
	{
		if( (Fan_DAT.target_percen-percen_dat)<=FAN_DIFF_ADD )
		{
			CTRL_speed_set(Fan_DAT.target_percen);
		}
		else
		{
			CTRL_speed_set(percen_dat+FAN_DIFF_ADD);
		}
	}
	else
	{
		if( (percen_dat - Fan_DAT.target_percen)<=FAN_DIFF_ADD )
		{
			CTRL_speed_set(Fan_DAT.target_percen);
		}
		else
		{
			CTRL_speed_set(percen_dat-FAN_DIFF_CUT);
		}
	}
}

///////////// 风机档位调整 ////////////////
void FAN_set_level(void)
{
	uint8_t u8_tmp,u8_tmp2;
	
	FAN_mod_set(FANMOD_MANUAL);
	if( Fan_DAT.loop_mod==0)
	{
		u8_tmp = FAN_percen_to_level(Fan_DAT.ctrl_percen);
	}
	else
	{
		u8_tmp = FAN_percen_to_level(Fan_DAT.ctrl_percen2);
	}
	u8_tmp++;
	if(u8_tmp>FAN_LEVEL_MAX)
		u8_tmp = 1;
	u8_tmp2 = FAN_level_to_percen(u8_tmp);
	CTRL_speed_set(u8_tmp2);
	LCD_display_en();
	buzz_on_def();
}

///////////// 风机定时关机设置 ////////////////
void FAN_set_OFFtim(void)
{
	static uint8_t off_tim_h=0;					//定时关机 小时
	
	if( Fan_DAT.off_flag==0 )
		Fan_DAT.off_flag = 1;
	switch(off_tim_h)
	{
		case 12: 	off_tim_h=0;	break;		//0小时
		case 0: 	off_tim_h=1;	break;		//2小时
		case 1: 	off_tim_h=2;	break;		//2小时
		case 2: 	off_tim_h=3;	break;		//3小时
		case 3: 	off_tim_h=6;	break;		//6小时
		case 6: 	off_tim_h=12;	break;		//12小时
		default:	off_tim_h=1;
	}
	if(off_tim_h==0)
	{
		Fan_DAT.off_flag = 0;
		Fan_DAT.off_tim_cnt = 0;
	}
	else
		Fan_DAT.off_tim_cnt = off_tim_h*3600*100;	//1h=3600 00
	LCD_display_en();
	buzz_on_def();
}

///////////// 加热模块设置 ////////////////
void FAN_set_HOTair(void)
{
	//Fan_DAT.hot_air++;
	if(Fan_DAT.hot_air == 0 )
		Fan_DAT.hot_air = 3;
	//if(Fan_DAT.hot_air>3)
	else
		Fan_DAT.hot_air = 0;
	CTRL_TX_set(UART_HEATING_SET);
	CTRL_TX_poll();
	LCD_display_en();
	buzz_on_def();
}


/////////////// 风机整机开机配置 ///////////////
void FANall_on_set(void)
{
	Fan_DAT.dev_onoff = 1;
	LED_showONOFF(1);
	CTRL_speed_set(FAN_SPEED_MIN);
	FAN_mod_set(FANMOD_AUTO);
	printf("** FAN ON **\r\n");
	buzz_on_set(BUZZ_TIM_100MS);
}

/////////////// 风机整机关机配置 ///////////////
void FANall_off_set(void)
{
	Fan_DAT.dev_onoff = 0;
	LED_showONOFF(0);
	CTRL_speed_set(0);
	printf("** FAN OFF!! **\r\n");
	buzz_on_def();
}


/////////////// 风机运行的轮询 ///////////////
void FAN_run_poll(void)
{
	if(Fan_DAT.dev_mod==FANMOD_AUTO)	//自动模式
	{
		//if(Fan_DAT.fan_flag)
		if(Fan_DAT.fan_tim_cnt==0 && Fan_DAT.dev_onoff==1)
		{
			Fan_DAT.fan_tim_cnt = 1000;
			//Fan_DAT.fan_flag = 0;
			
			if(Fan_DAT.loop_mod==0)
				FAN_set_val_run( Fan_DAT.ctrl_percen );
			else
				FAN_set_val_run( Fan_DAT.ctrl_percen2 );
		}
	}


	if(Fan_DAT.off_flag)
	{
		if( Fan_DAT.off_tim_cnt==0	)
		{
			Fan_DAT.off_flag = 0;
			FANall_off_set();
			LCD_display_en();
		}
	}

	if(Fan_DAT.dev_onoff==0)	//关机时
	{
		if(Fan_DAT.hot_air)
		{
			Fan_DAT.hot_air = 0;
			CTRL_TX_set(UART_HEATING_SET);
			CTRL_TX_poll();
		}
	}
}



