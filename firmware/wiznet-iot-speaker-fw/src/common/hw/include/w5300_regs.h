#ifndef W5300_REGS_H_
#define W5300_REGS_H_

#ifdef __cplusplus
extern "C" {
#endif


#include "hw_def.h"

#ifdef _USE_HW_W5300



typedef union
{
  uint8_t  D8[2];
  uint16_t D16;
} reg16_t;

typedef union
{
  uint8_t  D8[4];
  uint16_t D16[2];
  uint32_t D32;
} reg32_t;


typedef union
{
  __IO reg16_t MR;              // 0x200 SOCKET0 Mode Register
  __IO reg16_t CR;              // 0x202 SOCKET0 Command Register
  __IO reg16_t IMR;             // 0x204 SOCKET0 Interrupt Mask Register
  __IO reg16_t IR;              // 0x206 SOCKET0 Interrupt Register
  __IO reg16_t SSR;             // 0x208 SOCKET0 Socket Status Register
  __IO reg16_t PORTR;           // 0x20A SOCKET0 Source Port Register
  __IO uint8_t DHAR[6];         // 0x20C SOCKET0 Destination Hardware Address Register
  __IO reg16_t DPORTR;          // 0x212 SOCKET0 Destination Port Register
  __IO uint8_t DIPR[4];         // 0x214 SOCKET0 Destination IP Address
  __IO reg16_t MSSR;            // 0x218 SOCKET0 Maximum Segment Size Register
  __IO uint8_t KPALVTR;         // 0x21A SOCKET0 Keep Alive Time Register
  __IO uint8_t PROTOR;          // 0x21B SOCKET0 Protocol Number Register
  __IO reg16_t TOSR;            // 0x21C SOCKET0 TOS Register
  __IO reg16_t TTLR;            // 0x21E SOCKET0 TTL Register
  __IO reg32_t TX_WRSR;         // 0x220 SOCKET0 TX Write Size Register
  __IO reg32_t TX_FSR;          // 0x224 SOCKET0 TX Free Size Register
  __IO reg32_t RX_RSR;          // 0x228 SOCKET0 RX Receive Size Register
  __IO reg16_t FRAGR;           // 0x22C SOCKET0 FLAG Register
  __IO reg16_t TX_FIFOR;        // 0x22E SOCKET0 TX FIFO Register
  __IO reg16_t RX_FIFOR;        // 0x230 SOCKET0 RX FIFO Register
  __IO uint8_t r0[0x0E];
} W5300_REG_SOCKET_t;


typedef struct
{
  //-- MODE
  //
  __IO reg16_t  MR;             // 0x000

  //-- COMMON
  //
  __IO uint8_t  IR[2];          // 0x002
  __IO uint8_t  IMR[2];         // 0x004
  __IO uint8_t  r0[2];          // 0x006
  __IO uint8_t  SHAR[6];        // 0x008
  __IO uint8_t  r1[2];          // 0x00E
  __IO uint8_t  GAR[4];         // 0x010
  __IO uint8_t  SUBR[4];        // 0x014 Subnet Mask Register
  __IO uint8_t  SIPR[4];        // 0x018 Source IP Address Register   
  __IO uint8_t  RTR[2];         // 0x01C Retransmission Timeout-value Register
  __IO uint8_t  RCR[2];         // 0x01E Retransmission Retry-count Register
  __IO uint8_t  TMSR[8];        // 0x020 Transmit Memory Size Register of SOCKET[0~7]
  __IO uint8_t  RMSR[8];        // 0x028 Receive Memory Size Register of SOCKET[0~7]
  __IO uint8_t  MTYPER[2];      // 0x030 Memory Block Type Register
  __IO uint8_t  PATR[2];        // 0x032 PPPoE Authentication Register
  __IO uint8_t  r2[2];          // 0x034
  __IO uint8_t  PTIMER[2];      // 0x036 PPP LCP Request Time Register
  __IO uint8_t  PMAGICR[2];     // 0x038 PPP LCP Magic Number Register
  __IO uint8_t  r3[2];          // 0x03A
  __IO uint8_t  PSIDR[2];       // 0x03C PPP Session ID Register
  __IO uint8_t  r4[2];          // 0x03E
  __IO uint8_t  PDHAR[6];       // 0x040 PPP Destination Hardware Address Register
  __IO uint8_t  r5[2];          // 0x046
  __IO uint8_t  UIPR[4];        // 0x048 Unreachable IP Address Register
  __IO uint8_t  UPORTR[2];      // 0x04C Unreachable Port Number Register
  __IO uint8_t  FMTUR[2];       // 0x04E Fragment MTU Register
  __IO uint8_t  r6[0x10];       // 0x050
  __IO uint8_t  P0_BRDYR[2];    // 0x060
  __IO uint8_t  P0_BDPTHR[2];   // 0x062
  __IO uint8_t  P1_BRDYR[2];    // 0x064
  __IO uint8_t  P1_BDPTHR[2];   // 0x066
  __IO uint8_t  P2_BRDYR[2];    // 0x068
  __IO uint8_t  P2_BDPTHR[2];   // 0x06A
  __IO uint8_t  P3_BRDYR[2];    // 0x06C
  __IO uint8_t  P3_BDPTHR[2];   // 0x06E
  __IO uint8_t  r7[0x8E];       // 0x070
  __IO reg16_t  IDR;            // 0x0FE


  __IO uint8_t  r8[0x100];      // 0x100

  //-- SOCKET
  //
  W5300_REG_SOCKET_t SOCKET[8];

} W5300_REGS_t;



#define W5300_REGS       ((W5300_REGS_t *)(HW_W5300_BASE_ADDR + 0x000))




#endif

#ifdef __cplusplus
}
#endif

#endif