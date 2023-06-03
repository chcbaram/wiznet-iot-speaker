#include "boot.h"
#include "driver/cmd_uart.h"



#define BOOT_CMD_INFO                   0x0000
#define BOOT_CMD_VERSION                0x0001

#define BOOT_CMD_FLASH_ERASE            0x0003
#define BOOT_CMD_FLASH_WRITE            0x0004
#define BOOT_CMD_FLASH_READ             0x0005

#define BOOT_CMD_FW_VER                 0x0006
#define BOOT_CMD_FW_ERASE               0x0007
#define BOOT_CMD_FW_WRITE               0x0008
#define BOOT_CMD_FW_READ                0x0009
#define BOOT_CMD_FW_VERIFY              0x000A
#define BOOT_CMD_FW_UPDATE              0x000B
#define BOOT_CMD_FW_JUMP                0x000C




static cmd_t cmd_boot;
static cmd_driver_t cmd_driver;





bool bootInit(uint8_t ch, char *port_name, uint32_t baud)
{
  bool ret;


  if (port_name != NULL)
  {
    uartSetPortName(ch, port_name);
  }

  cmdUartInitDriver(&cmd_driver, ch, baud);
  cmdInit(&cmd_boot, &cmd_driver);

  ret = cmdOpen(&cmd_boot);

  return ret;
}

bool bootDeInit(uint8_t ch)
{
  bool ret;

  ret = uartClose(ch);

  return ret;
}

uint16_t bootCmdReadInfo(boot_info_t *p_info)
{
  uint16_t ret = CMD_OK;
  cmd_t *p_cmd = &cmd_boot;


  if (cmdSendCmdRxResp(p_cmd, BOOT_CMD_INFO, NULL, 0, 100) == true)
  {
    cmd_packet_t *p_packet = &p_cmd->packet;

    memset(p_info, 0, sizeof(boot_info_t));
    if (p_packet->err_code == CMD_OK)
    {
      if (p_packet->length == sizeof(boot_info_t))
      {
        memcpy(p_info, p_packet->data, p_packet->length);
      } 
      else
      {
        p_packet->err_code = ERR_CMD_RX_LENGTH;
      }    
    }
  } 

  ret = p_cmd->packet.err_code;

  return ret;
}


uint16_t bootCmdReadVersion(boot_version_t *version)
{
  uint16_t ret = CMD_OK;
  cmd_t *p_cmd = &cmd_boot;


  if (cmdSendCmdRxResp(p_cmd, BOOT_CMD_VERSION, NULL, 0, 100) == true)
  {
    cmd_packet_t *p_packet = &p_cmd->packet;

    memset(version, 0, sizeof(boot_version_t));
    if (p_packet->err_code == CMD_OK)
    {
      if (p_packet->length == sizeof(boot_version_t))
      {
        memcpy(version, p_packet->data, p_packet->length);
      } 
      else
      {
        p_packet->err_code = ERR_CMD_RX_LENGTH;
      }    
    }
  } 

  ret = p_cmd->packet.err_code;

  return ret;
}


uint16_t bootCmdFirmVersion(firm_ver_t *version)
{
  uint16_t ret = CMD_OK;
  cmd_t *p_cmd = &cmd_boot;


  if (cmdSendCmdRxResp(p_cmd, BOOT_CMD_FW_VER, NULL, 0, 100) == true)
  {
    cmd_packet_t *p_packet = &p_cmd->packet;

    memset(version, 0, sizeof(firm_ver_t));
    if (p_packet->err_code == CMD_OK)
    {
      if (p_packet->length == sizeof(firm_ver_t))
      {
        memcpy(version, p_packet->data, p_packet->length);
      } 
      else
      {
        p_packet->err_code = ERR_CMD_RX_LENGTH;
      }    
    }
  } 

  ret = p_cmd->packet.err_code;

  return ret;
}

uint16_t bootCmdFirmErase(uint32_t addr, uint32_t length, uint32_t timeout)
{
  uint16_t ret = CMD_OK;
  cmd_t *p_cmd = &cmd_boot;
  uint8_t tx_buf[8];


  tx_buf[0] = (addr >>  0) & 0xFF;
  tx_buf[1] = (addr >>  8) & 0xFF;
  tx_buf[2] = (addr >> 16) & 0xFF;
  tx_buf[3] = (addr >> 24) & 0xFF;

  tx_buf[4] = (length >>  0) & 0xFF;
  tx_buf[5] = (length >>  8) & 0xFF;
  tx_buf[6] = (length >> 16) & 0xFF;
  tx_buf[7] = (length >> 24) & 0xFF;

  cmdSendCmdRxResp(p_cmd, BOOT_CMD_FW_ERASE, tx_buf, 8, timeout);

  ret = p_cmd->packet.err_code;

  return ret;
}

uint16_t bootCmdFirmWrite(uint32_t addr, uint8_t *p_data, uint32_t length, uint32_t timeout)
{
  uint16_t ret = CMD_OK;
  cmd_t *p_cmd = &cmd_boot;
  uint8_t *tx_buf;


  tx_buf = p_cmd->packet.data;

  tx_buf[0] = (addr >>  0) & 0xFF;
  tx_buf[1] = (addr >>  8) & 0xFF;
  tx_buf[2] = (addr >> 16) & 0xFF;
  tx_buf[3] = (addr >> 24) & 0xFF;

  tx_buf[4] = (length >>  0) & 0xFF;
  tx_buf[5] = (length >>  8) & 0xFF;
  tx_buf[6] = (length >> 16) & 0xFF;
  tx_buf[7] = (length >> 24) & 0xFF;

  for (int i=0; i<length; i++)
  {
    tx_buf[8+i] = p_data[i];  
  }

  cmdSendCmdRxResp(p_cmd, BOOT_CMD_FW_WRITE, tx_buf, 8+length, timeout);

  ret = p_cmd->packet.err_code;

  return ret;
}

uint16_t bootCmdFirmRead(uint32_t addr, uint8_t *p_data, uint32_t length, uint32_t timeout)
{
  uint16_t ret = CMD_OK;
  cmd_t *p_cmd = &cmd_boot;
  uint8_t *tx_buf;


  tx_buf = p_cmd->packet.data;

  tx_buf[0] = (addr >>  0) & 0xFF;
  tx_buf[1] = (addr >>  8) & 0xFF;
  tx_buf[2] = (addr >> 16) & 0xFF;
  tx_buf[3] = (addr >> 24) & 0xFF;

  tx_buf[4] = (length >>  0) & 0xFF;
  tx_buf[5] = (length >>  8) & 0xFF;
  tx_buf[6] = (length >> 16) & 0xFF;
  tx_buf[7] = (length >> 24) & 0xFF;


  if (cmdSendCmdRxResp(p_cmd, BOOT_CMD_FW_READ, tx_buf, 8, timeout) == true)
  {
    cmd_packet_t *p_packet = &p_cmd->packet;

    if (p_packet->err_code == CMD_OK)
    {
      for (int i=0; i<p_packet->length; i++)
      {
        p_data[i] = p_packet->data[i];
      }  
    }
  }

  ret = p_cmd->packet.err_code;

  return ret; 
}

uint16_t bootCmdFirmVerify(uint32_t timeout)
{
  uint16_t ret = CMD_OK;
  cmd_t *p_cmd = &cmd_boot;


  cmdSendCmdRxResp(p_cmd, BOOT_CMD_FW_VERIFY, NULL, 0, timeout);
  ret = p_cmd->packet.err_code;

  return ret;  
}
