#include "hw.h"






bool hwInit(void)
{
  bspInit();

  ledInit();


  return true;
}