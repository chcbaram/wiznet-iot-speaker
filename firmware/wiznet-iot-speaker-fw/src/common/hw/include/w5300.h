#ifndef W5300_H_
#define W5300_H_

#ifdef __cplusplus
extern "C" {
#endif


#include "hw_def.h"

#ifdef _USE_HW_W5300


#define W5300_BASE_ADDR HW_W5300_BASE_ADDR



bool w5300Init(void);
bool w5300IsInit(void);
bool w5300Reset(void);

#endif

#ifdef __cplusplus
}
#endif

#endif