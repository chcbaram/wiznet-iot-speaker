#ifndef BSP_H_
#define BSP_H_


#include "def.h"


#include "stm32h7xx_hal.h"


typedef struct
{
  uint32_t magic_number;
  uint32_t type;

  uint32_t REG_R0;
  uint32_t REG_R1;
  uint32_t REG_R2;
  uint32_t REG_R3;
  uint32_t REG_R12;
  uint32_t REG_LR;
  uint32_t REG_PC;
  uint32_t REG_PSR;

} fault_log_t;



void logPrintf(const char *fmt, ...);



bool bspInit(void);

void delay(uint32_t time_ms);
uint32_t millis(void);

void Error_Handler(void);


#endif