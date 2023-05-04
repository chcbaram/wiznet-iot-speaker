#include "ap.h"







void apInit(void)
{
  cliOpen(_DEF_UART1, 115200);

  logBoot(false);

  gpioPinWrite(_PIN_GPIO_W5300_RST, _DEF_LOW);
  delay(5);
  gpioPinWrite(_PIN_GPIO_W5300_RST, _DEF_HIGH);
}

void apMain(void)
{
  uint32_t pre_time;


  pre_time = millis();
  while(1)
  {
    if (millis()-pre_time >= 500)
    {
      pre_time = millis();
      ledToggle(_DEF_LED1);
      ledToggle(_DEF_LED2);
    }    

    cliMain();
  }
}
