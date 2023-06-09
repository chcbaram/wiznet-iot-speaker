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



bool wiznetInit(void);
void wiznetUpdate(void);

#ifdef __cplusplus
}
#endif


#endif 