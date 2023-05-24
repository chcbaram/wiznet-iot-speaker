#include "sai.h"
#include "cli.h"
#include "gpio.h"
#include "qbuffer.h"
#include "buzzer.h"
#include "files.h"
#include "mixer.h"


#ifdef _USE_HW_SAI


#define SAI_SAMPLERATE_HZ       16000
#define SAI_BUF_MS              (4)
#define SAI_BUF_LEN             (8*1024)
#define SAI_BUF_FRAME_LEN       ((48000 * 2 * SAI_BUF_MS) / 1000)  // 48Khz, Stereo, 4ms


#ifdef _USE_HW_CLI
static void cliSai(cli_args_t *args);
#endif

static bool is_init = false;
static bool is_started = false;

static uint32_t sai_sample_rate = SAI_SAMPLERATE_HZ;

static volatile uint32_t sai_frame_cur_i = 0;
static volatile bool     sai_frame_update = false;
static int16_t  sai_frame_buf[2][SAI_BUF_FRAME_LEN];
const int16_t   sai_frame_buf_zero[SAI_BUF_FRAME_LEN] = {0, };
static uint32_t sai_frame_len = 0;

static mixer_t   mixer;

static SAI_HandleTypeDef hsai_BlockA1;
static DMA_HandleTypeDef hdma_sai1_a;






bool saiInit(void)
{
  bool ret = true;


  hsai_BlockA1.Instance             = SAI1_Block_A;
  hsai_BlockA1.Init.AudioMode       = SAI_MODEMASTER_TX;
  hsai_BlockA1.Init.Synchro         = SAI_ASYNCHRONOUS;
  hsai_BlockA1.Init.OutputDrive     = SAI_OUTPUTDRIVE_DISABLE;
  hsai_BlockA1.Init.NoDivider       = SAI_MASTERDIVIDER_ENABLE;
  hsai_BlockA1.Init.FIFOThreshold   = SAI_FIFOTHRESHOLD_EMPTY;
  hsai_BlockA1.Init.AudioFrequency  = SAI_AUDIO_FREQUENCY_16K;
  hsai_BlockA1.Init.SynchroExt      = SAI_SYNCEXT_DISABLE;
  hsai_BlockA1.Init.MonoStereoMode  = SAI_STEREOMODE;
  hsai_BlockA1.Init.CompandingMode  = SAI_NOCOMPANDING;
  hsai_BlockA1.Init.TriState        = SAI_OUTPUT_NOTRELEASED;
  if (HAL_SAI_InitProtocol(&hsai_BlockA1, SAI_I2S_STANDARD, SAI_PROTOCOL_DATASIZE_16BIT, 2) != HAL_OK)
  {
    ret = false;
  }

  sai_frame_len = (sai_sample_rate * 2 * SAI_BUF_MS) / 1000;
  
  mixerInit(&mixer);

  saiStart();

  delay(50);

  is_init = ret;

  logPrintf("[%s] saiInit()\n", ret ? "OK" : "NG");

#ifdef _USE_HW_CLI
  cliAdd("sai", cliSai);
#endif

  return ret;
}

bool saiSetSampleRate(uint32_t freq)
{
  bool ret = true;
  uint32_t frame_len;


  saiStop();
  delay(10);

  sai_sample_rate = freq;
  frame_len = (sai_sample_rate * 2 * SAI_BUF_MS) / 1000;
  sai_frame_len = frame_len;


  hsai_BlockA1.Init.AudioFrequency  = freq;
  if (HAL_SAI_Init(&hsai_BlockA1) != HAL_OK)
  {
    ret = false;
  }
  saiStart();

  return ret;
}

bool saiStart(void)
{
  bool ret = false;
  HAL_StatusTypeDef status;


  status = HAL_SAI_Transmit_DMA(&hsai_BlockA1, (uint8_t *)sai_frame_buf_zero, sai_frame_len);
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

bool saiStop(void)
{
  is_started = false;
  return true;
}

int8_t saiGetEmptyChannel(void)
{
  return mixerGetEmptyChannel(&mixer);
}

uint32_t saiGetFrameSize(void)
{
  return sai_frame_len;
}

uint32_t saiAvailableForWrite(uint8_t ch)
{
  return mixerAvailableForWrite(&mixer, ch);
}

bool saiWrite(uint8_t ch, int16_t *p_data, uint32_t length)
{
  return mixerWrite(&mixer, ch, p_data, length);
}

// https://m.blog.naver.com/PostView.nhn?blogId=hojoon108&logNo=80145019745&proxyReferer=https:%2F%2Fwww.google.com%2F
//
float saiGetNoteHz(int8_t octave, int8_t note)
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
float saiSin(float x)
{
  const float B = 4 / M_PI;
  const float C = -4 / (M_PI * M_PI);

  return -(B * x + C * x * ((x < 0) ? -x : x));
}

bool saiPlayBeep(uint32_t freq_hz, uint16_t volume, uint32_t time_ms)
{
  uint32_t pre_time;
  int32_t sample_rate = sai_sample_rate;
  int32_t num_samples = sai_frame_len;
  float sample_point;
  int16_t sample[num_samples];
  int16_t sample_index = 0;
  float div_freq;
  int8_t mix_ch;
  int32_t volume_out;


  volume = constrain(volume, 0, 100);
  volume_out = (INT16_MAX/40) * volume / 100;

  mix_ch =  saiGetEmptyChannel();

  div_freq = (float)sample_rate/(float)freq_hz;

  pre_time = millis();
  while(millis()-pre_time <= time_ms)
  {
    if (saiAvailableForWrite(mix_ch) >= num_samples)
    {
      for (int i=0; i<num_samples; i+=2)
      {
        sample_point = saiSin(2.0f * M_PI * (float)(sample_index) / ((float)div_freq));
        sample[i + 0] = (int16_t)(sample_point * volume_out);
        sample[i + 1] = (int16_t)(sample_point * volume_out);
        sample_index = (sample_index + 1) % (int)(div_freq);
      }
      saiWrite(mix_ch, (int16_t *)sample, num_samples);
    }
    delay(2);
  }

  return true;
}


void HAL_SAI_TxCpltCallback(SAI_HandleTypeDef *hi2s)
{
  if (is_started != true)
  {
    return;
  }

  if (sai_frame_update)
  {
    HAL_SAI_Transmit_DMA(hi2s, (uint8_t *)sai_frame_buf[sai_frame_cur_i], sai_frame_len);
    sai_frame_cur_i ^= 1;
    sai_frame_update = false;
  }
  else
  {
    HAL_SAI_Transmit_DMA(hi2s, (uint8_t *)sai_frame_buf_zero, sai_frame_len);
  }
}

void HAL_SAI_TxHalfCpltCallback(SAI_HandleTypeDef *hi2s)
{
  if (mixerAvailable(&mixer) >= sai_frame_len)
  {    
    mixerRead(&mixer, sai_frame_buf[sai_frame_cur_i], sai_frame_len);
    sai_frame_update = true;
  }
}

void HAL_SAI_ErrorCallback(SAI_HandleTypeDef *hi2s)
{

}

void SAI1_IRQHandler(void)
{
  HAL_SAI_IRQHandler(&hsai_BlockA1);
}

void DMA1_Stream4_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&hdma_sai1_a);
}

static uint32_t SAI1_client =0;

void HAL_SAI_MspInit(SAI_HandleTypeDef* saiHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
  /* SAI1 */
  if(saiHandle->Instance==SAI1_Block_A)
  {
    /* SAI1 clock enable */

  /** Initializes the peripherals clock
  */

    switch(saiHandle->Init.AudioFrequency)
    {
      case SAI_AUDIO_FREQUENCY_48K:
      case SAI_AUDIO_FREQUENCY_8K:
        PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_SAI1;
        PeriphClkInitStruct.PLL3.PLL3M = 10;
        PeriphClkInitStruct.PLL3.PLL3N = 162;
        PeriphClkInitStruct.PLL3.PLL3P = 33;
        PeriphClkInitStruct.PLL3.PLL3Q = 2;
        PeriphClkInitStruct.PLL3.PLL3R = 2;
        PeriphClkInitStruct.PLL3.PLL3RGE = RCC_PLL3VCIRANGE_1;
        PeriphClkInitStruct.PLL3.PLL3VCOSEL = RCC_PLL3VCOWIDE;
        PeriphClkInitStruct.PLL3.PLL3FRACN = 0;
        PeriphClkInitStruct.Sai1ClockSelection = RCC_SAI1CLKSOURCE_PLL3;      
        break;

      case SAI_AUDIO_FREQUENCY_44K:
      case SAI_AUDIO_FREQUENCY_22K:
      case SAI_AUDIO_FREQUENCY_11K:
        PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_SAI1;
        PeriphClkInitStruct.PLL3.PLL3M = 10;
        PeriphClkInitStruct.PLL3.PLL3N = 149;
        PeriphClkInitStruct.PLL3.PLL3P = 33;
        PeriphClkInitStruct.PLL3.PLL3Q = 2;
        PeriphClkInitStruct.PLL3.PLL3R = 2;
        PeriphClkInitStruct.PLL3.PLL3RGE = RCC_PLL3VCIRANGE_1;
        PeriphClkInitStruct.PLL3.PLL3VCOSEL = RCC_PLL3VCOWIDE;
        PeriphClkInitStruct.PLL3.PLL3FRACN = 0;
        PeriphClkInitStruct.Sai1ClockSelection = RCC_SAI1CLKSOURCE_PLL3;      
        break;

      default:
        // For 16Khz, 32Khz
        //
        PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_SAI1;
        PeriphClkInitStruct.PLL3.PLL3M = 10;
        PeriphClkInitStruct.PLL3.PLL3N = 210;
        PeriphClkInitStruct.PLL3.PLL3P = 32;
        PeriphClkInitStruct.PLL3.PLL3Q = 2;
        PeriphClkInitStruct.PLL3.PLL3R = 2;
        PeriphClkInitStruct.PLL3.PLL3RGE = RCC_PLL3VCIRANGE_1;
        PeriphClkInitStruct.PLL3.PLL3VCOSEL = RCC_PLL3VCOWIDE;
        PeriphClkInitStruct.PLL3.PLL3FRACN = 0;
        PeriphClkInitStruct.Sai1ClockSelection = RCC_SAI1CLKSOURCE_PLL3;
        break;
    }

    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
      Error_Handler();
    }

    if (SAI1_client == 0)
    {
       __HAL_RCC_SAI1_CLK_ENABLE();

      /* Peripheral interrupt init*/
      HAL_NVIC_SetPriority(SAI1_IRQn, 5, 0);
      HAL_NVIC_EnableIRQ(SAI1_IRQn);
    }
    SAI1_client ++;

    /**SAI1_A_Block_A GPIO Configuration
    PE2     ------> SAI1_MCLK_A
    PE4     ------> SAI1_FS_A
    PE5     ------> SAI1_SCK_A
    PC1     ------> SAI1_SD_A
    */
    GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_4|GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF6_SAI1;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF6_SAI1;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    /* Peripheral DMA init*/

    hdma_sai1_a.Instance = DMA1_Stream4;
    hdma_sai1_a.Init.Request = DMA_REQUEST_SAI1_A;
    hdma_sai1_a.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_sai1_a.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_sai1_a.Init.MemInc = DMA_MINC_ENABLE;
    hdma_sai1_a.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    hdma_sai1_a.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    hdma_sai1_a.Init.Mode = DMA_NORMAL;
    hdma_sai1_a.Init.Priority = DMA_PRIORITY_LOW;
    hdma_sai1_a.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_sai1_a) != HAL_OK)
    {
      Error_Handler();
    }

    /* Several peripheral DMA handle pointers point to the same DMA handle.
     Be aware that there is only one channel to perform all the requested DMAs. */
    __HAL_LINKDMA(saiHandle,hdmarx,hdma_sai1_a);
    __HAL_LINKDMA(saiHandle,hdmatx,hdma_sai1_a);


    /* DMA1_Stream4_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(DMA1_Stream4_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(DMA1_Stream4_IRQn);    
  }
}


void HAL_SAI_MspDeInit(SAI_HandleTypeDef* saiHandle)
{

  /* SAI1 */
  if(saiHandle->Instance==SAI1_Block_A)
  {
    SAI1_client --;
    if (SAI1_client == 0)
    {
      /* Peripheral clock disable */
      __HAL_RCC_SAI1_CLK_DISABLE();
      HAL_NVIC_DisableIRQ(SAI1_IRQn);
    }

    /**SAI1_A_Block_A GPIO Configuration
    PE2     ------> SAI1_MCLK_A
    PE4     ------> SAI1_FS_A
    PE5     ------> SAI1_SCK_A
    PC1     ------> SAI1_SD_A
    */
    HAL_GPIO_DeInit(GPIOE, GPIO_PIN_2|GPIO_PIN_4|GPIO_PIN_5);

    HAL_GPIO_DeInit(GPIOC, GPIO_PIN_1);

    HAL_DMA_DeInit(saiHandle->hdmarx);
    HAL_DMA_DeInit(saiHandle->hdmatx);
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


void cliSai(cli_args_t *args)
{
  bool ret = false;



  if (args->argc == 1 && args->isStr(0, "info") == true)
  {

    cliPrintf("sai init      : %d\n", is_init);
    cliPrintf("sai rate      : %d Khz\n", sai_sample_rate/1000);
    cliPrintf("sai buf ms    : %d ms\n", SAI_BUF_MS);
    cliPrintf("sai frame len : %d \n", sai_frame_len);
    ret = true;
  }

  if (args->argc == 3 && args->isStr(0, "beep") == true)
  {
    uint32_t freq;
    uint32_t time_ms;

    freq = args->getData(1);
    time_ms = args->getData(2);
    
    saiPlayBeep(freq, 100, time_ms);

    ret = true;
  }

  if (args->argc == 1 && args->isStr(0, "melody"))
  {
    uint16_t melody[] = {NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4};
    int note_durations[] = { 4, 8, 8, 4, 4, 4, 4, 4 };

    for (int i=0; i<8; i++) 
    {
      int note_duration = 1000 / note_durations[i];

      saiPlayBeep(melody[i], 100, note_duration);
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


    saiSetSampleRate(header.SampleRate);

    r_len = sai_frame_len/2;

    int16_t buf_frame[sai_frame_len];

    fseek(fp, sizeof(wavfile_header_t) + 1024, SEEK_SET);

    ch = saiGetEmptyChannel();

    while(cliKeepLoop())
    {
      int len;


      if (saiAvailableForWrite(ch) >= sai_frame_len)
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

          saiWrite(ch, (int16_t *)buf_data, 2);
        }
      }
    }

    fclose(fp);

    ret = true;
  }

  if (ret != true)
  {
    cliPrintf("sai info\n");
    cliPrintf("sai melody\n");
    cliPrintf("sai beep freq time_ms\n");
    cliPrintf("sai play-wav filename\n");
  }
}
#endif

#endif
