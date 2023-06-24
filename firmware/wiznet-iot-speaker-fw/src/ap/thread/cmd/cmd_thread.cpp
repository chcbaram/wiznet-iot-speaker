#include "cmd_thread.h"
#include "driver/cmd_uart.h"
#include "driver/cmd_udp.h"
#include "process/cmd_boot.h"
#include "process/cmd_audio.h"



#define CMD_DRIVER_MAX_CH     2


static cmd_t        cmd[CMD_DRIVER_MAX_CH];
static cmd_driver_t cmd_drvier[CMD_DRIVER_MAX_CH];




bool cmdThreadInit(void)
{
  cmdUartInitDriver(&cmd_drvier[0], HW_UART_CH_USB, 1000000);  
  cmdInit(&cmd[0], &cmd_drvier[0]);
  cmdOpen(&cmd[0]);

  cmdUdpInitDriver(&cmd_drvier[1], NULL, 5000);  
  cmdInit(&cmd[1], &cmd_drvier[1]);
  cmdOpen(&cmd[1]);

  cmdBootInit();
  cmdAudioInit();
  return true;
}

bool cmdThreadUpdate(void)
{
  for (int i=0; i<CMD_DRIVER_MAX_CH; i++)
  {
    if (cmd[i].is_init == true)
    {
      if (cmdReceivePacket(&cmd[i]) == true)
      {
        bool ret = false;
        ret |= cmdBootProcess(&cmd[i]);
        ret |= cmdAudioProcess(&cmd[i]);

        if (ret != true)
        {
          cmdSendResp(&cmd[i], cmd[i].packet.cmd, ERR_CMD_NO_CMD, NULL, 0);
        }
      }
      cmdBootUpdate(&cmd[i]);
      cmdAudioUpdate(&cmd[i]);
    }
  }

  

  return true;
}
