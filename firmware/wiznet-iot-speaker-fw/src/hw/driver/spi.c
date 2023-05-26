/*
 * spi.c
 *
 *  Created on: 2020. 12. 27.
 *      Author: baram
 */


#include "spi.h"
#include "stm32h7xx_ll_spi.h"


#ifdef _USE_HW_SPI

#define SPI_TX_DMA_MAX_LENGTH   0xFFFF


typedef struct
{
  bool tx_done;
  uint8_t *p_tx_buf;
  uint8_t *p_tx_buf_next;
  uint32_t tx_length_next;
} spi_dma_buf_t;


typedef struct
{
  bool is_open;
  bool is_tx_done;
  bool is_error;

  void (*func_tx)(void);

  SPI_HandleTypeDef *h_spi;
  DMA_HandleTypeDef *h_dma_tx;
  DMA_HandleTypeDef *h_dma_rx;

  spi_dma_buf_t      dma_tx_buf;
} spi_t;



spi_t spi_tbl[SPI_MAX_CH];



SPI_HandleTypeDef hspi4;
DMA_HandleTypeDef hdma_spi4_tx;


bool spiInit(void)
{
  bool ret = true;


  for (int i=0; i<SPI_MAX_CH; i++)
  {
    spi_tbl[i].is_open = false;
    spi_tbl[i].is_tx_done = true;
    spi_tbl[i].is_error = false;
    spi_tbl[i].func_tx = NULL;
    spi_tbl[i].h_dma_rx = NULL;
    spi_tbl[i].h_dma_tx = NULL;

    spi_tbl[i].dma_tx_buf.p_tx_buf = NULL;
    spi_tbl[i].dma_tx_buf.p_tx_buf_next = NULL;
    spi_tbl[i].dma_tx_buf.tx_done = false;
    spi_tbl[i].dma_tx_buf.tx_length_next = 0;    
  }

  return ret;
}

bool spiBegin(uint8_t ch)
{
  bool ret = false;
  spi_t *p_spi = &spi_tbl[ch];

  switch(ch)
  {
    case _DEF_SPI1:
      p_spi->h_spi = &hspi4;
      p_spi->h_dma_tx = &hdma_spi4_tx;

      p_spi->h_spi->Instance              = SPI4;
      p_spi->h_spi->Init.Mode             = SPI_MODE_MASTER;
      p_spi->h_spi->Init.Direction        = SPI_DIRECTION_2LINES_TXONLY;
      p_spi->h_spi->Init.DataSize         = SPI_DATASIZE_8BIT;
      p_spi->h_spi->Init.CLKPolarity      = SPI_POLARITY_LOW;
      p_spi->h_spi->Init.CLKPhase         = SPI_PHASE_1EDGE;
      p_spi->h_spi->Init.NSS              = SPI_NSS_SOFT;
      p_spi->h_spi->Init.BaudRatePrescaler= SPI_BAUDRATEPRESCALER_4;
      p_spi->h_spi->Init.FirstBit         = SPI_FIRSTBIT_MSB;
      p_spi->h_spi->Init.TIMode           = SPI_TIMODE_DISABLE;
      p_spi->h_spi->Init.CRCCalculation   = SPI_CRCCALCULATION_DISABLE;
      p_spi->h_spi->Init.CRCPolynomial    = 0;

      p_spi->h_spi->Init.NSSPMode                   = SPI_NSS_PULSE_DISABLE;
      p_spi->h_spi->Init.NSSPolarity                = SPI_NSS_POLARITY_LOW;
      p_spi->h_spi->Init.FifoThreshold              = SPI_FIFO_THRESHOLD_01DATA;
      p_spi->h_spi->Init.TxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
      p_spi->h_spi->Init.RxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
      p_spi->h_spi->Init.MasterSSIdleness           = SPI_MASTER_SS_IDLENESS_01CYCLE;
      p_spi->h_spi->Init.MasterInterDataIdleness    = SPI_MASTER_INTERDATA_IDLENESS_01CYCLE;
      p_spi->h_spi->Init.MasterReceiverAutoSusp     = SPI_MASTER_RX_AUTOSUSP_DISABLE;
      p_spi->h_spi->Init.MasterKeepIOState          = SPI_MASTER_KEEP_IO_STATE_DISABLE;
      p_spi->h_spi->Init.IOSwap                     = SPI_IO_SWAP_DISABLE;


      HAL_SPI_DeInit(p_spi->h_spi);
      if (HAL_SPI_Init(p_spi->h_spi) == HAL_OK)
      {
        p_spi->is_open = true;
        ret = true;
      }
      break;
  }

  return ret;
}

void spiSetDataMode(uint8_t ch, uint8_t dataMode)
{
  spi_t  *p_spi = &spi_tbl[ch];


  if (p_spi->is_open == false) return;


  switch( dataMode )
  {
    // CPOL=0, CPHA=0
    case SPI_MODE0:
      p_spi->h_spi->Init.CLKPolarity = SPI_POLARITY_LOW;
      p_spi->h_spi->Init.CLKPhase    = SPI_PHASE_1EDGE;
      HAL_SPI_Init(p_spi->h_spi);
      break;

    // CPOL=0, CPHA=1
    case SPI_MODE1:
      p_spi->h_spi->Init.CLKPolarity = SPI_POLARITY_LOW;
      p_spi->h_spi->Init.CLKPhase    = SPI_PHASE_2EDGE;
      HAL_SPI_Init(p_spi->h_spi);
      break;

    // CPOL=1, CPHA=0
    case SPI_MODE2:
      p_spi->h_spi->Init.CLKPolarity = SPI_POLARITY_HIGH;
      p_spi->h_spi->Init.CLKPhase    = SPI_PHASE_1EDGE;
      HAL_SPI_Init(p_spi->h_spi);
      break;

    // CPOL=1, CPHA=1
    case SPI_MODE3:
      p_spi->h_spi->Init.CLKPolarity = SPI_POLARITY_HIGH;
      p_spi->h_spi->Init.CLKPhase    = SPI_PHASE_2EDGE;
      HAL_SPI_Init(p_spi->h_spi);
      break;
  }
}

void spiSetBitWidth(uint8_t ch, uint8_t bit_width)
{
  spi_t  *p_spi = &spi_tbl[ch];

  if (p_spi->is_open == false) return;

  p_spi->h_spi->Init.DataSize = SPI_DATASIZE_8BIT;

  if (bit_width == 16)
  {
    p_spi->h_spi->Init.DataSize = SPI_DATASIZE_16BIT;
    LL_SPI_SetDataWidth(p_spi->h_spi->Instance, LL_SPI_DATAWIDTH_16BIT);
  }
  else
  {
    LL_SPI_SetDataWidth(p_spi->h_spi->Instance, LL_SPI_DATAWIDTH_8BIT);
  }
  //HAL_SPI_Init(p_spi->h_spi);  
}

uint8_t spiTransfer8(uint8_t ch, uint8_t data)
{
  uint8_t ret;
  spi_t  *p_spi = &spi_tbl[ch];


  if (p_spi->is_open == false) return 0;

  HAL_SPI_TransmitReceive(p_spi->h_spi, &data, &ret, 1, 10);

  return ret;
}

uint16_t spiTransfer16(uint8_t ch, uint16_t data)
{
  uint8_t tBuf[2];
  uint8_t rBuf[2];
  uint16_t ret;
  spi_t  *p_spi = &spi_tbl[ch];


  if (p_spi->is_open == false) return 0;

  if (p_spi->h_spi->Init.DataSize == SPI_DATASIZE_8BIT)
  {
    tBuf[1] = (uint8_t)data;
    tBuf[0] = (uint8_t)(data>>8);
    HAL_SPI_TransmitReceive(p_spi->h_spi, (uint8_t *)&tBuf, (uint8_t *)&rBuf, 2, 10);

    ret = rBuf[0];
    ret <<= 8;
    ret += rBuf[1];
  }
  else
  {
    HAL_SPI_TransmitReceive(p_spi->h_spi, (uint8_t *)&data, (uint8_t *)&ret, 1, 10);
  }

  return ret;
}

bool spiTransfer(uint8_t ch, uint8_t *tx_buf, uint8_t *rx_buf, uint32_t length, uint32_t timeout)
{
  bool ret = true;
  HAL_StatusTypeDef status;
  spi_t  *p_spi = &spi_tbl[ch];

  if (p_spi->is_open == false) return false;

  if (rx_buf == NULL)
  {
    status =  HAL_SPI_Transmit(p_spi->h_spi, tx_buf, length, timeout);
  }
  else if (tx_buf == NULL)
  {
    status =  HAL_SPI_Receive(p_spi->h_spi, rx_buf, length, timeout);
  }
  else
  {
    status =  HAL_SPI_TransmitReceive(p_spi->h_spi, tx_buf, rx_buf, length, timeout);
  }

  if (status != HAL_OK)
  {
    return false;
  }

  return ret;
}

void spiDmaTxStart(uint8_t spi_ch, uint8_t *p_buf, uint32_t length)
{
  spi_t  *p_spi = &spi_tbl[spi_ch];

  if (p_spi->is_open == false) return;

  p_spi->is_tx_done = false;
  HAL_SPI_Transmit_DMA(p_spi->h_spi, p_buf, length);
}

bool spiDmaTxTransfer(uint8_t ch, void *buf, uint32_t length, uint32_t timeout)
{
  bool ret = true;
  uint32_t t_time;


  spiDmaTxStart(ch, (uint8_t *)buf, length);

  t_time = millis();

  if (timeout == 0) return true;

  while(1)
  {
    if(spiDmaTxIsDone(ch))
    {
      break;
    }
    if((millis()-t_time) > timeout)
    {
      ret = false;
      break;
    }
  }

  return ret;
}

bool spiDmaTxIsDone(uint8_t ch)
{
  spi_t  *p_spi = &spi_tbl[ch];

  if (p_spi->is_open == false)     return true;

  return p_spi->is_tx_done;
}

void spiAttachTxInterrupt(uint8_t ch, void (*func)())
{
  spi_t  *p_spi = &spi_tbl[ch];


  if (p_spi->is_open == false)     return;

  p_spi->func_tx = func;
}



void HAL_SPI_ErrorCallback(SPI_HandleTypeDef *hspi)
{
  if (hspi->Instance == spi_tbl[_DEF_SPI1].h_spi->Instance)
  {
    spi_tbl[_DEF_SPI1].is_error = true;
  }
}

void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
  spi_t  *p_spi;
  volatile uint32_t length;


  if (hspi->Instance == spi_tbl[_DEF_SPI1].h_spi->Instance)
  {
    p_spi = &spi_tbl[_DEF_SPI1];

    p_spi->is_tx_done = true;

    if(p_spi->dma_tx_buf.tx_length_next > 0)  
    {
      p_spi->dma_tx_buf.p_tx_buf = p_spi->dma_tx_buf.p_tx_buf_next;

      if(p_spi->dma_tx_buf.tx_length_next > SPI_TX_DMA_MAX_LENGTH)
      {
        length = SPI_TX_DMA_MAX_LENGTH;
        p_spi->dma_tx_buf.tx_length_next = p_spi->dma_tx_buf.tx_length_next - SPI_TX_DMA_MAX_LENGTH;
        p_spi->dma_tx_buf.p_tx_buf_next = &p_spi->dma_tx_buf.p_tx_buf[SPI_TX_DMA_MAX_LENGTH];
      }
      else
      {
        length = p_spi->dma_tx_buf.tx_length_next;
        p_spi->dma_tx_buf.tx_length_next = 0;
        p_spi->dma_tx_buf.p_tx_buf_next = NULL;
      }
      HAL_SPI_Transmit_DMA(hspi, p_spi->dma_tx_buf.p_tx_buf, length);
    }
    else
    {
      p_spi->dma_tx_buf.tx_done = true;

      if (p_spi->func_tx != NULL)
      {
        (*p_spi->func_tx)();
      }
    }
  }
}

void SPI4_IRQHandler(void)
{
  HAL_SPI_IRQHandler(&hspi4);
}

void DMA1_Stream5_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&hdma_spi4_tx);
}

void HAL_SPI_MspInit(SPI_HandleTypeDef* spiHandle)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  if(spiHandle->Instance==SPI4)
  {

  /** Initializes the peripherals clock
  */
    /* DMA controller clock enable */
    __HAL_RCC_DMA1_CLK_ENABLE();

    /* SPI4 clock enable */
    __HAL_RCC_SPI4_CLK_ENABLE();

    __HAL_RCC_GPIOE_CLK_ENABLE();
    /**SPI4 GPIO Configuration
    PE6     ------> SPI4_MOSI
    PE12     ------> SPI4_SCK
    */
    GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
    GPIO_InitStruct.Alternate = GPIO_AF5_SPI4;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

    /* SPI4 DMA Init */
    /* SPI4_TX Init */
    hdma_spi4_tx.Instance = DMA1_Stream5;
    hdma_spi4_tx.Init.Request = DMA_REQUEST_SPI4_TX;
    hdma_spi4_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_spi4_tx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_spi4_tx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_spi4_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    hdma_spi4_tx.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    hdma_spi4_tx.Init.Mode = DMA_NORMAL;
    hdma_spi4_tx.Init.Priority = DMA_PRIORITY_LOW;
    hdma_spi4_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_spi4_tx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(spiHandle,hdmatx,hdma_spi4_tx);

    /* SPI4 interrupt Init */
    HAL_NVIC_SetPriority(SPI4_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(SPI4_IRQn);

    /* DMA1_Stream3_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(DMA1_Stream5_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);
  }
}

void HAL_SPI_MspDeInit(SPI_HandleTypeDef* spiHandle)
{

  if(spiHandle->Instance==SPI3)
  {
    /* Peripheral clock disable */
    __HAL_RCC_SPI4_CLK_DISABLE();

    /**SPI4 GPIO Configuration
    PE6     ------> SPI4_MOSI
    PE12     ------> SPI4_SCK
    */
    HAL_GPIO_DeInit(GPIOE, GPIO_PIN_6|GPIO_PIN_12);

    /* SPI4 DMA DeInit */
    HAL_DMA_DeInit(spiHandle->hdmatx);

    /* SPI4 interrupt Deinit */
    HAL_NVIC_DisableIRQ(SPI4_IRQn);
  }
}


#endif
