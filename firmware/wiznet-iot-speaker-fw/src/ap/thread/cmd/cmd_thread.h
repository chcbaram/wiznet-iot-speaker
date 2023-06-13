#ifndef CMD_THREAD_H_
#define CMD_THREAD_H_


#include "ap_def.h"

#include "process/cmd_boot.h"
#include "process/cmd_audio.h"

bool cmdThreadInit(void);
bool cmdThreadUpdate(void);

#endif