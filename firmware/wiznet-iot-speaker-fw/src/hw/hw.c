#include "hw.h"






bool hwInit(void)
{
  bspInit();

  cliInit();
  logInit();
  swtimerInit();
  ledInit();
  buttonInit();
  gpioInit();
  i2cInit();
  uartInit();
  uartOpen(_DEF_UART1, 115200);
  uartOpen(_DEF_UART2, 115200);

  logOpen(_DEF_UART1, 115200);
  logPrintf("\r\n[ Firmware Begin... ]\r\n");
  logPrintf("Booting..Name \t\t: %s\r\n", _DEF_BOARD_NAME);
  logPrintf("Booting..Ver  \t\t: %s\r\n", _DEF_FIRMWATRE_VERSION);  
  logPrintf("Booting..Clock\t\t: %d Mhz\r\n", (int)HAL_RCC_GetSysClockFreq()/1000000);
  logPrintf("\n");

  faultInit();
  qspiInit();
  flashInit();
  fsInit();
  fmcInit();
  buzzerInit();
  i2sInit();
  w5300Init();
  dxlInit();
  sdInit();
  fatfsInit();

  return true;
}