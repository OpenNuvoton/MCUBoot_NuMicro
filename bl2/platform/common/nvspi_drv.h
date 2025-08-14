#ifndef NVSPI_DRV_H
#define NVSPI_DRV_H

#include <stdint.h>

int32_t NVT_NVSPI_Init(void);
int32_t NVT_NVSPI_Read(uint32_t addr, uint32_t len, uint8_t *data);
int32_t NVT_NVSPI_Program(uint32_t addr, uint32_t len, uint8_t *buf);
int32_t NVT_NVSPI_Erase(uint32_t addr);
int32_t NVT_NVSPI_WaitReady(void);

#if defined(SECONDARY_SLOT_IN_SPI_FLASH) && !defined(USE_SPIM)
/* Use SPI driver */
#ifdef PLATFORM_M460
#define NVT_NVSPI_SECTOR_COUNT  (0x00400000 / NVT_NVSPI_SECTOR_SIZE) // 4 MiB
#define NVT_NVSPI_SECTOR_SIZE   (4096)
#define NVT_NVSPI_PAGE_SIZE     (256)
#define NVT_NVSPI_PROGRAM_UNIT  (4)
#define NVT_NVSPI_MEMORY_BASE   (0x00800000)
#define SPI_FLASH_PORT          SPI2
#endif
#endif

#endif