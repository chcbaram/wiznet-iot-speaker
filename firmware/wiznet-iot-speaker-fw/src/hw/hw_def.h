#ifndef HW_DEF_H_
#define HW_DEF_H_



#include "bsp.h"


#define _DEF_FIRMWATRE_VERSION    "V230504R1"
#define _DEF_BOARD_NAME           "IOT-SPEAKER-FW"


#define _USE_HW_FMC
#define _USE_HW_BUZZER
#define _USE_HW_FLASH
#define _USE_HW_QSPI
#define _USE_HW_FAULT
#define _USE_HW_SD


#define _USE_HW_LED
#define      HW_LED_MAX_CH          2

#define _USE_HW_UART
#define      HW_UART_MAX_CH         3
#define      HW_UART_CH_SWD         _DEF_UART1
#define      HW_UART_CH_DEBUG       _DEF_UART2
#define      HW_UART_CH_TTL         _DEF_UART3

#define _USE_HW_CLI
#define      HW_CLI_CMD_LIST_MAX    32
#define      HW_CLI_CMD_NAME_MAX    16
#define      HW_CLI_LINE_HIS_MAX    8
#define      HW_CLI_LINE_BUF_MAX    64

#define _USE_HW_LOG
#define      HW_LOG_CH              _DEF_UART1
#define      HW_LOG_BOOT_BUF_MAX    1024
#define      HW_LOG_LIST_BUF_MAX    1024

#define _USE_HW_BUTTON
#define      HW_BUTTON_MAX_CH       3

#define _USE_HW_SWTIMER
#define      HW_SWTIMER_MAX_CH      8

#define _USE_HW_GPIO
#define      HW_GPIO_MAX_CH         2

#define _USE_HW_W5300
#define      HW_W5300_BASE_ADDR     0x60000000

#define _USE_HW_FS
#define      HW_FS_FLASH_OFFSET     0x90000000
#define      HW_FS_MAX_SIZE         (8*1024*1024)

#define _USE_HW_DXL
#define      HW_DXL_PACKET_BUF_MAX  512
#define      HW_DXL_DEVICE_CNT_MAX  16


#define _PIN_GPIO_W5300_RST         0
#define _PIN_GPIO_SDCARD_DETECT     1

#endif