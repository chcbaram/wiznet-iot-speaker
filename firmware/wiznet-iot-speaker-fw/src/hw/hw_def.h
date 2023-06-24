#ifndef HW_DEF_H_
#define HW_DEF_H_



#include "bsp.h"


#define _DEF_FIRMWATRE_VERSION    "V230616R1"
#define _DEF_BOARD_NAME           "IOT-SPEAKER-FW"


#define _USE_HW_FMC
#define _USE_HW_BUZZER
#define _USE_HW_FLASH
#define _USE_HW_QSPI
#define _USE_HW_FAULT
#define _USE_HW_SD
#define _USE_HW_FATFS
#define _USE_HW_FILES
#define _USE_HW_SAI
#define _USE_HW_ES8156
#define _USE_HW_NVS


#define _USE_HW_LED
#define      HW_LED_MAX_CH          2

#define _USE_HW_UART
#define      HW_UART_MAX_CH         4
#define      HW_UART_CH_SWD         _DEF_UART1
#define      HW_UART_CH_DEBUG       _DEF_UART2
#define      HW_UART_CH_TTL         _DEF_UART3
#define      HW_UART_CH_USB         _DEF_UART4

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
#define      HW_BUTTON_MAX_CH       5

#define _USE_HW_SWTIMER
#define      HW_SWTIMER_MAX_CH      8

#define _USE_HW_GPIO
#define      HW_GPIO_MAX_CH         10

#define _USE_HW_W5300
#define      HW_W5300_BASE_ADDR     0x60000000

#define _USE_HW_FS
#define      HW_FS_FLASH_OFFSET     0x90000000
#define      HW_FS_MAX_SIZE         (8*1024*1024)

#define _USE_HW_DXL
#define      HW_DXL_PACKET_BUF_MAX  512
#define      HW_DXL_DEVICE_CNT_MAX  16

#define _USE_HW_I2C
#define      HW_I2C_MAX_CH          1

#define _USE_HW_ENCODER
#define      HW_ENCODER_MAX_CH      2

#define _USE_HW_SPI
#define      HW_SPI_MAX_CH          1

#define _USE_HW_PWM
#define      HW_PWM_MAX_CH          1

#define _USE_HW_LCD
#define      HW_LCD_SWAP_RGB        0
#define      HW_LCD_LVGL            1
#define      HW_LCD_LOGO            1
#define _USE_HW_ST7789
#define      HW_LCD_WIDTH           240
#define      HW_LCD_HEIGHT          240

#define _USE_HW_MIXER
#define      HW_MIXER_MAX_CH        4
#define      HW_MIXER_MAX_BUF_LEN   (48*2*4*4) // 48Khz * Stereo * 4ms * 2

#define _USE_HW_USB
#define _USE_HW_CDC
#define      HW_USE_CDC             1
#define      HW_USE_MSC             1

#define _USE_HW_RESET
#define      HW_RESET_BOOT          0

#define _USE_HW_RTC
#define      HW_RTC_BOOT_MODE       RTC_BKP_DR3
#define      HW_RTC_RESET_BITS      RTC_BKP_DR4

#define _USE_HW_I2S
#define      HW_I2S_LCD             1

#define _USE_HW_CMD
#define      HW_CMD_MAX_DATA_LENGTH 2048

#define _USE_HW_WIZNET
#define      HW_WIZNET_SOCKET_CMD   0
#define      HW_WIZNET_SOCKET_DHCP  1
#define      HW_WIZNET_SOCKET_SNTP  2


#define _PIN_GPIO_W5300_RST         0
#define _PIN_GPIO_SDCARD_DETECT     1
#define _PIN_GPIO_SPK_EN            2
#define _PIN_GPIO_LCD_BLK           7
#define _PIN_GPIO_LCD_RST           8
#define _PIN_GPIO_LCD_DC            9

#define _BTN_A                      0   
#define _BTN_B                      1   
#define _BTN_C                      2   
#define _BTN_ENC_L                  3   
#define _BTN_ENC_R                  4   


#define FLASH_SIZE_TAG              0x400
#define FLASH_SIZE_VER              0x400
#define FLASH_SIZE_FIRM             (1024*1024 - 128*1024)

#define FLASH_ADDR_BOOT             0x08000000
#define FLASH_ADDR_FIRM             0x08020000

#define FLASH_ADDR_UPDATE           0x90800000


#endif