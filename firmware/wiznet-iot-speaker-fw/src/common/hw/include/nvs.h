/*
 * nvs.h
 *
 *  Created on: 2022. 9. 17.
 *      Author: baram
 */

#ifndef SRC_COMMON_HW_INCLUDE_NVS_H_
#define SRC_COMMON_HW_INCLUDE_NVS_H_

#ifdef __cplusplus
extern "C" {
#endif


#include "hw_def.h"

#ifdef _USE_HW_NVS


typedef struct
{
  uint32_t crc;
  uint32_t len;
} nvs_tag_t;


bool nvsInit(void);
bool nvsIsInit(void);

bool nvsIsExist(const char *p_name);
bool nvsSet(const char *p_name, void *p_data, uint32_t length);
bool nvsGet(const char *p_name, void *p_data, uint32_t length);

#endif


#ifdef __cplusplus
}
#endif

#endif 