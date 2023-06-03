#include "ap.h"
#include "thread/cmd/cmd_thread.h"
#include "thread/boot/boot.h"

bool is_run_fw = true;
bool is_update_fw = false;


void apInit(void)
{
  uint32_t boot_param;
  uint16_t err_code;

  boot_param = resetGetBootMode();

  if (boot_param & (1<<MODE_BIT_BOOT))
  {
    boot_param &= ~(1<<MODE_BIT_BOOT);
    resetSetBootMode(boot_param);
    
    is_run_fw = false;
  }

  if (buttonGetPressed(_DEF_BUTTON1) == true)
  {
    is_run_fw = false;
  }

  if (boot_param & (1<<MODE_BIT_UPDATE))
  {
    boot_param &= ~(1<<MODE_BIT_UPDATE);
    resetSetBootMode(boot_param);
    
    is_run_fw = true;
    is_update_fw = true;
  }
  logPrintf("\n");


  if (is_update_fw)
  {
    logPrintf("[  ] bootUpdateFirm()\r");
    err_code = bootUpdateFirm();
    logPrintf("[%s]\n", err_code==CMD_OK ? "OK":"NG");
    if (err_code != CMD_OK)
      logPrintf("     err : 0x%04X\n", err_code);
  }

  if (faultIsReady())
  {
    is_run_fw = false;
  }

  if (is_run_fw)
  {
    logPrintf("[  ] bootJumpFirm()\r");
    err_code = bootJumpFirm();
    logPrintf("[%s]\n", err_code==CMD_OK ? "OK":"NG");
    if (err_code != CMD_OK)
      logPrintf("     err : 0x%04X\n", err_code);
  }

  delay(100);
  usbInit();
  usbBegin(USB_CDC_MODE);
  cdcInit();

  logPrintf("\n");
  logPrintf("Boot Mode..\n");  
}

void apMain(void)
{
  uint32_t pre_time;
  uint8_t cli_ch;


  cliOpen(_DEF_UART1, 115200);
  cmdThreadInit();
  
  pre_time = millis();
  while(1)
  {
    if (millis()-pre_time >= 100)
    {
      pre_time = millis();
      ledToggle(_DEF_LED1);
      ledOff(_DEF_LED2);
    }    

    sdUpdate();

    if (usbIsOpen() && usbGetType() == USB_CON_CLI)
    {
      cli_ch = HW_UART_CH_USB;
    }
    else
    {
      cli_ch = HW_UART_CH_SWD;
    }
    if (cli_ch != cliGetPort())
    {
      cliOpen(cli_ch, 0);
    }

    if (cli_ch != HW_UART_CH_USB)
    {
      if (cmdThreadUpdate() == true)
      {
        ledToggle(_DEF_LED2);
      }
    }
    cliMain();
  }
}
