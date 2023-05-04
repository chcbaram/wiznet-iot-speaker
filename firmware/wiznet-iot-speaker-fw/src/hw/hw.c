#include "hw.h"






bool hwInit(void)
{
  bspInit();

  cliInit();
  logInit();
  ledInit();
  buttonInit();
  uartInit();
  logOpen(_DEF_UART1, 115200);

  logPrintf("\r\n[ Firmware Begin... ]\r\n");
  logPrintf("Booting..Name \t\t: %s\r\n", _DEF_BOARD_NAME);
  logPrintf("Booting..Ver  \t\t: %s\r\n", _DEF_FIRMWATRE_VERSION);  
  logPrintf("Booting..Clock\t\t: %d Mhz\r\n", (int)HAL_RCC_GetSysClockFreq()/1000000);
  logPrintf("\n");

  fmcInit();
  
  return true;
}