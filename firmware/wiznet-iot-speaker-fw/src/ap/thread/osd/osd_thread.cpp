#include "osd_thread.h"


enum 
{
  OSD_IDLE,
  OSD_BEGIN,
  OSD_BUSY,
  OSD_END,
};

enum
{
  ENC_L = 0,
  ENC_R = 1,
  ENC_MAX,
};

static void osdThreadISR(void);


static uint8_t osd_state = OSD_IDLE;
static uint8_t osd_mode = 0;
static uint32_t osd_pre_time;
static int32_t osd_enc_cnt[2];
static int32_t osd_enc_pre_cnt[2];
static button_event_t btn_event;




bool osdThreadInit(void)
{
  
  buttonEventInit(&btn_event, 5);


  lcdSetCallbackDraw(osdThreadISR);

  for (int i=0; i<ENC_MAX; i++)
  {
    osd_enc_cnt[i] = encoderGetCount(i);
    osd_enc_pre_cnt[i] = osd_enc_cnt[i];
  }

  return true;
}

bool osdThreadUpdate(void)
{
  return true;
}

void osdThreadISR(void)
{
  for (int i=0; i<ENC_MAX; i++)
  {
    osd_enc_cnt[i] = encoderGetCount(i);
  }

  switch(osd_state)
  {
    case OSD_IDLE:
      if (buttonEventGetPressed(&btn_event, _BTN_ENC_R))
      {
        osd_state = OSD_BEGIN;
        buttonEventClear(&btn_event);
      }
      break;

    case OSD_BEGIN:
      osd_state = OSD_BUSY;
      osd_pre_time = millis();
      osd_enc_pre_cnt[ENC_L] = osd_enc_cnt[ENC_L];  
      osd_enc_pre_cnt[ENC_R] = osd_enc_cnt[ENC_R];  

      osd_mode = 0;
      if (saiIsBusy())
      {
        osd_mode = 1;
      }
      break;
    
    case OSD_BUSY:
      if (buttonEventGetPressed(&btn_event, _BTN_ENC_R))
      {
        osd_state = OSD_END;
      }
      for (int i=0; i<ENC_MAX; i++)
      {
        if (osd_enc_cnt[i] != osd_enc_pre_cnt[i])
          osd_pre_time = millis();
      }
      if (millis()-osd_pre_time >= 5000)
      {
        osd_state = OSD_END;
      }
      break;

    case OSD_END:
      osd_state = OSD_IDLE;
      i2sCfgSave();
      saiCfgSave();
      break;      
  }

  if (osd_state == OSD_BUSY)
  {
    int16_t o_x;
    int16_t o_y;
    int16_t o_w = 200;
    int16_t o_h = 100;
    const char *mode_str[2] = {"I2S 볼륨", "SAI 볼륨"};

    o_x = (lcdGetWidth() - o_w) / 2;
    o_y = (lcdGetHeight() - o_h) / 2;



    if (osd_enc_cnt[ENC_R] != osd_enc_pre_cnt[ENC_R])
    {
      int16_t enc_diff;

      enc_diff = (osd_enc_cnt[ENC_R] - osd_enc_pre_cnt[ENC_R]) * 3;

      switch(osd_mode)
      {
        case 0:
          i2sSetVolume(i2sGetVolume() + enc_diff);
          break;
        case 1:
          saiSetVolume(saiGetVolume() + enc_diff);
          break;
      }
    }

    int16_t volume = 0;

    switch(osd_mode)
    {
      case 0:
        volume = i2sGetVolume();
        break;
      case 1:
        volume = saiGetVolume();
        break;
    }

    if (buttonEventGetPressed(&btn_event, _BTN_ENC_L))
    {
      osd_mode = (osd_mode + 1) % 2;
      osd_pre_time = millis();
    }


    lcdDrawFillRect(o_x-1, o_y-1, o_w+2, o_h+2, black);
    lcdDrawRect(o_x-1, o_y-1, o_w+2, o_h+2, orange);

    lcdDrawFillRect(o_x, o_y, o_w, 32, white);     
    lcdPrintfRect  (o_x, o_y, o_w, 32, black, 32, 
                    LCD_ALIGN_H_CENTER|LCD_ALIGN_V_CENTER, 
                    mode_str[osd_mode]);



    //lcdDrawRect    (o_x+1, o_y + o_h - 32 - 2, o_w - 2, 32, white);
    lcdDrawRect(o_x+1, o_y + o_h - 32 - 2, o_w - 2, 32, white);
    lcdDrawFillRect(o_x+1, o_y + o_h - 32 - 2, o_w * volume / 100 - 2, 32, orange);     

    lcdPrintfRect  (o_x+1, o_y + o_h - 32 - 2, o_w - 2, 32, white, 32, 
                    LCD_ALIGN_H_CENTER|LCD_ALIGN_V_CENTER, 
                    "%d", volume);   
  }

  for (int i=0; i<ENC_MAX; i++)
  {
    osd_enc_pre_cnt[i] = osd_enc_cnt[i];  
  }
}