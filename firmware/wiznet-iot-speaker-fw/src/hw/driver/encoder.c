#include "encoder.h"
#include "swtimer.h"
#include "gpio.h"
#include "cli.h"

#ifdef _USE_HW_ENCODER



typedef struct 
{
  uint16_t gpio_a;
  uint16_t gpio_b;

  uint16_t tmp_cnt;
  uint16_t tmp_a;
  uint16_t tmp_b;
  uint16_t cur_a;
  uint16_t cur_b;
  uint16_t pre_a;
  uint16_t pre_b;
  int32_t  count;
} encoder_tbl_t;



#ifdef _USE_HW_CLI
static void cliEncoder(cli_args_t *args);
#endif
static void encoderISR(void *arg);


static encoder_tbl_t encoder_tbl[ENCODER_MAX_CH] = 
{
  {3, 4, 0, },
  {5, 6, 0, },
};

static bool is_init = false;





bool encoderInit(void)
{
  bool ret = false;

  for (int i=0; i<ENCODER_MAX_CH; i++)
  {
    encoder_tbl[i].count = 0;
  }

  swtimer_handle_t timer_ch;
  timer_ch = swtimerGetHandle();
  if (timer_ch >= 0)
  {
    swtimerSet(timer_ch, 1, LOOP_TIME, encoderISR, NULL);
    swtimerStart(timer_ch);
    ret = true;
  }

  is_init = ret;
  logPrintf("[%s] encoderInit()\n", ret ? "OK" : "NG");

#ifdef _USE_HW_CLI
  cliAdd("encoder", cliEncoder);
#endif  

  return true;
}

bool encoderReset(void)
{
  for (int i=0; i<ENCODER_MAX_CH; i++)
  {
    encoder_tbl[i].count = 0;
  }
  return true;
}

bool encoderClearCount(uint8_t ch)
{
  if (ch >= ENCODER_MAX_CH) return false;

  return true;
}

int32_t encoderGetCount(uint8_t ch)
{
  if (ch >= ENCODER_MAX_CH) return 0;

  return encoder_tbl[ch].count;
}

void encoderISR(void *arg)
{
  for (int i=0; i<ENCODER_MAX_CH; i++)
  {
    encoder_tbl_t *p_enc = &encoder_tbl[i];

    // ENC_A
    //
    p_enc->tmp_a = (p_enc->tmp_a << 1) | gpioPinRead(p_enc->gpio_a);

    if ((p_enc->tmp_a & 0x07) == 0x07) p_enc->cur_a = _DEF_HIGH;
    if ((p_enc->tmp_a & 0x07) == 0x00) p_enc->cur_a = _DEF_LOW;

    // ENC_B
    //
    p_enc->tmp_b = (p_enc->tmp_b << 1) | gpioPinRead(p_enc->gpio_b);

    if ((p_enc->tmp_b & 0x07) == 0x07) p_enc->cur_b = _DEF_HIGH;
    if ((p_enc->tmp_b & 0x07) == 0x00) p_enc->cur_b = _DEF_LOW;

    if (p_enc->tmp_cnt >= 8)
    {
      if (p_enc->pre_a == _DEF_LOW && p_enc->cur_a == _DEF_HIGH)
      {
        if (p_enc->cur_b == _DEF_LOW)
          p_enc->count++;
        else
          p_enc->count--;
      }
    }
    else
    {
      encoder_tbl[i].tmp_cnt++;
    }

    p_enc->pre_a = p_enc->cur_a;
    p_enc->pre_b = p_enc->cur_b;
  }
}

#ifdef _USE_HW_CLI
void cliEncoder(cli_args_t *args)
{
  bool ret = false;


  if (args->argc == 1 && args->isStr(0, "info"))
  {
    while(cliKeepLoop())
    {
      for (int i=0; i<ENCODER_MAX_CH; i++)
      {
        cliPrintf("%8d ", encoderGetCount(i));
      }
      cliPrintf("\n");
      delay(50);
    }
    ret = true;
  }
  
  if (ret != true)
  {
    cliPrintf("encoder info\n");
  }
}
#endif

#endif