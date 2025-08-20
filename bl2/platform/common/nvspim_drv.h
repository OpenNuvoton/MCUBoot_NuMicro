#ifndef NVSPIM_DRV_H
#define NVSPIM_DRV_H

#include <stdint.h>

int32_t NVT_NVSPIM_Init(uint32_t fnc);
int32_t NVT_NVSPIM_Read(uint32_t addr, uint32_t size, uint32_t *data);
int32_t NVT_NVSPIM_Program(uint32_t addr, uint32_t size, uint8_t *buf);
int32_t NVT_NVSPIM_Erase(uint32_t addr);

#if defined(SECONDARY_SLOT_IN_SPI_FLASH) && defined(USE_SPIM)
/* Use SPIM driver */
#ifdef PLATFORM_M460
#define NVT_NVSPIM_SECTOR_COUNT  (0x02000000 / NVT_NVSPIM_SECTOR_SIZE) // 32 MiB
#define NVT_NVSPIM_SECTOR_SIZE   (256)
#define NVT_NVSPIM_PAGE_SIZE     (256)
#define NVT_NVSPIM_PROGRAM_UNIT  (4)
#define NVT_NVSPIM_MEMORY_BASE   (0)
#endif
#endif

#endif