#include "cmd_udp.h"


typedef struct
{
  char     ip_addr[32];
  uint32_t port;
} cmd_udp_args_t;


static bool open(void *args);
static bool close(void *args);  
static uint32_t available(void *args);
static bool flush(void *args);
static uint8_t read(void *args);
static uint32_t write(void *args, uint8_t *p_data, uint32_t length);  

static bool is_init = false;
static bool is_open = false;

static uint8_t   socket_id = HW_WIZNET_SOCKET_BOOT;
static uint8_t   rx_buf[8*1024];
static qbuffer_t rx_q;


static reg32_t  dest_ip;
static reg16_t  dest_port;
static bool     dest_update = false;



bool cmdUdpInitDriver(cmd_driver_t *p_driver, const char *ip_addr, uint32_t port)
{
  cmd_udp_args_t *p_args = (cmd_udp_args_t *)p_driver->args;


  qbufferCreate(&rx_q, rx_buf, 8*1024);

  p_args->port  = port;
  strncpy(p_args->ip_addr, ip_addr, 32);

  p_driver->open = open;
  p_driver->close = close;
  p_driver->available = available;
  p_driver->flush = flush;
  p_driver->read = read;
  p_driver->write= write;

  is_init = true;

  return true;
}

bool open(void *args)
{
  bool ret = false;
  // cmd_udp_args_t *p_args = (cmd_udp_args_t *)args;
  reg16_t reg;


  reg.d16 = W5300_REGS->RMSR.d16[0];
  reg.d8[0] = 8;
  W5300_REGS->RMSR.d16[0] = reg.d16;

  logPrintf("[OK] cmdUdpOpen()\n");
  logPrintf("     TMSR0 : %d KB\n", W5300_REGS->TMSR.d8[0]);
  logPrintf("     RMSR0 : %d KB\n", W5300_REGS->RMSR.d8[0]);


  W5300_REGS->IMR.d16 = 0xFFFF;
  W5300_REGS->SOCKET[socket_id].PORTR.d16 = SWAP16(5000);
  W5300_REGS->SOCKET[socket_id].MR.d16   |= SWAP16(0x02);    
  W5300_REGS->SOCKET[socket_id].CR.d16    = SWAP16(0x01);

  ret = true;
  is_open = ret;

  return ret;
}

bool close(void *args)
{
  if (is_open == false) return true;

  W5300_REGS->SOCKET[socket_id].CR.d16 = SWAP16(0x08);
  W5300_REGS->SOCKET[socket_id].CR.d16 = SWAP16(0x10);

  is_open = false;

  return true;  
}

uint32_t available(void *args)
{
  if (W5300_REGS->SOCKET[socket_id].RX_RSR.d32 > 0)
  {
    reg16_t reg_size;
    reg16_t reg_data;
    uint16_t reg[2];

    (void)reg_size;
    (void)reg_data;

    dest_ip.d16[0] = W5300_REGS->SOCKET[socket_id].RX_FIFOR.d16;
    dest_ip.d16[1] = W5300_REGS->SOCKET[socket_id].RX_FIFOR.d16;
    dest_port.d16  = W5300_REGS->SOCKET[socket_id].RX_FIFOR.d16;
    dest_update = true;

    reg[0] = W5300_REGS->SOCKET[socket_id].RX_FIFOR.d16;
    reg_size.d16 = SWAP16(reg[0]);


    uint16_t rx_count;

    rx_count = reg_size.d16;
    for (uint32_t i=0; i<rx_count; i++)
    {
      if ((i & 1) == 0)
      {
        reg_data.d16 = W5300_REGS->SOCKET[socket_id].RX_FIFOR.d16;
        qbufferWrite(&rx_q, &reg_data.d8[0], 1);
      }
      else
      {
        qbufferWrite(&rx_q, &reg_data.d8[1], 1);
      }
    }

    W5300_REGS->SOCKET[socket_id].IR.d16 = W5300_REGS->SOCKET[socket_id].IR.d16;
    W5300_REGS->SOCKET[socket_id].CR.d16 = SWAP16(0x40);  
  }

  return qbufferAvailable(&rx_q);
}

bool flush(void *args)
{
  uint32_t pre_time;

  pre_time = millis();
  while(available(args) > 0 && millis()-pre_time < 200)
  {
    read(args);
  }
  qbufferFlush(&rx_q);
  return true;
}

uint8_t read(void *args)
{
  uint8_t ret;

  qbufferRead(&rx_q, &ret, 1);
  return ret;
}

uint32_t write(void *args, uint8_t *p_data, uint32_t length)
{
  uint32_t ret = 0;

  if (is_init == false) return 0;

  if (is_open == true && dest_update == true)
  {
    W5300_REGS->SOCKET[socket_id].DIPR.d32   = dest_ip.d32;
    W5300_REGS->SOCKET[socket_id].DPORTR.d16 = dest_port.d16;

    // logPrintf("%d %d %d %d : %d\n", 
    //     dest_ip.d8[0],
    //     dest_ip.d8[1],
    //     dest_ip.d8[2],
    //     dest_ip.d8[3],
    //     dest_port.d16
    // );

    uint32_t tx_count;
    uint32_t tx_index;

    if (length%2 == 0)
      tx_count = (length + 0) / 2;
    else
      tx_count = (length + 1) / 2;

    tx_index = 0;
    for (uint32_t i=0; i<tx_count; i++)
    {
      reg16_t reg;

      if (tx_index < length)
      {
        reg.d8[0] = p_data[tx_index];
      }
      tx_index++;

      if (tx_index < length)
      {
        reg.d8[1] = p_data[tx_index];
      }
      tx_index++;

      W5300_REGS->SOCKET[socket_id].TX_FIFOR.d16 = reg.d16;

      // logPrintf("tx %d 0x%04X\n", i, reg.d16);
    }
    // logPrintf("tx len : %d, %d\n", tx_count, length);

    W5300_REGS->SOCKET[socket_id].TX_WRSR.d16[0] = SWAP16((length>>16) & 0xFFFF);
    W5300_REGS->SOCKET[socket_id].TX_WRSR.d16[1] = SWAP16((length>> 0) & 0xFFFF);
    W5300_REGS->SOCKET[socket_id].CR.d16 = SWAP16(0x20);
    delay(1);
  }

  return ret;
}
