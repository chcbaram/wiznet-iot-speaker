#include "audio.h"
#include "audio_def.h"
#include "cmd/driver/cmd_udp.h"


#define AUDIO_CMD_INFO              0x0020
#define AUDIO_CMD_BEGIN             0x0021
#define AUDIO_CMD_END               0x0022
#define AUDIO_CMD_READY             0x0023
#define AUDIO_CMD_WRITE             0x0024

typedef struct
{
  uint8_t  hw_type;
  char     file_name[128];
  uint32_t sample_rate;
  uint32_t file_size;
  uint8_t  file_type;
} audio_begin_t;

static uint16_t audioCmdBegin(audio_begin_t *p_data, uint32_t timeout);
static uint16_t audioCmdEnd(uint32_t timeout);
static uint16_t audioCmdReady(uint32_t *p_data, uint32_t timeout);
static uint16_t audioCmdWrite(void *p_data, uint32_t length, uint32_t timeout);
static char *getFileNameFromPath(char *path );
static int32_t getFileSize(char *file_name);

static bool is_init = false;
static cmd_t cmd;
static cmd_driver_t cmd_driver;
static audio_begin_t audio_begin;





void audioMain(arg_option_t *args)
{
  logPrintf("\n");
  logPrintf("audioMain()\n");



  char *file_name;
  char *file_path;
  int32_t file_size;
  int32_t file_index;
  FILE *fp;
  wavfile_header_t header;
  int32_t  volume = 100;


  if ((args->arg_bits & ARG_OPTION_PORT) == 0)
  {
    const char ip_str[32] = "255.255.255.255";
    strncpy(args->port_str, ip_str, 128);
    args->arg_bits |= ARG_OPTION_PORT;
    logPrintf("-p %s\n", args->port_str);
  }

  cmdUdpInitDriver(&cmd_driver, args->port_str, 5000);
  cmdInit(&cmd, &cmd_driver);
  cmdOpen(&cmd);  

  uartOpen(_USE_UART_CLI, 115200);

  file_path = args->file_str;
  file_name = getFileNameFromPath(args->file_str);
  file_size = getFileSize(file_path);

  logPrintf("FileName      : %s\n", file_name);
  logPrintf("FileSize      : %d KB\n", file_size/1024);
  logPrintf("\n");


  //-- File Open
  //
  fp = fopen(file_path, "r");
  if (fp == NULL)
  {
    logPrintf("fopen fail : %s\n", file_path);
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
  logPrintf("\n");


  int16_t buf_frame[4096];

  fseek(fp, sizeof(wavfile_header_t) + 1024, SEEK_SET);
  file_index = sizeof(wavfile_header_t) + 1024;

  uint32_t ready_cnt;
  uint16_t err_ret;


  // audioBegin()
  //
  audio_begin.hw_type = args->type;
  strncpy(audio_begin.file_name, file_name, 128);
  audio_begin.file_size = file_size;
  
  audioCmdBegin(&audio_begin, 100);


  while(uartAvailable(_DEF_UART1) == 0)
  {
    int len;
    bool ready_ret;

    ready_ret = false;
    ready_cnt = 0;
    for (int i=0; i<3; i++)
    {
      if (audioCmdReady(&ready_cnt, 100) == CMD_OK)
      {
        ready_ret = true;
        break;
      }
    }
    if (ready_ret == false)
    {
      logPrintf("audioCmdReady() Fail\n");
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
      file_index += len;

      float percent;

      percent = (float)file_index * 100. / (float)file_size;

      logPrintf("Play          : %3.2f%%\r", percent);

      int16_t buf_data[r_len*2 + 2];

      buf_data[0] = (file_index >>  0) & 0xFFFF;
      buf_data[1] = (file_index >> 16) & 0xFFFF;

      for (int i=0; i<r_len; i++)
      {
        if (header.NumChannels == 2)
        {
          buf_data[i*2 + 0 + 2] = buf_frame[i*2 + 0] * volume / 100;;
          buf_data[i*2 + 1 + 2] = buf_frame[i*2 + 1] * volume / 100;;
        }
        else
        {
          buf_data[i*2 + 0 + 2] = buf_frame[i] * volume / 100;;
          buf_data[i*2 + 1 + 2] = buf_frame[i] * volume / 100;;
        }
      }
      err_ret = audioCmdWrite(buf_data, r_len * 2 * 2 + 4, 500);
      if (err_ret != CMD_OK)
      {
        logPrintf("audioCmdWrite() Fail\n");
        // break;
      }
    }
  }
  fclose(fp);

  audioCmdEnd(100);

  is_init = true;
  return;
}

uint16_t audioCmdBegin(audio_begin_t *p_data, uint32_t timeout)
{
  uint16_t ret = CMD_OK;
  cmd_t *p_cmd = &cmd;


  cmdSendCmdRxResp(p_cmd, AUDIO_CMD_BEGIN, (uint8_t *)p_data, sizeof(audio_begin_t), timeout);
  ret = p_cmd->packet.err_code;

  return ret;
}

uint16_t audioCmdEnd(uint32_t timeout)
{
  uint16_t ret = CMD_OK;
  cmd_t *p_cmd = &cmd;


  cmdSendCmdRxResp(p_cmd, AUDIO_CMD_END, NULL, 0, timeout);
  ret = p_cmd->packet.err_code;

  return ret;
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

char *getFileNameFromPath(char *path )
{
  if( path == NULL )
      return NULL;

  char  *pFileName = path;
  for (char * pCur = path; *pCur != '\0'; pCur++)
  {
    if( *pCur == '/' || *pCur == '\\' )
      pFileName = pCur+1;
  }
  
  return pFileName;
}

int32_t getFileSize(char *file_name)
{
  int32_t ret = -1;
  FILE *fp;

  if ((fp = fopen( file_name, "rb")) == NULL)
  {
    fprintf( stderr, "Unable to open %s\n", file_name );
    return -1;
  }
  else
  {
    fseek( fp, 0, SEEK_END );
    ret = ftell( fp );
    fclose(fp);
  }

  return ret;
}