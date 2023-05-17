#include "ap.h"







void apInit(void)
{
  cliOpen(_DEF_UART1, 115200);

  logBoot(false);
}

void apMain(void)
{
  uint32_t pre_time;
  uint8_t rot_i = 0;
  uint8_t rot_mode_tbl[2] = {4, 3};


  pre_time = millis();
  while(1)
  {
    if (millis()-pre_time >= 500)
    {
      pre_time = millis();
      ledToggle(_DEF_LED1);
      ledToggle(_DEF_LED2);
    }    

    if (buttonGetPressed(_DEF_BUTTON3))
    {
      rot_i = (rot_i + 1) % 2;
      lcdSetRotation(rot_mode_tbl[rot_i]);
      logPrintf("rot : %d\n", rot_i);
      lcdLogoOn();
      delay(50);
      while(buttonGetPressed(_DEF_BUTTON3));
    }
    cliMain();
    sdUpdate();
  }
}
