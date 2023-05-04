#ifndef BUTTON_H_
#define BUTTON_H_


#ifdef __cplusplus
extern "C" {
#endif

#include "hw_def.h"

#ifdef _USE_HW_BUTTON

#define BUTTON_MAX_CH       HW_BUTTON_MAX_CH



bool     buttonInit(void);
bool     buttonGetPressed(uint8_t ch);
uint32_t buttonGetData(void);

#endif

#ifdef __cplusplus
}
#endif



#endif /* SRC_COMMON_HW_INCLUDE_BUTTON_H_ */