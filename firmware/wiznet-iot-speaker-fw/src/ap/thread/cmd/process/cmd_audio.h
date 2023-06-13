#ifndef CMD_AUDIO_H_
#define CMD_AUDIO_H_


#include "ap_def.h"



bool cmdAudioInit(void);
bool cmdAudioIsBusy(void);
bool cmdAudioProcess(cmd_t *p_cmd);


#endif