#ifndef NVMC_DRV_H_
#define NVMC_DRV_H_

#include <stdint.h>
#include "NuMicro.h"

#define NVT_NVMC_OK         0
#define NVT_NVMC_ERROR      -1
#define NVT_NVMC_TIMEOUT    -2
#define NVT_NVMC_ISPFF      -3
#define NVT_NVMC_PARAMETER  -4

int32_t NVT_NVMC_Init(void);
int32_t NVT_NVMC_Read(uint32_t addr, uint32_t *data);
int32_t NVT_NVMC_Program(uint32_t addr, uint64_t dword);
int32_t NVT_NVMC_Erase(uint32_t addr);

typedef struct _NVT_NVMC_T
{
    int32_t     (*init) (void);
    int32_t     (*read) (uint32_t addr, uint32_t *data);
    int32_t     (*program) (uint32_t addr, uint64_t dword);
    int32_t     (*erase) (uint32_t addr);
} NVT_NVMC_T;

#if NVMC_TYPE_RMC == 1
#define NVT_NVMC_APROM_MEMORY_BASE  (RMC_APROM_BASE)
#define NVT_NVMC_LDROM_MEMORY_BASE  (RMC_LDROM_BASE)
#define NVT_NVMC_APROM_SECTOR_COUNT (RMC_APROM_SIZE / RMC_FLASH_PAGE_SIZE)
#define NVT_NVMC_LDROM_SECTOR_COUNT (RMC_LDROM_SIZE / RMC_FLASH_PAGE_SIZE)
#define NVT_NVMC_SECTOR_SIZE        (RMC_FLASH_PAGE_SIZE)
#define NVT_NVMC_PAGE_SIZE          (RMC_FLASH_PAGE_SIZE)
#define NVT_NVMC_PROGRAM_UNIT       (4)
#else
#define NVT_NVMC_APROM_MEMORY_BASE  (FMC_APROM_BASE)
#define NVT_NVMC_LDROM_MEMORY_BASE  (FMC_LDROM_BASE)
#define NVT_NVMC_APROM_SECTOR_COUNT (FMC_APROM_SIZE / FMC_FLASH_PAGE_SIZE)
#define NVT_NVMC_LDROM_SECTOR_COUNT (FMC_LDROM_SIZE / FMC_FLASH_PAGE_SIZE)
#define NVT_NVMC_SECTOR_SIZE        (FMC_FLASH_PAGE_SIZE)
#define NVT_NVMC_PAGE_SIZE          (FMC_FLASH_PAGE_SIZE)
#define NVT_NVMC_PROGRAM_UNIT       (8)
#endif

#endif