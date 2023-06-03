#ifndef CMD_BOOT_H_
#define CMD_BOOT_H_


#include "ap_def.h"


typedef struct
{
  uint32_t mode;
} boot_info_t;


typedef struct
{
  firm_ver_t boot;
  firm_ver_t firm;
  firm_ver_t update;
} boot_version_t;


bool cmdBootProcess(cmd_t *p_cmd);


#endif