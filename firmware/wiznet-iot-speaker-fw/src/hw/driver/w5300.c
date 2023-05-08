#include "w5300.h"
#include "cli.h"
#include "gpio.h"



#define SWAP16(x)   (uint16_t)(((uint16_t)x<<8) | ((uint16_t)x>>8))


#ifdef _USE_HW_CLI
static void cliCmd(cli_args_t *args);
#endif

static bool is_init = false;
static const int32_t gpio_rst = _PIN_GPIO_W5300_RST;

static const uint8_t default_ip[4]      = {172, 30,  1,  55};
static const uint8_t default_subnet[4]  = {255,255, 255,  0};
static const uint8_t default_gateway[4] = {172, 30,  1, 254};
static const uint8_t default_mac[6]     = {0, 1, 2, 3, 4, 5};





bool w5300Init(void)
{

  w5300Reset();

  if (W5300_REGS->MR.d16 != 0xB800)
  {
    logPrintf("[NG] w5300 not found\n");
    return false;
  }

  W5300_REGS->MR.d16 = 0;
  // W5300_REGS->MR.d16 |= (1<<8);
  W5300_REGS->MR.d16 |= (1<<2);
  delay(50);

  w5300SetNetInfo(default_ip,
                  default_subnet,
                  default_gateway,
                  default_mac);


  logPrintf("[OK] w5300Init()\n");

  uint8_t ip[4];
  uint8_t subnet[4];
  uint8_t gateway[4];
  uint8_t mac[6];


  w5300GetNetInfo(ip, subnet, gateway, mac);

  logPrintf("     ip      : %d.%d.%d.%d\n", ip[0], ip[1], ip[2], ip[3]);
  logPrintf("     subnet  : %d.%d.%d.%d\n", subnet[0], subnet[1], subnet[2], subnet[3]);
  logPrintf("     gateway : %d.%d.%d.%d\n", gateway[0], gateway[1], gateway[2], gateway[3]);
  logPrintf("     mac     : %02X:%02X:%02X:%02X:%02X:%02X\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

  reg16_t reg;
  reg.d16 = SWAP16(W5300_REGS->RTR.d16);
  logPrintf("     rtr     : %d ms\n", reg.d16/10);
  logPrintf("     rcr     : %d \n", W5300_REGS->RCR.d8[1]);


  #ifdef _USE_HW_CLI
  cliAdd("w5300", cliCmd);
  #endif

  is_init = true;

  return true;
}

bool w5300IsInit(void)
{
  return is_init;
}

bool w5300Reset(void)
{
  if (gpio_rst >= 0)
  {
    gpioPinWrite(gpio_rst, _DEF_LOW);
    delay(5);
    gpioPinWrite(gpio_rst, _DEF_HIGH);
    delay(10);
  }
  return true;
}

bool w5300SetNetInfo(const uint8_t ip[4], 
                     const uint8_t subnet[4], 
                     const uint8_t gateway[4], 
                     const uint8_t mac[6])
{
  if (gateway != NULL)
  {
    reg32_t reg;

    memcpy(reg.d8, gateway, 4);
    W5300_REGS->GAR.d32  = reg.d32;
  }

  if (subnet != NULL)
  {
    reg32_t reg;

    memcpy(reg.d8, subnet, 4);
    W5300_REGS->SUBR.d32 = reg.d32;
  }

  if (mac != NULL)
  {
    reg48_t reg;

    reg.d8[0] = mac[0];
    reg.d8[1] = mac[1];
    reg.d8[2] = mac[2];
    reg.d8[3] = mac[3];
    reg.d8[4] = mac[4];
    reg.d8[5] = mac[5];
    
    W5300_REGS->SHAR.d16[0]  = reg.d16[0];
    W5300_REGS->SHAR.d16[1]  = reg.d16[1];
    W5300_REGS->SHAR.d16[2]  = reg.d16[2];
  }

  if (ip != NULL)
  {
    reg32_t reg;
    reg16_t rtr;
    reg16_t rcr;

    rtr.d16 = W5300_REGS->RTR.d16;
    rcr.d16 = W5300_REGS->RCR.d16;

    memcpy(reg.d8, ip, 4);

    W5300_REGS->SIPR.d32 = reg.d32;

    W5300_REGS->RTR.d16 = rtr.d16;
    W5300_REGS->RCR.d16 = rcr.d16;
  }  
  return true; 
}

bool w5300GetNetInfo(uint8_t ip[4], 
                     uint8_t subnet[4], 
                     uint8_t gateway[4], 
                     uint8_t mac[6])
{
  if (ip != NULL)
  {
    memcpy(ip, (void *)W5300_REGS->SIPR.d8, 4);
  }

  if (subnet != NULL)
  {
    memcpy(subnet, (void *)W5300_REGS->SUBR.d8, 4);
  }

  if (gateway != NULL)
  {
    memcpy(gateway, (void *)W5300_REGS->GAR.d8, 4);
  }  

  if (mac != NULL)
  {
    memcpy(mac, (void *)W5300_REGS->SHAR.d8, 6);
  } 

  return true;
}

// static void setRTR(uint16_t time_ms)
// {
//   reg16_t reg;

//   reg.d16 = time_ms * 10;
//   W5300_REGS->RTR.d16 = SWAP16(reg.d16);
// }

static uint16_t getRTR(void)
{
  reg16_t reg;

  reg.d16 = SWAP16(W5300_REGS->RTR.d16);
  return reg.d16/10;
}


#ifdef _USE_HW_CLI
void cliCmd(cli_args_t *args)
{
  bool ret = false;


  if (args->argc == 1 && args->isStr(0, "info"))
  {
    uint8_t ip[4];
    uint8_t subnet[4];
    uint8_t gateway[4];
    uint8_t mac[6];

    w5300GetNetInfo(ip, subnet, gateway, mac);

    cliPrintf("w5300 info\n");
    cliPrintf("     init    : %s\n", is_init ? "OK":"Fail");
    cliPrintf("     id      : 0x%04X\n", SWAP16(W5300_REGS->IDR.d16));

    cliPrintf("     ip      : %d.%d.%d.%d\n", ip[0], ip[1], ip[2], ip[3]);
    cliPrintf("     subnet  : %d.%d.%d.%d\n", subnet[0], subnet[1], subnet[2], subnet[3]);
    cliPrintf("     gateway : %d.%d.%d.%d\n", gateway[0], gateway[1], gateway[2], gateway[3]);
    cliPrintf("     mac     : %02X:%02X:%02X:%02X:%02X:%02X\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    cliPrintf("     rtr     : %d ms\n", getRTR());
    ret = true;
  }

  if (args->argc == 1 && args->isStr(0, "status"))
  {
    while(cliKeepLoop())
    {
      cliPrintf("IR 0x%02X%02X\n", W5300_REGS->IR.d8[0], W5300_REGS->IR.d8[1]);
      delay(100);
    } 
    ret = true;
  }

  if (args->argc == 1 && args->isStr(0, "udp"))
  {
    reg16_t reg;
    uint32_t pre_time;
    uint32_t rx_length;


    reg.d16 = W5300_REGS->RMSR.d16[0];
    reg.d8[0] = 8;
    W5300_REGS->RMSR.d16[0] = reg.d16;

    cliPrintf("TMSR0 : %d KB\n", W5300_REGS->TMSR.d8[0]);
    cliPrintf("RMSR0 : %d KB\n", W5300_REGS->RMSR.d8[0]);


    W5300_REGS->IMR.d16 = 0xFFFF;
    W5300_REGS->SOCKET[0].PORTR.d16 = SWAP16(5000);
    W5300_REGS->SOCKET[0].MR.d16   |= SWAP16(0x2);    
    W5300_REGS->SOCKET[0].CR.d16    = SWAP16(0x01);

    rx_length = 0;
    while(cliKeepLoop())
    {
      if (millis()-pre_time >= 1000)
      {
        pre_time = millis();
        cliPrintf("SSR 0x%02X, %X, %X, RSR : 0x%08X, %d KB/s %d MBps/s\n", 
          W5300_REGS->SOCKET[0].SSR.d8[1], 
          W5300_REGS->IR.d16,
          W5300_REGS->SOCKET[0].IR.d8[1],
          W5300_REGS->SOCKET[0].RX_RSR.d32,
          rx_length/1024,
          rx_length*8/1000000
          );
        rx_length = 0;
      }

      if (W5300_REGS->SOCKET[0].RX_RSR.d32 > 0)
      {
        reg32_t reg_ip;
        reg16_t reg_port;
        reg16_t reg_size;
        reg16_t reg_data;

        (void)reg_ip;
        (void)reg_port;
        (void)reg_size;
        (void)reg_data;

        // cliPrintf("RSR : 0x%08X\n", W5300_REGS->SOCKET[0].RX_RSR.d32);

        reg_ip.d16[0] = W5300_REGS->SOCKET[0].RX_FIFOR.d16;
        reg_ip.d16[1] = W5300_REGS->SOCKET[0].RX_FIFOR.d16;
        // for (int i=0; i<4; i++)
        // {
        //   cliPrintf("%d.", reg_ip.d8[i]);
        // }
        // cliPrintf("\n");

        uint16_t reg[2];

        reg[0] = W5300_REGS->SOCKET[0].RX_FIFOR.d16;
        reg[1] = W5300_REGS->SOCKET[0].RX_FIFOR.d16;
        reg_port.d16 = SWAP16(reg[0]);
        reg_size.d16 = SWAP16(reg[1]);

        // cliPrintf("port : %X\n", reg_port.d16);
        // cliPrintf("size : %X\n", reg_size.d16);

        rx_length += reg_size.d16;

        if (reg_size.d16%2 == 0)
          reg_size.d16 = reg_size.d16 / 2;
        else
          reg_size.d16 = (reg_size.d16 + 1) / 2;

        for (int i=0; i<reg_size.d16; i++)
        {
          reg_data.d16 = W5300_REGS->SOCKET[0].RX_FIFOR.d16;
          // cliPrintf("0x%02X\n", reg_data.d8[0]);
          // cliPrintf("0x%02X\n", reg_data.d8[1]);
        }

        // cliPrintf("RSR : 0x%08X\n", W5300_REGS->SOCKET[0].RX_RSR.d32);

        W5300_REGS->SOCKET[0].IR = W5300_REGS->SOCKET[0].IR;
        W5300_REGS->SOCKET[0].CR.d16 = SWAP16(0x40);  
      }

    } 

    W5300_REGS->SOCKET[0].CR.d16 = SWAP16(0x08);
    delay(100);
    W5300_REGS->SOCKET[0].CR.d16 = SWAP16(0x10);
    delay(100);
    cliPrintf("SSR 0x%02X, %X, %X\n", 
      W5300_REGS->SOCKET[0].SSR.d8[1], 
      W5300_REGS->IR.d16,
      W5300_REGS->SOCKET[0].IR.d8[1]);

    ret = true;
  }

  if (ret == false)
  {
    cliPrintf("w5300 info\n");
    cliPrintf("w5300 status\n");
    cliPrintf("w5300 udp\n");
  }
}
#endif