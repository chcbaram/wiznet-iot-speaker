#include "ap.h"
#include "thread/cmd/cmd_thread.h"
#include "wiznet/wiznet.h"



static void lcdUpdate(void);




void apInit(void)
{
  cliOpen(_DEF_UART1, 115200);
  logBoot(false);

  cmdThreadInit();
}

void apMain(void)
{
  uint32_t pre_time;
  uint8_t rot_i = 0;
  uint8_t rot_mode_tbl[2] = {4, 3};
  uint8_t cli_ch;


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
      logPrintf("\nrot : %d\n", rot_mode_tbl[rot_i]);
      lcdLogoOn();
      delay(50);
      while(buttonGetPressed(_DEF_BUTTON3));
    }
    

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


    cliMain();

    sdUpdate();
    wiznetUpdate();
    cmdThreadUpdate();
    lcdUpdate();
  }
}

#include <time.h>

void lcdUpdate(void)
{
  static uint32_t pre_time = 0;
  static uint8_t  index = 0;
  wiznet_info_t net_info;


  if (lcdDrawAvailable() && millis()-pre_time >= 100)
  {
    pre_time = millis();

    lcdClearBuffer(black);  
    lcdLogoDraw(-1, -1);

    wiznetGetInfo(&net_info);

    if (wiznetIsGetIP() == true)
    {
      lcdPrintfRect(0, 150, LCD_WIDTH, 32, white, 32, LCD_ALIGN_H_CENTER|LCD_ALIGN_V_CENTER, 
                    "%d.%d.%d.%d", net_info.ip[0], net_info.ip[1], net_info.ip[2], net_info.ip[3]);
    }
    else
    {
      lcdDrawFillRect(0+60/2, 150+32+2, LCD_WIDTH-60, 10, blue);
      lcdDrawFillRect(0+60/2, 150+32+2, (LCD_WIDTH-60) * index / 10, 10, green);

      lcdPrintfRect(0, 150, LCD_WIDTH, 32, white, 32, LCD_ALIGN_H_CENTER|LCD_ALIGN_V_CENTER, 
                    "Getting_IP");

      index = (index+1)%11;
    }

    rtc_time_t rtc_time;
    rtc_date_t rtc_date;
    const char *week_str[] = {"일", "월", "화", "수", "목", "금", "토"};

    rtcGetTime(&rtc_time);
    rtcGetDate(&rtc_date);

    lcdDrawFillRect(0, 200, LCD_WIDTH, 32, green);
    lcdPrintfRect(0, 200, LCD_WIDTH, 32, black, 32, LCD_ALIGN_H_CENTER|LCD_ALIGN_V_CENTER, 
                  "%02d:%02d:%02d", rtc_time.hours, rtc_time.minutes, rtc_time.seconds);

    lcdDrawFillRect(0, 0, LCD_WIDTH, 32, green);
    lcdPrintfRect(0, 0, LCD_WIDTH, 32, black, 32, LCD_ALIGN_H_CENTER|LCD_ALIGN_V_CENTER, 
                  "%02d-%d-%02d %s", rtc_date.year, rtc_date.month, rtc_date.day, week_str[rtc_date.week]);

    lcdRequestDraw();
  }
}