#include "cmd_audio.h"


#define AUDIO_CMD_INFO              0x0020
#define AUDIO_CMD_START             0x0021
#define AUDIO_CMD_STOP              0x0022
#define AUDIO_CMD_READY             0x0023
#define AUDIO_CMD_WRITE             0x0024



static uint8_t i2s_ch;



bool cmdAudioInit(void)
{
  i2s_ch = i2sGetEmptyChannel();
  return true;
}

static void cmdAudioReady(cmd_t *p_cmd)
{
  data_t data;

  data.u32D = i2sAvailableForWrite(i2s_ch);

  cmdSendResp(p_cmd, p_cmd->packet.cmd, CMD_OK, data.u8Data, 4);  
}

static void cmdAudioWrite(cmd_t *p_cmd)
{
  i2sWrite(i2s_ch, (int16_t *)p_cmd->packet.data, p_cmd->packet.length/2);

  cmdSendResp(p_cmd, p_cmd->packet.cmd, CMD_OK, NULL, 0);  
}

static void cmdAudioUpdate(cmd_t *p_cmd)
{
}

bool cmdAudioProcess(cmd_t *p_cmd)
{
  bool ret = true;


  switch(p_cmd->packet.cmd)
  {
    case AUDIO_CMD_READY:
      cmdAudioReady(p_cmd);
      break;

    case AUDIO_CMD_WRITE:
      cmdAudioWrite(p_cmd);
      break;

    default:
      ret = false;
      break;  
  }

  cmdAudioUpdate(p_cmd);

  return ret;
}

