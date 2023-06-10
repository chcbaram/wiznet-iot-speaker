#include "hw.h"
#include "wiznet/wiznet.h"



extern uint32_t _fw_flash_begin;

volatile const firm_ver_t firm_ver __attribute__((section(".version"))) = 
{
  .magic_number = VERSION_MAGIC_NUMBER,
  .version_str  = _DEF_FIRMWATRE_VERSION,
  .name_str     = _DEF_BOARD_NAME,
  .firm_addr    = (uint32_t)&_fw_flash_begin
};




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
  spiInit();
  uartInit();
  uartOpen(_DEF_UART1, 115200);
  uartOpen(_DEF_UART2, 115200);

  logOpen(_DEF_UART1, 115200);
  logPrintf("\r\n[ Firmware Begin... ]\r\n");
  logPrintf("Booting..Name \t\t: %s\r\n", _DEF_BOARD_NAME);
  logPrintf("Booting..Ver  \t\t: %s\r\n", _DEF_FIRMWATRE_VERSION);  
  logPrintf("Booting..Clock\t\t: %d Mhz\r\n", (int)HAL_RCC_GetSysClockFreq()/1000000);
  logPrintf("\n");

  rtcInit();
  resetInit();
  faultInit();
  qspiInit();
  flashInit();
  fsInit();
  nvsInit();

  pwmInit();
  fmcInit();
  buzzerInit();
  i2sInit();
  saiInit();
  es8156Init();
  w5300Init();
  dxlInit();
  sdInit();
  fatfsInit();

  usbInit();
  if (buttonGetPressed(_DEF_BUTTON2) == true)
  {
    usbBegin(USB_MSC_MODE);
  }
  else
  {
    usbBegin(USB_CDC_MODE);
    cdcInit();
  }

  encoderInit();
  lcdInit();

  wiznetInit();
  wiznetDHCP();

  return true;
}