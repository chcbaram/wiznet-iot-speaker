/*
 * i2s.c
 *
 *  Created on: 2021. 1. 9.
 *      Author: baram
 */


#include "i2s.h"
#include "cli.h"
#include "gpio.h"
#include "qbuffer.h"
#include "buzzer.h"
#include "files.h"


#ifdef _USE_HW_I2S


#define I2S_SAMPLERATE_HZ       16000
#define I2S_BUF_MS              (4)
#define I2S_BUF_LEN             (8*1024)
#define I2S_BUF_FRAME_LEN       ((48000 * 2 * I2S_BUF_MS) / 1000)  // 48Khz, Stereo, 4ms


#ifdef _USE_HW_CLI
static void cliI2s(cli_args_t *args);
#endif

static bool is_init = false;
static bool is_started = false;

static uint32_t i2s_sample_rate = I2S_SAMPLERATE_HZ;

static volatile uint32_t i2s_frame_cur_i = 0;
static volatile bool     i2s_frame_update = false;
static int16_t  i2s_frame_buf[2][I2S_BUF_FRAME_LEN];
const int16_t   i2s_frame_buf_zero[I2S_BUF_FRAME_LEN] = {0, };
static uint32_t i2s_frame_len = 0;

static qbuffer_t q_tx;
static int16_t   q_buf[I2S_BUF_LEN];

static I2S_HandleTypeDef hi2s1;
static DMA_HandleTypeDef hdma_spi1_tx;






bool i2sInit(void)
{
  bool ret = true;


  hi2s1.Instance = SPI1;
  hi2s1.Init.Mode = I2S_MODE_MASTER_TX;
  hi2s1.Init.Standard = I2S_STANDARD_PHILIPS;
  hi2s1.Init.DataFormat = I2S_DATAFORMAT_16B;
  hi2s1.Init.MCLKOutput = I2S_MCLKOUTPUT_DISABLE;
  hi2s1.Init.AudioFreq = I2S_AUDIOFREQ_16K;
  hi2s1.Init.CPOL = I2S_CPOL_LOW;
  hi2s1.Init.FirstBit = I2S_FIRSTBIT_MSB;
  hi2s1.Init.WSInversion = I2S_WS_INVERSION_DISABLE;
  hi2s1.Init.Data24BitAlignment = I2S_DATA_24BIT_ALIGNMENT_RIGHT;
  hi2s1.Init.MasterKeepIOState = I2S_MASTER_KEEP_IO_STATE_DISABLE;
  if (HAL_I2S_Init(&hi2s1) != HAL_OK)
  {
    ret = false;
  }

  i2s_frame_len = (i2s_sample_rate * 2 * I2S_BUF_MS) / 1000;

  qbufferCreateBySize(&q_tx, (uint8_t *)q_buf, 2, I2S_BUF_LEN);

  i2sStart();

  delay(50);
  gpioPinWrite(_PIN_GPIO_SPK_EN, _DEF_HIGH);

  is_init = ret;

  logPrintf("[%s] i2sInit()\n", ret ? "OK" : "NG");

#ifdef _USE_HW_CLI
  cliAdd("i2s", cliI2s);
#endif

  return ret;
}

bool i2sSetSampleRate(uint32_t freq)
{
  bool ret = true;
  uint32_t frame_len;


  i2sStop();
  delay(10);

  i2s_sample_rate = freq;
  frame_len = (i2s_sample_rate * 2 * I2S_BUF_MS) / 1000;
  i2s_frame_len = frame_len;


  gpioPinWrite(_PIN_GPIO_SPK_EN, _DEF_LOW);

  hi2s1.Init.AudioFreq = freq;
  if (HAL_I2S_Init(&hi2s1) != HAL_OK)
  {
    ret = false;
  }
  i2sStart();

  gpioPinWrite(_PIN_GPIO_SPK_EN, _DEF_HIGH);

  return ret;
}

bool i2sStart(void)
{
  bool ret = false;
  HAL_StatusTypeDef status;
  I2S_HandleTypeDef *p_i2s = &hi2s1;


  status = HAL_I2S_Transmit_DMA(p_i2s, (uint16_t *)i2s_frame_buf_zero, i2s_frame_len);
  if (status == HAL_OK)
  {
    is_started = true;
  }
  else
  {
    is_started = false;
  }

  return ret;
}

bool i2sStop(void)
{
  is_started = false;
  return true;
}

int8_t i2sGetEmptyChannel(void)
{
  return 0;
}

uint32_t i2sAvailableForWrite(uint8_t ch)
{
  uint32_t rx_len;
  uint32_t wr_len;

  rx_len = qbufferAvailable(&q_tx);
  wr_len = (q_tx.len - 1) - rx_len;  

  return wr_len;
}

bool i2sWrite(uint8_t ch, int16_t *p_data, uint32_t length)
{
  return qbufferWrite(&q_tx, (uint8_t *)p_data, length);
}

// https://m.blog.naver.com/PostView.nhn?blogId=hojoon108&logNo=80145019745&proxyReferer=https:%2F%2Fwww.google.com%2F
//
float i2sGetNoteHz(int8_t octave, int8_t note)
{
  float hz;
  float f_note;

  if (octave < 1) octave = 1;
  if (octave > 8) octave = 8;

  if (note <  1) note = 1;
  if (note > 12) note = 12;

  f_note = (float)(note-10)/12.0f;

  hz = pow(2, (octave-1)) * 55 * pow(2, f_note);

  return hz;
}

// https://gamedev.stackexchange.com/questions/4779/is-there-a-faster-sine-function
//
float i2sSin(float x)
{
  const float B = 4 / M_PI;
  const float C = -4 / (M_PI * M_PI);

  return -(B * x + C * x * ((x < 0) ? -x : x));
}

bool i2sPlayBeep(uint32_t freq_hz, uint16_t volume, uint32_t time_ms)
{
  uint32_t pre_time;
  int32_t sample_rate = i2s_sample_rate;
  int32_t num_samples = i2s_frame_len;
  float sample_point;
  int16_t sample[num_samples];
  int16_t sample_index = 0;
  float div_freq;
  int8_t mix_ch;
  int32_t volume_out;


  volume = constrain(volume, 0, 100);
  volume_out = (INT16_MAX/40) * volume / 100;

  mix_ch =  i2sGetEmptyChannel();

  div_freq = (float)sample_rate/(float)freq_hz;

  pre_time = millis();
  while(millis()-pre_time <= time_ms)
  {
    if (i2sAvailableForWrite(mix_ch) >= num_samples)
    {
      for (int i=0; i<num_samples; i+=2)
      {
        sample_point = i2sSin(2.0f * M_PI * (float)(sample_index) / ((float)div_freq));
        sample[i + 0] = (int16_t)(sample_point * volume_out);
        sample[i + 1] = (int16_t)(sample_point * volume_out);
        sample_index = (sample_index + 1) % (int)(div_freq);
      }
      i2sWrite(mix_ch, (int16_t *)sample, num_samples);
    }
    delay(2);
  }

  return true;
}


void HAL_I2S_TxCpltCallback(I2S_HandleTypeDef *hi2s)
{
  if (is_started != true)
  {
    return;
  }

  if (i2s_frame_update)
  {
    HAL_I2S_Transmit_DMA(hi2s, (uint16_t *)i2s_frame_buf[i2s_frame_cur_i], i2s_frame_len);
    i2s_frame_cur_i ^= 1;
    i2s_frame_update = false;
  }
  else
  {
    HAL_I2S_Transmit_DMA(hi2s, (uint16_t *)i2s_frame_buf_zero, i2s_frame_len);
  }
}

void HAL_I2S_TxHalfCpltCallback(I2S_HandleTypeDef *hi2s)
{
  if (qbufferAvailable(&q_tx) >= i2s_frame_len)
  {    
    qbufferRead(&q_tx, (uint8_t *)i2s_frame_buf[i2s_frame_cur_i], i2s_frame_len);
    i2s_frame_update = true;
  }
}

void HAL_I2S_ErrorCallback(I2S_HandleTypeDef *hi2s)
{

}

void SPI1_IRQHandler(void)
{
  HAL_I2S_IRQHandler(&hi2s1);
}

void DMA1_Stream3_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&hdma_spi1_tx);
}

void HAL_I2S_MspInit(I2S_HandleTypeDef* i2sHandle)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
  if(i2sHandle->Instance==SPI1)
  {

  /** Initializes the peripherals clock
  */
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_SPI1;
    PeriphClkInitStruct.Spi123ClockSelection = RCC_SPI123CLKSOURCE_PLL;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
      Error_Handler();
    }

    /* I2S1 clock enable */
    __HAL_RCC_SPI1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOG_CLK_ENABLE();
    /**I2S1 GPIO Configuration
    PA7     ------> I2S1_SDO
    PA15(JTDI)     ------> I2S1_WS
    PG11     ------> I2S1_CK
    */
    GPIO_InitStruct.Pin = GPIO_PIN_7|GPIO_PIN_15;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_11;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
    HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

    /* I2S1 DMA Init */
    /* SPI1_TX Init */
    hdma_spi1_tx.Instance = DMA1_Stream3;
    hdma_spi1_tx.Init.Request = DMA_REQUEST_SPI1_TX;
    hdma_spi1_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_spi1_tx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_spi1_tx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_spi1_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    hdma_spi1_tx.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    hdma_spi1_tx.Init.Mode = DMA_NORMAL;
    hdma_spi1_tx.Init.Priority = DMA_PRIORITY_LOW;
    hdma_spi1_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_spi1_tx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(i2sHandle,hdmatx,hdma_spi1_tx);

    /* I2S1 interrupt Init */
    HAL_NVIC_SetPriority(SPI1_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(SPI1_IRQn);

    /* DMA1_Stream3_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(DMA1_Stream3_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(DMA1_Stream3_IRQn);    
  }
}

void HAL_I2S_MspDeInit(I2S_HandleTypeDef* i2sHandle)
{

  if(i2sHandle->Instance==SPI1)
  {
    /* Peripheral clock disable */
    __HAL_RCC_SPI1_CLK_DISABLE();

    /**I2S1 GPIO Configuration
    PA7     ------> I2S1_SDO
    PA15(JTDI)     ------> I2S1_WS
    PG11     ------> I2S1_CK
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_7|GPIO_PIN_15);

    HAL_GPIO_DeInit(GPIOG, GPIO_PIN_11);

    /* I2S1 DMA DeInit */
    HAL_DMA_DeInit(i2sHandle->hdmatx);

    /* I2S1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(SPI1_IRQn);
  }
}



#ifdef _USE_HW_CLI


typedef struct wavfile_header_s
{
  char    ChunkID[4];     /*  4   */
  int32_t ChunkSize;      /*  4   */
  char    Format[4];      /*  4   */

  char    Subchunk1ID[4]; /*  4   */
  int32_t Subchunk1Size;  /*  4   */
  int16_t AudioFormat;    /*  2   */
  int16_t NumChannels;    /*  2   */
  int32_t SampleRate;     /*  4   */
  int32_t ByteRate;       /*  4   */
  int16_t BlockAlign;     /*  2   */
  int16_t BitsPerSample;  /*  2   */

  char    Subchunk2ID[4];
  int32_t Subchunk2Size;
} wavfile_header_t;


void cliI2s(cli_args_t *args)
{
  bool ret = false;



  if (args->argc == 1 && args->isStr(0, "info") == true)
  {

    cliPrintf("i2s init      : %d\n", is_init);
    cliPrintf("i2s rate      : %d Khz\n", i2s_sample_rate/1000);
    cliPrintf("i2s buf ms    : %d ms\n", I2S_BUF_MS);
    cliPrintf("i2s frame len : %d \n", i2s_frame_len);
    ret = true;
  }

  if (args->argc == 3 && args->isStr(0, "beep") == true)
  {
    uint32_t freq;
    uint32_t time_ms;

    freq = args->getData(1);
    time_ms = args->getData(2);
    
    i2sPlayBeep(freq, 100, time_ms);

    ret = true;
  }

  if (args->argc == 1 && args->isStr(0, "melody"))
  {
    uint16_t melody[] = {NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4};
    int note_durations[] = { 4, 8, 8, 4, 4, 4, 4, 4 };

    for (int i=0; i<8; i++) 
    {
      int note_duration = 1000 / note_durations[i];

      i2sPlayBeep(melody[i], 100, note_duration);
      delay(note_duration * 0.3);    
    }
    ret = true;
  }

  if (args->argc == 2 && args->isStr(0, "play-wav") == true)
  {
    char *file_name;
    FILE *fp;
    wavfile_header_t header;
    uint32_t r_len;
    int32_t  volume = 100;
    int8_t ch;


    file_name = args->getStr(1);

    cliPrintf("FileName      : %s\n", file_name);


    fp = fopen(file_name, "r");
    if (fp == NULL)
    {
      cliPrintf("fopen fail : %s\n", file_name);
      return;
    }
    fread(&header, sizeof(wavfile_header_t), 1, fp);

    cliPrintf("ChunkSize     : %d\n", header.ChunkSize);
    cliPrintf("Format        : %c%c%c%c\n", header.Format[0], header.Format[1], header.Format[2], header.Format[3]);
    cliPrintf("Subchunk1Size : %d\n", header.Subchunk1Size);
    cliPrintf("AudioFormat   : %d\n", header.AudioFormat);
    cliPrintf("NumChannels   : %d\n", header.NumChannels);
    cliPrintf("SampleRate    : %d\n", header.SampleRate);
    cliPrintf("ByteRate      : %d\n", header.ByteRate);
    cliPrintf("BlockAlign    : %d\n", header.BlockAlign);
    cliPrintf("BitsPerSample : %d\n", header.BitsPerSample);
    cliPrintf("Subchunk2Size : %d\n", header.Subchunk2Size);


    i2sSetSampleRate(header.SampleRate);

    r_len = i2s_frame_len/2;

    int16_t buf_frame[i2s_frame_len];

    fseek(fp, sizeof(wavfile_header_t) + 1024, SEEK_SET);

    ch = i2sGetEmptyChannel();

    while(cliKeepLoop())
    {
      int len;


      if (i2sAvailableForWrite(ch) >= i2s_frame_len)
      {
        len = fread(buf_frame, r_len, 2*header.NumChannels, fp);

        if (len != r_len*2*header.NumChannels)
        {
          break;
        }

        int16_t buf_data[2];

        for (int i=0; i<r_len; i++)
        {
          if (header.NumChannels == 2)
          {
            buf_data[0] = buf_frame[i*2 + 0] * volume / 100;;
            buf_data[1] = buf_frame[i*2 + 1] * volume / 100;;
          }
          else
          {
            buf_data[0] = buf_frame[i] * volume / 100;;
            buf_data[1] = buf_frame[i] * volume / 100;;
          }

          i2sWrite(ch, (int16_t *)buf_data, 2);
        }
      }
    }

    fclose(fp);

    ret = true;
  }

  if (ret != true)
  {
    cliPrintf("i2s info\n");
    cliPrintf("i2s melody\n");
    cliPrintf("i2s beep freq time_ms\n");
    cliPrintf("i2s play-wav filename\n");
  }
}
#endif

#endif