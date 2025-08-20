#ifndef NVQSPI_DRV_H
#define NVQSPI_DRV_H

#include <stdint.h>

int32_t NVT_NVQSPI_Init(void);
int32_t NVT_NVQSPI_Read(uint32_t addr, uint32_t len, uint8_t *data);
int32_t NVT_NVQSPI_Program(uint32_t addr, uint32_t len, uint8_t *buf);
int32_t NVT_NVQSPI_Erase(uint32_t addr);
int32_t NVT_NVQSPI_WaitReady(void);

#if defined(SECONDARY_SLOT_IN_SPI_FLASH) && defined(USE_QSPI)
/* Use QSPI driver */
#ifdef PLATFORM_M460
#define NVT_NVQSPI_SECTOR_COUNT  (0x02000000 / NVT_NVQSPI_SECTOR_SIZE) // 32 MiB
#define NVT_NVQSPI_SECTOR_SIZE   (4096)
#define NVT_NVQSPI_PAGE_SIZE     (256)
#define NVT_NVQSPI_PROGRAM_UNIT  (4)
#define NVT_NVQSPI_MEMORY_BASE   (0)
#define QSPI_FLASH_PORT          QSPI0
#endif
#endif

#endif