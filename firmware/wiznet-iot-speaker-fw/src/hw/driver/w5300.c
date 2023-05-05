#include "w5300.h"
#include "cli.h"
#include "gpio.h"




#ifdef _USE_HW_CLI
static void cliCmd(cli_args_t *args);
#endif

static bool is_init = false;
static const int32_t gpio_rst = _PIN_GPIO_W5300_RST;

static const uint8_t default_ip[4]      = {172, 30,  1,  55};
static const uint8_t default_subnet[4]  = {255,255, 255,  1};
static const uint8_t default_gateway[4] = {192,168,   0,  1};
static const uint8_t default_mac[6]     = {0, 1, 2, 3, 4, 5};





bool w5300Init(void)
{

  w5300Reset();

  if (W5300_REGS->MR.d16 != 0xB800)
  {
    logPrintf("[NG] w5300 not found\n");
    return false;
  }

  W5300_REGS->MR.d16 |= (1<<8);
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
    delay(5);
  }
  return true;
}

bool w5300SetNetInfo(const uint8_t ip[4], 
                     const uint8_t subnet[4], 
                     const uint8_t gateway[4], 
                     const uint8_t mac[6])
{
  if (ip != NULL)
  {
    reg32_t reg;

    memcpy(reg.d8, ip, 4);
    W5300_REGS->SIPR.d32  = reg.d32;
  }

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
    cliPrintf("     id      : 0x%02X%02X\n", W5300_REGS->IDR.d8[0], W5300_REGS->IDR.d8[1]);

    cliPrintf("     ip      : %d.%d.%d.%d\n", ip[0], ip[1], ip[2], ip[3]);
    cliPrintf("     subnet  : %d.%d.%d.%d\n", subnet[0], subnet[1], subnet[2], subnet[3]);
    cliPrintf("     gateway : %d.%d.%d.%d\n", gateway[0], gateway[1], gateway[2], gateway[3]);
    cliPrintf("     mac     : %02X:%02X:%02X:%02X:%02X:%02X\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    ret = true;
  }

  if (args->argc == 1 && args->isStr(0, "status"))
  {
    while(cliKeepLoop())
    {
      // cliPrintf("IR 0x%X %X\n", W5300_REGS_COMMON->IR, W5300_REGS_COMMON->SIPR);
      delay(100);
    } 
    ret = true;
  }

  if (ret == false)
  {
    cliPrintf("w5300 info\n");
    cliPrintf("w5300 status\n");
  }
}
#endif