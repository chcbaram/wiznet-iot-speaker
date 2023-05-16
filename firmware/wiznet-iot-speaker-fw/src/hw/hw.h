#ifndef HW_H_
#define HW_H_


#include "hw_def.h"

#include "led.h"
#include "uart.h"
#include "cli.h"
#include "log.h"
#include "button.h"
#include "qbuffer.h"
#include "fmc.h"
#include "buzzer.h"
#include "swtimer.h"
#include "gpio.h"
#include "w5300.h"
#include "flash.h"
#include "fault.h"
#include "qspi.h"
#include "fs.h"
#include "dxl.h"
#include "sd.h"
#include "fatfs.h"
#include "i2c.h"
#include "i2s.h"
#include "sai.h"
#include "spi.h"
#include "es8156.h"
#include "encoder.h"
#include "lcd.h"

bool hwInit(void);


#endif