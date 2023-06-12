#include "audio.h"
#include "cmd/driver/cmd_udp.h"


#define AUDIO_CMD_INFO              0x0020
#define AUDIO_CMD_START             0x0021
#define AUDIO_CMD_STOP              0x0022
#define AUDIO_CMD_READY             0x0023
#define AUDIO_CMD_WRITE             0x0024


typedef struct wavfile_header_s
{
  char    ChunkID[4];     /*  4   */
  int32_t ChunkSize;      /*  4   */
  char    Format[4];      /*  4   */

  char    Subchunk1ID[4]; /*  4   */
  int32_t Subchunk1Size;  /*  4   */
  int16_t AudioFormat;    /*  2   */
  int16_t NumChannels;    /*  2   */
  int32_t SampleRate;     /*  4   */
  int32_t ByteRate;       /*  4   */
  int16_t BlockAlign;     /*  2   */
  int16_t BitsPerSample;  /*  2   */

  char    Subchunk2ID[4];
  int32_t Subchunk2Size;
} wavfile_header_t;


static uint16_t audioCmdReady(uint32_t *p_data, uint32_t timeout);
static uint16_t audioCmdWrite(void *p_data, uint32_t length, uint32_t timeout);

static bool is_init = false;
static cmd_t cmd;
static cmd_driver_t cmd_driver;






void audioMain(int argc, char *argv[])
{
  cmdUdpInitDriver(&cmd_driver, "255.255.255.255", 5000);
  cmdInit(&cmd, &cmd_driver);

  cmdOpen(&cmd);  

  logPrintf("\n");
  logPrintf("audioMain()\n");


  char *file_name = "test2.wav";
  FILE *fp;
  wavfile_header_t header;
  int32_t  volume = 100;
  int8_t ch;


  logPrintf("FileName      : %s\n", file_name);


  fp = fopen(file_name, "r");
  if (fp == NULL)
  {
    logPrintf("fopen fail : %s\n", file_name);
    return;
  }
  fread(&header, sizeof(wavfile_header_t), 1, fp);

  logPrintf("ChunkSize     : %d\n", header.ChunkSize);
  logPrintf("Format        : %c%c%c%c\n", header.Format[0], header.Format[1], header.Format[2], header.Format[3]);
  logPrintf("Subchunk1Size : %d\n", header.Subchunk1Size);
  logPrintf("AudioFormat   : %d\n", header.AudioFormat);
  logPrintf("NumChannels   : %d\n", header.NumChannels);
  logPrintf("SampleRate    : %d\n", header.SampleRate);
  logPrintf("ByteRate      : %d\n", header.ByteRate);
  logPrintf("BlockAlign    : %d\n", header.BlockAlign);
  logPrintf("BitsPerSample : %d\n", header.BitsPerSample);
  logPrintf("Subchunk2Size : %d\n", header.Subchunk2Size);


  int16_t buf_frame[4096];

  fseek(fp, sizeof(wavfile_header_t) + 1024, SEEK_SET);

  uint32_t ready_cnt;
  uint16_t err_ret;


  while(1)
  {
    int len;

    ready_cnt = 0;
    if (audioCmdReady(&ready_cnt, 500) != CMD_OK)
    {
      logPrintf("ready fail\n");
      break;
    }

    if (ready_cnt > 0)
    {
      int r_len;

      r_len = constrain(ready_cnt, 0, 256);
      r_len = r_len / 2;

      len = fread(buf_frame, 1, r_len * 2 * header.NumChannels, fp);

      if (len != r_len*2*header.NumChannels)
      {
        logPrintf("r_len : %d\n", r_len);
        logPrintf("len : %d, %d\n", len, r_len*2*header.NumChannels);
        break;
      }
      else
      {
        logPrintf("ready : %d, len : %d, r_len %d\n", ready_cnt,  len, r_len);
      }

      int16_t buf_data[r_len*2];

      for (int i=0; i<r_len; i++)
      {
        if (header.NumChannels == 2)
        {
          buf_data[i*2 + 0] = buf_frame[i*2 + 0] * volume / 100;;
          buf_data[i*2 + 1] = buf_frame[i*2 + 1] * volume / 100;;
        }
        else
        {
          buf_data[i*2 + 0] = buf_frame[i] * volume / 100;;
          buf_data[i*2 + 1] = buf_frame[i] * volume / 100;;
        }
      }
      err_ret = audioCmdWrite(buf_data, r_len * 2 * 2, 500);
      if (err_ret != CMD_OK)
      {
        logPrintf("audioCmdWrite() Fail\n");
        break;
      }
    }
  }

  fclose(fp);



  is_init = true;
  return;
}


uint16_t audioCmdReady(uint32_t *p_data, uint32_t timeout)
{
  uint16_t ret = CMD_OK;
  cmd_t *p_cmd = &cmd;

  *p_data = 0;

  if (cmdSendCmdRxResp(p_cmd, AUDIO_CMD_READY, NULL, 0, timeout) == true)
  {
    cmd_packet_t *p_packet = &p_cmd->packet;

    if (p_packet->err_code == CMD_OK)
    {
      memcpy(p_data, p_cmd->packet.data, 4);
    }
  } 
  ret = p_cmd->packet.err_code;

  return ret;
}

uint16_t audioCmdWrite(void *p_data, uint32_t length, uint32_t timeout)
{
  uint16_t ret = CMD_OK;
  cmd_t *p_cmd = &cmd;


  if (cmdSendCmdRxResp(p_cmd, AUDIO_CMD_WRITE, (uint8_t *)p_data, length, timeout) == true)
  {
    cmd_packet_t *p_packet = &p_cmd->packet;

    if (p_packet->err_code == CMD_OK)
    {
    }
  } 
  ret = p_cmd->packet.err_code;

  return ret;
}

void audioDeInit(void)
{
  if (is_init)
  {
    cmdClose(&cmd);
  }
}
