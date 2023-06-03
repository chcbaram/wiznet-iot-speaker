#include "pwm.h"
#include "cli.h"

#ifdef _USE_HW_PWM



typedef struct
{
  uint16_t max_value;
  uint16_t duty;
  uint32_t channel;
} pwm_tbl_t;


#ifdef _USE_HW_CLI
static void cliPwm(cli_args_t *args);
#endif

static bool is_init = false;

static pwm_tbl_t  pwm_tbl[PWM_MAX_CH];
static TIM_HandleTypeDef htim1;





bool pwmInit(void)
{
  bool ret = true;
  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};


  for (int i=0; i<PWM_MAX_CH; i++)
  {
    pwm_tbl[i].duty = 0;
    pwm_tbl[i].max_value = 100;
  }

  __HAL_RCC_TIM1_CLK_ENABLE();

  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 274;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 99;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    return false;
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    return false;
  }
  if (HAL_TIM_PWM_Init(&htim1) != HAL_OK)
  {
    return false;
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    return false;
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    return false;
  }
  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.BreakFilter = 0;
  sBreakDeadTimeConfig.Break2State = TIM_BREAK2_DISABLE;
  sBreakDeadTimeConfig.Break2Polarity = TIM_BREAK2POLARITY_HIGH;
  sBreakDeadTimeConfig.Break2Filter = 0;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTimeConfig) != HAL_OK)
  {
    return false;
  }

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  __HAL_RCC_GPIOA_CLK_ENABLE();
  /**TIM1 GPIO Configuration
  PA8     ------> TIM1_CH1
  */
  GPIO_InitStruct.Pin = GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF1_TIM1;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);


  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);

  is_init = ret;

  logPrintf("[%s] pwmInit()\n", ret ? "OK" : "NG");

#ifdef _USE_HW_CLI
  cliAdd("pwm", cliPwm);
#endif
  return ret;
}

bool pwmIsInit(void)
{
  return is_init;
}

void pwmWrite(uint8_t ch, uint16_t pwm_data)
{
  if (ch >= PWM_MAX_CH) return;

  pwm_tbl[ch].duty = constrain(pwm_data, 0, pwm_tbl[ch].max_value);

  switch(ch)
  {
    case _DEF_PWM1:
      htim1.Instance->CCR1 = pwm_tbl[ch].duty;
      break;
  }
}

uint16_t pwmRead(uint8_t ch)
{
  if (ch >= HW_PWM_MAX_CH) return 0;

  return pwm_tbl[ch].duty;
}

uint16_t pwmGetMax(uint8_t ch)
{
  if (ch >= HW_PWM_MAX_CH) return 255;

  return pwm_tbl[ch].max_value;
}



#ifdef _USE_HW_CLI
void cliPwm(cli_args_t *args)
{
  bool ret = true;
  uint8_t  ch;
  uint32_t pwm;


  if (args->argc == 3)
  {
    ch  = (uint8_t)args->getData(1);
    pwm = (uint8_t)args->getData(2);

    ch = constrain(ch, 0, PWM_MAX_CH);

    if(args->isStr(0, "set"))
    {
      pwmWrite(ch, pwm);
      cliPrintf("pwm ch%d %d\n", ch, pwm);
    }
    else
    {
      ret = false;
    }
  }
  else if (args->argc == 2)
  {
    ch = (uint8_t)args->getData(1);

    if(args->isStr(0, "get"))
    {
      cliPrintf("pwm ch%d %d\n", ch, pwmRead(ch));
    }
    else
    {
      ret = false;
    }
  }
  else
  {
    ret = false;
  }


  if (ret == false)
  {
    cliPrintf( "pwm set 0~%d 0~255 \n", PWM_MAX_CH-1);
    cliPrintf( "pwm get 0~%d \n", PWM_MAX_CH-1);
  }

}
#endif

#endif