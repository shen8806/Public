#ifndef _INITIALIZATION_H_
#define _INITIALIZATION_H_


#define PLL_CLOCK           50000000

//// 输入
#define PIN0_PMD_IN      0x00000000UL
#define PIN1_PMD_IN      0x00000000UL
#define PIN2_PMD_IN      0x00000000UL
#define PIN3_PMD_IN      0x00000000UL
#define PIN4_PMD_IN      0x00000000UL
#define PIN5_PMD_IN      0x00000000UL
#define PIN6_PMD_IN      0x00000000UL
#define PIN7_PMD_IN      0x00000000UL
#define PIN8_PMD_IN      0x00000000UL
#define PIN9_PMD_IN      0x00000000UL
#define PIN10_PMD_IN     0x00000000UL
#define PIN11_PMD_IN     0x00000000UL
#define PIN12_PMD_IN     0x00000000UL
#define PIN13_PMD_IN     0x00000000UL
#define PIN14_PMD_IN     0x00000000UL
#define PIN15_PMD_IN     0x00000000UL

//// 输出
#define PIN0_PMD_OUT      0x00000001UL
#define PIN1_PMD_OUT      0x00000004UL
#define PIN2_PMD_OUT      0x00000010UL
#define PIN3_PMD_OUT      0x00000040UL
#define PIN4_PMD_OUT      0x00000100UL
#define PIN5_PMD_OUT      0x00000400UL
#define PIN6_PMD_OUT      0x00001000UL
#define PIN7_PMD_OUT      0x00004000UL
#define PIN8_PMD_OUT      0x00010000UL
#define PIN9_PMD_OUT      0x00040000UL
#define PIN10_PMD_OUT     0x00100000UL
#define PIN11_PMD_OUT     0x00400000UL
#define PIN12_PMD_OUT     0x01000000UL
#define PIN13_PMD_OUT     0x04000000UL
#define PIN14_PMD_OUT     0x10000000UL
#define PIN15_PMD_OUT     0x40000000UL

//// 开漏
#define PIN0_PMD_OP      0x00000002UL
#define PIN1_PMD_OP      0x00000008UL
#define PIN2_PMD_OP      0x00000020UL
#define PIN3_PMD_OP      0x00000080UL
#define PIN4_PMD_OP      0x00000200UL
#define PIN5_PMD_OP      0x00000800UL
#define PIN6_PMD_OP      0x00002000UL
#define PIN7_PMD_OP      0x00008000UL
#define PIN8_PMD_OP      0x00020000UL
#define PIN9_PMD_OP      0x00080000UL
#define PIN10_PMD_OP     0x00200000UL
#define PIN11_PMD_OP     0x00800000UL
#define PIN12_PMD_OP     0x02000000UL
#define PIN13_PMD_OP     0x08000000UL
#define PIN14_PMD_OP     0x20000000UL
#define PIN15_PMD_OP     0x80000000UL

//// 双向
#define PIN0_PMD_QUA      0x00000003UL
#define PIN1_PMD_QUA      0x0000000CUL
#define PIN2_PMD_QUA      0x00000030UL
#define PIN3_PMD_QUA      0x000000C0UL
#define PIN4_PMD_QUA      0x00000300UL
#define PIN5_PMD_QUA      0x00000C00UL
#define PIN6_PMD_QUA      0x00003000UL
#define PIN7_PMD_QUA      0x0000C000UL
#define PIN8_PMD_QUA      0x00030000UL
#define PIN9_PMD_QUA      0x000C0000UL
#define PIN10_PMD_QUA     0x00300000UL
#define PIN11_PMD_QUA     0x00C00000UL
#define PIN12_PMD_QUA     0x03000000UL
#define PIN13_PMD_QUA     0x0C000000UL
#define PIN14_PMD_QUA     0x30000000UL
#define PIN15_PMD_QUA     0xC0000000UL



void SYS_Init(void);
void IO_Init(void);
void UART0_Init(void);
void TMR1_Init(void);
void PWM_Init(void);

/*
void I2C_Init(void);
void PWMA_Init(void);
void PWMB_Init(void);
void SPI0_Init(void);
void SPI1_Init(void);
void TMR0_Init(void);
//void UART1_Init(void);
//void UART1_IRQHandler(void);
//void ADC_Init(void);
void HDIV_Init(void);

*/


#endif



