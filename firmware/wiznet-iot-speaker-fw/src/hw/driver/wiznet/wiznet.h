#ifndef WIZNET_H_
#define WIZNET_H_

#ifdef __cplusplus
extern "C" {
#endif


#include "hw_def.h"

#include "wiznet/w5300.h"
#include "wiznet/wizchip_conf.h"
#include "wiznet/socket/socket.h"
#include "wiznet/dhcp/dhcp.h"
#include "wiznet/dns/dns.h"
#include "wiznet/sntp/sntp.h"
#include "wiznet/mqtt/mqtt.h"


typedef struct 
{
  uint8_t mac[6];  ///< Source Mac Address
  uint8_t ip[4];   ///< Source IP Address
  uint8_t sn[4];   ///< Subnet Mask 
  uint8_t gw[4];   ///< Gateway IP Address
  uint8_t dns[4];  ///< DNS server IP Address
  bool    dhcp;    
} wiznet_info_t;


bool wiznetInit(void);
bool wiznetDHCP(void);
bool wiznetSNTP(void);
bool wiznetMQTT(void);
void wiznetUpdate(void);
bool wiznetIsGetIP(void);
bool wiznetGetInfo(wiznet_info_t *p_info);

#ifdef __cplusplus
}
#endif


#endif 