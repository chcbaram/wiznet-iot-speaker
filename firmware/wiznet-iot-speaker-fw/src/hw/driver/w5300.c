#include "w5300.h"
#include "cli.h"
#include "gpio.h"
#include "w5300_regs.h"



#ifdef _USE_HW_CLI
static void cliCmd(cli_args_t *args);
#endif

static bool is_init = false;
static const int32_t gpio_rst = _PIN_GPIO_W5300_RST;





bool w5300Init(void)
{
  w5300Reset();


  if (W5300_REGS->MR.D16 != 0xB800)
  {
    logPrintf("[NG] w5300 not found\n");
    return false;
  }

  W5300_REGS->MR.D8[1] |= (1<<0);
  W5300_REGS->MR.D8[0] |= (1<<2);

  W5300_REGS->GAR[0] = 192;
  W5300_REGS->GAR[1] = 168;
  W5300_REGS->GAR[2] = 0;
  W5300_REGS->GAR[3] = 1;

  W5300_REGS->SUBR[0] = 255;
  W5300_REGS->SUBR[1] = 255;
  W5300_REGS->SUBR[2] = 255;
  W5300_REGS->SUBR[3] = 0;

  W5300_REGS->SIPR[0] = 172;
  W5300_REGS->SIPR[1] = 30;
  W5300_REGS->SIPR[2] = 1;
  W5300_REGS->SIPR[3] = 55;


  logPrintf("[OK] w5300Init()\n");

  #ifdef _USE_HW_CLI
  cliAdd("w5300", cliCmd);
  #endif

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
  }
  return true;
}


#ifdef _USE_HW_CLI
void cliCmd(cli_args_t *args)
{
  bool ret = false;


  if (args->argc == 1 && args->isStr(0, "info"))
  {
    cliPrintf("W5300 ID : 0x%04X\n", W5300_REGS->IDR.D16);
    // cliPrintf("Data Bus Width        \t: %s\n", W5300_REG_MR->DBW ? "16Bit":"8Bit");
    // cliPrintf("MAC Layer Pause Frame \t: %s\n", W5300_REG_MR->MPF ? "Pause frame":"Normal frame");
    // cliPrintf("Write Data Fetch Time \t: %d\n", W5300_REG_MR->WDF);
    // cliPrintf("Read Data Hold Time   \t: %s\n", W5300_REG_MR->RDH ? "Use":"No use");
    // cliPrintf("FIFO Swap Bit         \t: %s\n", W5300_REG_MR->FS ? "Enable":"Disable");
    // cliPrintf("Ping Block Mode       \t: %d\n", W5300_REG_MR->PB);
    // cliPrintf("PPPoE Mode            \t: %d\n", W5300_REG_MR->PPPoE);
    // cliPrintf("Data Bus Swap         \t: %s\n", W5300_REG_MR->DBS ? "Enable":"Disable");
    // cliPrintf("Indirect Bus          \t: %s\n", W5300_REG_MR->IND ? "Indirect":"Direct");

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