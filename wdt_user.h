#ifndef	_WDT_USER_H_
#define	_WDT_USER_H_

#include "M0518.h"


#define delay_us(x)		CLK_SysTickDelay(x)


void WDT_Init(void);
void WDT_CR_set(void);
void WDT_poll(void);


void CPU_rst(void);
void delay_ms(uint16_t ms_dat);

#endif


