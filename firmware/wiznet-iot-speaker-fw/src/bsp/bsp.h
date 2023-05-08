#ifndef BSP_H_
#define BSP_H_


#include "def.h"


#include "stm32h7xx_hal.h"





void logPrintf(const char *fmt, ...);



bool bspInit(void);

void delay(uint32_t time_ms);
uint32_t millis(void);

void Error_Handler(void);


#endif