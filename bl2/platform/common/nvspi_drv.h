#ifndef NVSPI_DRV_H
#define NVSPI_DRV_H

#include <stdint.h>

int32_t NVT_NVSPI_Init(uint32_t fnc);
int32_t NVT_NVSPI_Read(uint32_t addr, uint32_t size, uint32_t *data);
int32_t NVT_NVSPI_Program(uint32_t addr, uint32_t size, uint8_t *buf);
int32_t NVT_NVSPI_Erase(uint32_t addr);

#ifdef SPI_FLASH_M460
#define NVT_NVSPI_SECTOR_COUNT  (0x02000000 / NVT_NVSPI_SECTOR_SIZE) // 32 MiB
#define NVT_NVSPI_SECTOR_SIZE   (256)
#define NVT_NVSPI_PAGE_SIZE     (256)
#define NVT_NVSPI_PROGRAM_UNIT  (4)
#define NVT_NVSPI_MEMORY_BASE   (0x00100000)
#endif

#endif