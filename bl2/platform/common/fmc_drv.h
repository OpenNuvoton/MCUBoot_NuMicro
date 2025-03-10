#ifndef FMC_DRV_H_
#define FMC_DRV_H_

#include <stdint.h>
#include "NuMicro.h"

#define NVT_FMC_OK          0
#define NVT_FMC_ERROR       -1
#define NVT_FMC_TIMEOUT     -2
#define NVT_FMC_ISPFF       -3
#define NVT_FMC_PARAMETER   -4

int32_t NVT_FMC_Read(uint32_t addr, uint32_t *data);
int32_t NVT_FMC_Program(uint32_t addr, uint64_t dword);
int32_t NVT_FMC_Erase(uint32_t addr);

#endif