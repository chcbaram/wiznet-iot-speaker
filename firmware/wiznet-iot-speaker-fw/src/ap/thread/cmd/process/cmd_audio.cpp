#include "cmd_audio.h"


#define AUDIO_CMD_INFO              0x0020
#define AUDIO_CMD_BEGIN             0x0021
#define AUDIO_CMD_END               0x0022
#define AUDIO_CMD_READY             0x0023
#define AUDIO_CMD_WRITE             0x0024
#define AUDIO_CMD_WRITE_NO_RESP     0x0025


#define AUDIO_Q_BUF_LEN (4*1024)

#define FFT_LEN         512
#define BLOCK_X_CNT     12
#define BLOCK_Y_CNT     12

enum
{
  AUDIO_TYPE_I2S = 0,
  AUDIO_TYPE_SAI = 1,
  AUDIO_TYPE_MAX
};

typedef struct
{
  uint8_t  hw_type;
  char     file_name[128];
  uint32_t sample_rate;
  uint32_t file_size;
  uint8_t  file_type;
} audio_begin_t;

typedef struct
{
  uint8_t ch;

  bool (*write)(uint8_t ch, int16_t *p_data, uint32_t length);
  bool (*setSampleRate)(uint32_t freq);
  uint32_t (*getSampleRate)(void);
  uint32_t (*availableForWrite)(uint8_t ch);
} audio_type_t;

typedef struct
{
  uint32_t pre_time_lcd;
  uint8_t  update_cnt;
  uint16_t q15_buf_index;
  q15_t    buf_q15[FFT_LEN*2];

  uint8_t block_target[BLOCK_X_CNT];
  uint8_t block_peak[BLOCK_X_CNT];
  uint8_t block_value[BLOCK_X_CNT];
} fft_t;

static uint8_t audio_hw_type = AUDIO_TYPE_I2S;
static uint8_t is_begin = false;
static char     file_name[128];
static uint32_t file_size  = 0;
static uint32_t file_index = 0;
static fft_t    audio_fft;

static int16_t   audio_q_r_buf[AUDIO_Q_BUF_LEN];
static int16_t   audio_q_w_buf[AUDIO_Q_BUF_LEN];
static qbuffer_t audio_q;

static audio_type_t audio_type[AUDIO_TYPE_MAX];





bool cmdAudioInit(void)
{
  audio_type[AUDIO_TYPE_I2S].ch = i2sGetEmptyChannel();
  audio_type[AUDIO_TYPE_I2S].write = i2sWrite;
  audio_type[AUDIO_TYPE_I2S].setSampleRate = i2sSetSampleRate;
  audio_type[AUDIO_TYPE_I2S].getSampleRate = i2sGetSampleRate;
  audio_type[AUDIO_TYPE_I2S].availableForWrite = i2sAvailableForWrite;

  audio_type[AUDIO_TYPE_SAI].ch = saiGetEmptyChannel();
  audio_type[AUDIO_TYPE_SAI].write = saiWrite;
  audio_type[AUDIO_TYPE_SAI].setSampleRate = saiSetSampleRate;
  audio_type[AUDIO_TYPE_SAI].getSampleRate = saiGetSampleRate;
  audio_type[AUDIO_TYPE_SAI].availableForWrite = saiAvailableForWrite;

  qbufferCreateBySize(&audio_q, (uint8_t *)audio_q_w_buf, 2, AUDIO_Q_BUF_LEN);
  return true;
}

bool cmdAudioIsBusy(void)
{
  return is_begin;
}

static void cmdAudioBegin(cmd_t *p_cmd)
{
  uint16_t err_code = CMD_OK;

  audio_begin_t *p_begin = (audio_begin_t *)p_cmd->packet.data;

  is_begin   = true;
  audio_hw_type = p_begin->hw_type;
  strncpy(file_name, p_begin->file_name, 128);
  file_size = p_begin->file_size;
  file_index = 0;

  audio_fft.q15_buf_index = 0;
  audio_fft.update_cnt = 0;
  memset(audio_fft.block_peak, 0, sizeof(audio_fft.block_peak));
  memset(audio_fft.block_value, 0, sizeof(audio_fft.block_value));
  memset(audio_fft.block_target, 0, sizeof(audio_fft.block_target));
  logPrintf("[  ] AudioBegin()\n");
  logPrintf("       type : %s \n", audio_hw_type == 0 ? "I2S":"SAI");
  logPrintf("       rate : %d Khz\n", p_begin->sample_rate/1000);
  logPrintf("       name : %s \n", file_name);
  logPrintf("       size : %d \n", file_size);


  if (audio_type[audio_hw_type].setSampleRate(p_begin->sample_rate) != true)
  {
    err_code = ERR_CMD_AUDIO_RATE;
  }

  cmdSendResp(p_cmd, p_cmd->packet.cmd, err_code, NULL, 0);  
}

static void cmdAudioEnd(cmd_t *p_cmd)
{
  is_begin   = false;

  logPrintf("[  ] AudioEnd()\n");

  qbufferFlush(&audio_q);

  cmdSendResp(p_cmd, p_cmd->packet.cmd, CMD_OK, NULL, 0);  
}

static void cmdAudioReady(cmd_t *p_cmd)
{
  data_t data;

  data.u32D = (AUDIO_Q_BUF_LEN - qbufferAvailable(&audio_q)) - 1;
  data.u32D = data.u32D - (data.u32D%2);

  cmdSendResp(p_cmd, p_cmd->packet.cmd, CMD_OK, data.u8Data, 4);  
}

static void cmdAudioWrite(cmd_t *p_cmd, bool resp)
{
  int16_t *p_buf;
  uint32_t length;
  

  memcpy(&file_index, &p_cmd->packet.data[0], 4);

  p_buf  = (int16_t *)&p_cmd->packet.data[4];
  length = (p_cmd->packet.length - 4) / 2;

  qbufferWrite(&audio_q, (uint8_t *)p_buf, length);

  if (resp == true)
  {
    cmdSendResp(p_cmd, p_cmd->packet.cmd, CMD_OK, NULL, 0);  
  }
}


#define BLOCK_X_CNT     12
#define BLOCK_Y_CNT     12

static void drawBlock(int16_t bx, int16_t by, uint16_t color)
{
  int16_t x;
  int16_t y;
  int16_t bw;
  int16_t bh;
  int16_t top_space = 120;
  int16_t bottom_space = 16;
  int16_t sw;
  int16_t sh;


  sw = lcdGetWidth();
  sh = lcdGetHeight()-top_space-bottom_space;

  bw = (sw / BLOCK_X_CNT);
  bh = (sh / BLOCK_Y_CNT);

  x = bx*bw;
  y = sh - bh*by - bh;

  lcdDrawFillRect(x, y+top_space, bw-2, bh-2, color);
}

void drawFFT(fft_t *p_args)
{
  arm_cfft_q15(&arm_cfft_sR_q15_len512, p_args->buf_q15, 0, 1);

  int16_t xi;

  xi = 0;
  for (int i=0; i<BLOCK_X_CNT; i++)
  {
    int32_t h;
    int32_t max_h;


    max_h = 0;
    for (int j=0; j<FFT_LEN/2/BLOCK_X_CNT; j++)
    {
      h = p_args->buf_q15[2*xi + 1];
      h = constrain(h, 0, 500);
      h = cmap(h, 0, 500, 0, 80);
      if (h > max_h)
      {
        max_h = h;
      }
      xi++;
    }
    h = cmap(max_h, 0, 80, 0, BLOCK_Y_CNT-1);

    p_args->block_target[i] = h;

    if (p_args->update_cnt%2 == 0)
    {
      if (p_args->block_peak[i] > 0)
      {
        p_args->block_peak[i]--;
      }
    }
    if (h >= p_args->block_peak[i])
    {
      p_args->block_peak[i] = h;
      p_args->block_value[i] = h;
    }
  }

  p_args->update_cnt++;

  for (int i=0; i<BLOCK_X_CNT; i++)
  {
    drawBlock(i, p_args->block_peak[i], red);

    if (p_args->block_value[i] > p_args->block_target[i])
    {
      p_args->block_value[i]--;
    }
    for (int j=0; j<p_args->block_value[i]; j++)
    {
      drawBlock(i, j, yellow);
    }
  }
}

void cmdAudioUpdate(cmd_t *p_cmd)
{
  enum 
  {
    STATE_IDLE,
    STATE_BEGIN,
    STATE_END,
  };
  static uint8_t state = STATE_IDLE;


  switch(state)
  {
    case STATE_IDLE:
      if (is_begin == true)
      {
        state = STATE_BEGIN;
      }
      break;

    case STATE_BEGIN:
      if (is_begin == false)
      {
        state = STATE_END;
      }
      break;

    case STATE_END:
      state = STATE_IDLE;
      break;
  }


  if (state == STATE_BEGIN)
  {
    uint32_t q_len;
    uint32_t length;
    int16_t *p_buf = audio_q_r_buf;
    audio_type_t *p_type = &audio_type[audio_hw_type];

    q_len = qbufferAvailable(&audio_q);
    length = cmin(q_len, p_type->availableForWrite(p_type->ch));
    if (length > 0)
    {
      qbufferRead(&audio_q, (uint8_t *)p_buf, length);
      p_type->write(p_type->ch, (int16_t *)p_buf, length);

      for (uint32_t i=0; i<length; i+=2)
      {
        if (audio_fft.q15_buf_index < FFT_LEN)
        {
          audio_fft.buf_q15[audio_fft.q15_buf_index*2 + 0] = p_buf[i];
          audio_fft.buf_q15[audio_fft.q15_buf_index*2 + 1] = 0;      
          audio_fft.q15_buf_index++;            
        }    
        else
        {
          break;
        }
      }
    }
    
    if (lcdDrawAvailable() && audio_fft.q15_buf_index == FFT_LEN && millis() - audio_fft.pre_time_lcd >= 50)
    {
      audio_fft.pre_time_lcd = millis();

      lcdClearBuffer(black);

      lcdDrawFillRect(0, 0, LCD_WIDTH, 32, white);     
      lcdPrintfRect  (0, 0, LCD_WIDTH, 32, black, 32, LCD_ALIGN_H_CENTER|LCD_ALIGN_V_CENTER, 
        file_name);

      drawFFT(&audio_fft);
      audio_fft.q15_buf_index = 0;

      int percent;
      int percent_buf;
      uint32_t sample_rate;
      percent = file_index * 100 / file_size;

      percent_buf = qbufferAvailable(&audio_q) * 100 / (AUDIO_Q_BUF_LEN - 1);
      sample_rate = p_type->getSampleRate();

      lcdPrintfResize(0, 40+ 0, white, 32, "%s_%d_KHz", audio_hw_type == AUDIO_TYPE_I2S ? "I2S":"SAI", sample_rate/1000);
      lcdPrintfResize(0, 40+32, blue, 32, "%dKB %d%%", file_index/1024, percent_buf);
      
      lcdDrawRect    (0, LCD_HEIGHT-16, LCD_WIDTH, 16, white);
      lcdDrawFillRect(0, LCD_HEIGHT-16, LCD_WIDTH * percent / 100, 16, green);     

      lcdRequestDraw();
    }
  }
}

bool cmdAudioProcess(cmd_t *p_cmd)
{
  bool ret = true;


  switch(p_cmd->packet.cmd)
  {
    case AUDIO_CMD_BEGIN:
      cmdAudioBegin(p_cmd);
      break;

    case AUDIO_CMD_END:
      cmdAudioEnd(p_cmd);
      break;

    case AUDIO_CMD_READY:
      cmdAudioReady(p_cmd);
      break;

    case AUDIO_CMD_WRITE:
      cmdAudioWrite(p_cmd, true);
      break;

    case AUDIO_CMD_WRITE_NO_RESP:
      cmdAudioWrite(p_cmd, false);
      break;

    default:
      ret = false;
      break;  
  }

  return ret;
}

