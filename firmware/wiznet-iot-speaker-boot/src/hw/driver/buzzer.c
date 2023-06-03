#include "buzzer.h"
#include "cli.h"
#include "swtimer.h"


static bool is_init = false;
static bool is_on = false;
static uint16_t on_time_cnt = 0;
static TIM_HandleTypeDef htim13;


#ifdef _USE_HW_CLI
static void cliBuzzer(cli_args_t *args);
#endif


void buzzerISR(void *arg)
{
  if (is_on && on_time_cnt > 0)
  {
    on_time_cnt--;

    if (on_time_cnt == 0)
    {
      buzzerOff();
    }
  }
}




bool buzzerInit(void)
{
  bool ret = true;
  TIM_OC_InitTypeDef sConfigOC = {0};
  GPIO_InitTypeDef GPIO_InitStruct = {0};


  __HAL_RCC_TIM13_CLK_ENABLE();

  htim13.Instance = TIM13;
  htim13.Init.Prescaler = 274;
  htim13.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim13.Init.Period = 2000;
  htim13.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim13.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim13) != HAL_OK)
  {
    ret = false;
  }
  if (HAL_TIM_PWM_Init(&htim13) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_ENABLE;
  if (HAL_TIM_OC_ConfigChannel(&htim13, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    ret = false;
  }

  __HAL_RCC_GPIOF_CLK_ENABLE();
  /**TIM13 GPIO Configuration
  PF8     ------> TIM13_CH1
  */
  GPIO_InitStruct.Pin = GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF9_TIM13;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);


  HAL_TIM_PWM_Start(&htim13, TIM_CHANNEL_1);


  swtimer_handle_t timer_ch;
  timer_ch = swtimerGetHandle();
  swtimerSet(timer_ch, 1, LOOP_TIME, buzzerISR, NULL);
  swtimerStart(timer_ch);

  
#ifdef _USE_HW_CLI
  cliAdd("buzzer", cliBuzzer);
#endif  

  logPrintf("[%s] buzzerInit()\n", ret ? "OK":"NG");

  is_init = ret;
  return ret;
}

void buzzerOn(uint16_t freq_hz, uint16_t time_ms)
{
  uint32_t freq_cmp;

  freq_cmp = (1000000/freq_hz) - 1;

  htim13.Instance->ARR = freq_cmp;
  htim13.Instance->CCR1 = freq_cmp/2;
  is_on = true;
  on_time_cnt = time_ms;
}

void buzzerOff(void)
{
  htim13.Instance->CCR1 = 0;  
  is_on = false;
}

#ifdef _USE_HW_CLI
void cliBuzzer(cli_args_t *args)
{
  bool ret = false;


  if (args->argc == 3 && args->isStr(0, "on"))
  {
    uint16_t freq_hz;
    uint16_t time_ms;

    freq_hz = args->getData(1);
    time_ms = args->getData(2);

    buzzerOn(freq_hz, time_ms);
    ret = true;
  }
  
  if (args->argc == 1 && args->isStr(0, "off"))
  {
    buzzerOff();
    ret = true;
  }

  if (args->argc == 1 && args->isStr(0, "test"))
  {
    uint16_t freq_tbl[8] = {261, 293, 329, 349, 391, 440, 493, 523};
    uint8_t freq_i;

    for (int i=0; i<16; i++)
    {
      if (i/8 == 0)
      {
        freq_i = i%8;
      }
      else
      {
        freq_i = 7 - (i%8);
      }
      buzzerOn(freq_tbl[freq_i], 150);
      cliPrintf("%dHz, %dms\n", freq_tbl[freq_i], 100);
      delay(300);
    }
    ret = true;
  }

  if (args->argc == 1 && args->isStr(0, "melody"))
  {
    uint16_t melody[] = {NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4};
    int note_durations[] = { 4, 8, 8, 4, 4, 4, 4, 4 };

    for (int i=0; i<8; i++) 
    {
      int note_duration = 1000 / note_durations[i];

      buzzerOn(melody[i], note_duration);
      delay(note_duration * 1.30);    
    }
    ret = true;
  }
  if (ret != true)
  {
    cliPrintf("buzzer on freq(32~500000) time_ms\n");
    cliPrintf("buzzer off\n");
    cliPrintf("buzzer test\n");
    cliPrintf("buzzer melody\n");
  }
}
#endif