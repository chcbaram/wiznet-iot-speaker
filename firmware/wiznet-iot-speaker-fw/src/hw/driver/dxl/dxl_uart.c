/*
 * dxl_uart.c
 *
 *  Created on: 2021. 3. 8.
 *      Author: baram
 */


#include "dxl/dxl_uart.h"
#include "uart.h"


#ifdef _USE_HW_DXL

                              // _DEF_DXL1
static const uint8_t dxl_ch_tbl[] = 
  {
    HW_UART_CH_TTL
  };



static bool     open(uint8_t ch, uint32_t baud);
static bool     close(uint8_t ch);
static uint32_t available(uint8_t ch);
static uint32_t write(uint8_t ch, uint8_t *p_data, uint32_t length);
static uint8_t  read(uint8_t ch);
static uint8_t  read(uint8_t ch);
static bool     flush(uint8_t ch);





bool dxlUartDriver(dxl_driver_t *p_driver)
{
  p_driver->is_init     = true;
  
  p_driver->open        = open;
  p_driver->close       = close;
  p_driver->available   = available;
  p_driver->write       = write;
  p_driver->read        = read;
  p_driver->flush       = flush;

  return true;
}

bool open(uint8_t dxl_ch, uint32_t baud)
{
  bool ret = false;


  switch(dxl_ch)
  {
    case _DEF_DXL1:
      ret = uartOpen(dxl_ch_tbl[dxl_ch], baud);
      break;
  }

  return ret;
}

bool close(uint8_t dxl_ch)
{
  bool ret = false;


  switch(dxl_ch)
  {
    case _DEF_DXL1:
      ret = uartClose(dxl_ch_tbl[dxl_ch]);
      break;
  }

  return ret;
}

uint32_t available(uint8_t dxl_ch)
{
  uint32_t ret = 0;


  switch(dxl_ch)
  {
    case _DEF_DXL1:
      ret = uartAvailable(dxl_ch_tbl[dxl_ch]);
      break;
  }

  return ret;
}

uint32_t write(uint8_t dxl_ch, uint8_t *p_data, uint32_t length)
{
  uint32_t ret = 0;


  switch(dxl_ch)
  {
    case _DEF_DXL1:
      ret = uartWrite(dxl_ch_tbl[dxl_ch], p_data, length);
      break;
  }

  return ret;
}

uint8_t read(uint8_t dxl_ch)
{
  uint32_t ret = 0;


  switch(dxl_ch)
  {
    case _DEF_DXL1:
      ret = uartRead(dxl_ch_tbl[dxl_ch]);
      break;
  }

  return ret;
}

bool flush(uint8_t dxl_ch)
{
  bool ret = true;


  switch(dxl_ch)
  {
    case _DEF_DXL1:
      ret = uartFlush(dxl_ch_tbl[dxl_ch]);
      break;
  }

  return ret;
}


#endif
