#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "Driver_Flash.h"
#include "NuMicro.h"
#include "nvmc_drv.h"
#include "nvspim_drv.h"
#include "nvspi_drv.h"
#include "nvqspi_drv.h"
#include "flash_layout.h"
#include "region_defs.h"

#ifndef ARG_UNUSED
#define ARG_UNUSED(arg)  ((void)arg)
#endif

/* Driver version */
#define ARM_FLASH_DRV_VERSION    ARM_DRIVER_VERSION_MAJOR_MINOR(1, 0)

/**
 * Data width values for ARM_FLASH_CAPABILITIES::data_width
 * \ref ARM_FLASH_CAPABILITIES
 */
enum {
    DATA_WIDTH_8BIT = 0u,
    DATA_WIDTH_16BIT,
    DATA_WIDTH_32BIT,
    DATA_WIDTH_ENUM_SIZE
};

static const uint32_t data_width_byte[DATA_WIDTH_ENUM_SIZE] = {
    sizeof(uint8_t),
    sizeof(uint16_t),
    sizeof(uint32_t),
};

/*
 * ARM FLASH device structure
 */
struct arm_flash_dev_t
{
    const uint32_t memory_base;   /*!< FLASH memory base address */
    ARM_FLASH_INFO *data;         /*!< FLASH data */
};

/* Flash Status */
static ARM_FLASH_STATUS FlashStatus = {0, 0, 0};

/* Driver Version */
static const ARM_DRIVER_VERSION DriverVersion =
{
    ARM_FLASH_API_VERSION,
    ARM_FLASH_DRV_VERSION
};

/* Driver Capabilities */
static const ARM_FLASH_CAPABILITIES DriverCapabilities =
{
    0, /* event_ready */
    2, /* data_width = 0:8-bit, 1:16-bit, 2:32-bit */
    1  /* erase_chip */
};

static int32_t is_write_aligned(struct arm_flash_dev_t *flash_dev,
                                uint32_t param)
{
    int32_t rc = 0;

    if((param % flash_dev->data->program_unit) != 0)
    {
        rc = -1;
    }
    return rc;
}

static int32_t is_sector_aligned(struct arm_flash_dev_t *flash_dev,
                                 uint32_t offset)
{
    int32_t rc = 0;

    if((offset % flash_dev->data->sector_size) != 0)
    {
        rc = -1;
    }
    return rc;
}

static ARM_FLASH_INFO ARM_FLASH0_DEV_DATA =
{
    .sector_info = NULL,
    .sector_count = NVT_NVMC_APROM_SECTOR_COUNT,
    .sector_size  = NVT_NVMC_SECTOR_SIZE,
    .page_size    = NVT_NVMC_SECTOR_SIZE,
    .program_unit = NVT_NVMC_PROGRAM_UNIT,
    .erased_value = 0xFF
};

static struct arm_flash_dev_t ARM_FLASH0_DEV =
{
    .memory_base = NVT_NVMC_APROM_MEMORY_BASE,
    .data        = &ARM_FLASH0_DEV_DATA
};

struct arm_flash_dev_t *FLASH0_DEV = &ARM_FLASH0_DEV;

static ARM_FLASH_INFO ARM_FLASH1_DEV_DATA =
{
    .sector_info = NULL,
    .sector_count = NVT_NVMC_LDROM_SECTOR_COUNT,
    .sector_size  = NVT_NVMC_SECTOR_SIZE,
    .page_size    = NVT_NVMC_SECTOR_SIZE,
    .program_unit = NVT_NVMC_PROGRAM_UNIT,
    .erased_value = 0xFF
};

static struct arm_flash_dev_t ARM_FLASH1_DEV =
{
    .memory_base = NVT_NVMC_LDROM_MEMORY_BASE,
    .data        = &ARM_FLASH1_DEV_DATA
};

struct arm_flash_dev_t *FLASH1_DEV = &ARM_FLASH1_DEV;

#ifdef SECONDARY_SLOT_IN_SPI_FLASH
#if defined(USE_SPIM)
static ARM_FLASH_INFO ARM_FLASH2_DEV_DATA =
{
    .sector_info = NULL,
    .sector_count = NVT_NVSPIM_SECTOR_COUNT,
    .sector_size  = NVT_NVSPIM_SECTOR_SIZE,
    .page_size    = NVT_NVSPIM_PAGE_SIZE,
    .program_unit = NVT_NVSPIM_PROGRAM_UNIT,
    .erased_value = 0xFF
};

static struct arm_flash_dev_t ARM_FLASH2_DEV =
{
    .memory_base = NVT_NVSPIM_MEMORY_BASE,
    .data        = &ARM_FLASH2_DEV_DATA
};
#elif defined(USE_SPI)
static ARM_FLASH_INFO ARM_FLASH2_DEV_DATA =
{
    .sector_info = NULL,
    .sector_count = NVT_NVSPI_SECTOR_COUNT,
    .sector_size  = NVT_NVSPI_SECTOR_SIZE,
    .page_size    = NVT_NVSPI_PAGE_SIZE,
    .program_unit = NVT_NVSPI_PROGRAM_UNIT,
    .erased_value = 0xFF
};

static struct arm_flash_dev_t ARM_FLASH2_DEV =
{
    .memory_base = NVT_NVSPI_MEMORY_BASE,
    .data        = &ARM_FLASH2_DEV_DATA
};
#elif defined(USE_QSPI)
static ARM_FLASH_INFO ARM_FLASH2_DEV_DATA =
{
    .sector_info = NULL,
    .sector_count = NVT_NVQSPI_SECTOR_COUNT,
    .sector_size  = NVT_NVQSPI_SECTOR_SIZE,
    .page_size    = NVT_NVQSPI_PAGE_SIZE,
    .program_unit = NVT_NVQSPI_PROGRAM_UNIT,
    .erased_value = 0xFF
};

static struct arm_flash_dev_t ARM_FLASH2_DEV =
{
    .memory_base = NVT_NVQSPI_MEMORY_BASE,
    .data        = &ARM_FLASH2_DEV_DATA
};
#endif

struct arm_flash_dev_t *FLASH2_DEV = &ARM_FLASH2_DEV;
#endif

/*
 * Functions
 */

static int32_t is_range_valid_internal(struct arm_flash_dev_t *dev,
                                       uint32_t addr)
{
    uint32_t flash_size = 0;
    int32_t rc = 0;

    flash_size = (dev->data->sector_count * dev->data->sector_size);

    if((addr - dev->memory_base) >= flash_size)
    {
        rc = -1;
    }
    return rc;
}

static int32_t is_range_valid_APROM(uint32_t addr)
{
    return is_range_valid_internal(FLASH0_DEV, addr);
}

static int32_t is_range_valid_LDROM(uint32_t addr)
{
    return is_range_valid_internal(FLASH1_DEV, addr);
}

static ARM_DRIVER_VERSION ARM_Flash_GetVersion(void)
{
    return DriverVersion;
}

static ARM_FLASH_CAPABILITIES ARM_Flash_GetCapabilities(void)
{
    return DriverCapabilities;
}

static int32_t ARM_Flash_Initialize(ARM_Flash_SignalEvent_t cb_event)
{
    ARG_UNUSED(cb_event);

    if (DriverCapabilities.data_width >= DATA_WIDTH_ENUM_SIZE) {
        return ARM_DRIVER_ERROR;
    }

    NVT_NVMC_Init();

    return ARM_DRIVER_OK;
}

static int32_t ARM_Flash_Uninitialize(void)
{
    /* Nothing to be done */
    return ARM_DRIVER_OK;
}

static int32_t ARM_Flash_PowerControl(ARM_POWER_STATE state)
{
    switch(state)
    {
        case ARM_POWER_FULL:
            /* Nothing to be done */
            return ARM_DRIVER_OK;
            break;

        case ARM_POWER_OFF:
        case ARM_POWER_LOW:
        default:
            return ARM_DRIVER_ERROR_UNSUPPORTED;
    }
}

static int32_t ARM_Flash_EraseSector_Internal(uint32_t addr)
{
    int32_t ret;

    ret = NVT_NVMC_Erase(addr);
    if (ret != NVT_NVMC_OK)
    {
        return ARM_DRIVER_ERROR;
    }

    return ARM_DRIVER_OK;
}

static int32_t ARM_Flash_EraseSector_APROM(uint32_t addr)
{
    uint32_t rc = 0;
    int32_t ret;

    rc  = is_range_valid_APROM(addr);
    rc |= is_sector_aligned(FLASH0_DEV, addr);
    if (rc != 0)
    {
        return ARM_DRIVER_ERROR_PARAMETER;
    }

    ret = ARM_Flash_EraseSector_Internal(addr);
    if (ret != ARM_DRIVER_OK)
    {
        return ARM_DRIVER_ERROR;
    }

    return ARM_DRIVER_OK;
}

static int32_t ARM_Flash_EraseSector_LDROM(uint32_t addr)
{
    uint32_t rc = 0;
    int32_t ret;

    rc  = is_range_valid_LDROM(addr);
    rc |= is_sector_aligned(FLASH1_DEV, addr);
    if (rc != 0)
    {
        return ARM_DRIVER_ERROR_PARAMETER;
    }

    ret = ARM_Flash_EraseSector_Internal(addr);
    if (ret != ARM_DRIVER_OK)
    {
        return ARM_DRIVER_ERROR;
    }

    return ARM_DRIVER_OK;
}

static inline void clean_invalidate_dcacheline(void *ptr) {
#if defined (__DCACHE_PRESENT) && (__DCACHE_PRESENT == 1U)
    uint32_t addr = (uint32_t)ptr & ~(__SCB_DCACHE_LINE_SIZE - 1);
    SCB_CleanInvalidateDCache_by_Addr((volatile void *)addr, __SCB_DCACHE_LINE_SIZE);
#endif
}

static int32_t ARM_Flash_ReadData_Internal(uint32_t start_addr, void *data, uint32_t sz)
{
    uint8_t *dest;
    uint32_t remain;
    uint32_t addr;
    uint32_t r_data;
    uint32_t i;
    int32_t ret;

    dest = data;
    remain = sz;
    addr = start_addr;

    // printf("[%s]: Read 0x%x (0x%x bytes)\n", __func__, addr, sz);
    while (remain > 0)
    {
        ret = NVT_NVMC_Read(addr, &r_data);
        if (ret != NVT_NVMC_OK)
        {
            return ARM_DRIVER_ERROR;
        }

        if (remain > 4)
        {
            memcpy(dest, &r_data, 4);
            clean_invalidate_dcacheline(dest);
            dest += 4;
            addr += 4;
            remain -= 4;
        }
        else
        {
            memcpy(dest, &r_data, remain);
            clean_invalidate_dcacheline(dest);
            break;
        }
    }

    return ARM_DRIVER_OK;
}

static int32_t ARM_Flash_ReadData_APROM(uint32_t addr, void *data, uint32_t cnt)
{
    int32_t rc;
    uint32_t sz;
    int32_t ret;

    /* Conversion between data items and bytes */
    sz = cnt * data_width_byte[DriverCapabilities.data_width];

    /* Check flash memory boundaries */
    rc = is_range_valid_APROM(addr + sz - 1);
    if (rc != 0) {
        return ARM_DRIVER_ERROR_PARAMETER;
    }

    ret = ARM_Flash_ReadData_Internal(addr, data, sz);
    if (ret != ARM_DRIVER_OK)
    {
        return ARM_DRIVER_ERROR;
    }

    return cnt;
}

static int32_t ARM_Flash_ReadData_LDROM(uint32_t addr, void *data, uint32_t cnt)
{
    int32_t rc;
    uint32_t sz;
    int32_t ret;

    /* Conversion between data items and bytes */
    sz = cnt * data_width_byte[DriverCapabilities.data_width];

    /* Check flash memory boundaries */
    rc = is_range_valid_LDROM(addr + sz - 1);
    if (rc != 0) {
        return ARM_DRIVER_ERROR_PARAMETER;
    }

    ret = ARM_Flash_ReadData_Internal(addr, data, sz);
    if (ret != ARM_DRIVER_OK)
    {
        return ARM_DRIVER_ERROR;
    }

    return cnt;
}

static int32_t ARM_Flash_ProgramData_Internal(uint32_t addr, const void *data, uint32_t sz, uint32_t prog_unit)
{
    int32_t rc = 0;
    uint32_t dst_offst;
    uint32_t src_offst;
    int32_t ret;
    uint64_t dword;

    // printf("[%s]: Write 0x%x (0x%x bytes)\n", __func__, addr, sz);

    src_offst = 0;
    for (dst_offst = 0; dst_offst < sz; dst_offst += prog_unit)
    {
        /* Create local copy to avoid unaligned access */
        memcpy(&dword, data + src_offst, prog_unit);

        ret = NVT_NVMC_Program(addr + dst_offst, dword);
        if (ret != NVT_NVMC_OK)
        {
            return ARM_DRIVER_ERROR;
        }

        src_offst += prog_unit;
    }

    return ARM_DRIVER_OK;
}

static int32_t ARM_Flash_ProgramData_APROM(uint32_t addr, const void *data, uint32_t cnt)
{
    int32_t ret;
    int32_t rc;
    uint32_t sz;

    sz = cnt * data_width_byte[DriverCapabilities.data_width];

    /* Check flash memory boundaries and alignment with minimal write size */
    rc  = is_range_valid_APROM(addr + sz - 1);
    rc |= is_write_aligned(FLASH0_DEV, addr);
    rc |= is_write_aligned(FLASH0_DEV, sz);
    if(rc != 0)
    {
        return ARM_DRIVER_ERROR_PARAMETER;
    }

    ret = ARM_Flash_ProgramData_Internal(addr, data, sz, FLASH0_DEV->data->program_unit);
    if (ret != ARM_DRIVER_OK)
    {
        return ARM_DRIVER_ERROR;
    }

    return cnt;
}

static int32_t ARM_Flash_ProgramData_LDROM(uint32_t addr, const void *data, uint32_t cnt)
{
    int32_t ret;
    int32_t rc;
    uint32_t sz;

    sz = cnt * data_width_byte[DriverCapabilities.data_width];

    /* Check flash memory boundaries and alignment with minimal write size */
    rc  = is_range_valid_LDROM(addr + sz - 1);
    rc |= is_write_aligned(FLASH1_DEV, addr);
    rc |= is_write_aligned(FLASH1_DEV, sz);
    if(rc != 0)
    {
        return ARM_DRIVER_ERROR_PARAMETER;
    }

    ret = ARM_Flash_ProgramData_Internal(addr, data, sz, FLASH1_DEV->data->program_unit);
    if (ret != ARM_DRIVER_OK)
    {
        return ARM_DRIVER_ERROR;
    }

    return cnt;
}

static ARM_FLASH_STATUS ARM_Flash_GetStatus(void)
{
    return FlashStatus;
}

static ARM_FLASH_INFO * ARM_Flash_GetInfo_APROM(void)
{
    return FLASH0_DEV->data;
}

static ARM_FLASH_INFO * ARM_Flash_GetInfo_LDROM(void)
{
    return FLASH1_DEV->data;
}

#ifdef SECONDARY_SLOT_IN_SPI_FLASH
#if defined(USE_SPIM)
static int32_t ARM_Flash_Initialize_SPIM(ARM_Flash_SignalEvent_t cb_event)
{
    ARG_UNUSED(cb_event);

    if (DriverCapabilities.data_width >= DATA_WIDTH_ENUM_SIZE) {
        return ARM_DRIVER_ERROR;
    }

    if (NVT_NVSPIM_Init(0xAD004444) != 0) {
        return ARM_DRIVER_ERROR;
    }

    return ARM_DRIVER_OK;
}

static int32_t ARM_Flash_ReadData_SPIM(uint32_t addr, void *data, uint32_t cnt)
{
    uint32_t u32Len;
    uint32_t sz;

    /* Conversion between data items and bytes */
    sz = cnt * data_width_byte[DriverCapabilities.data_width];

    // printf("[%s]: Read 0x%x (0x%x bytes)\n", __func__, addr, sz);

    if ((addr < NVT_NVSPIM_MEMORY_BASE) || ((addr + sz) > (NVT_NVSPIM_MEMORY_BASE + SPIM_DMM_SIZE)))
        return ARM_DRIVER_ERROR_PARAMETER;

    addr -= NVT_NVSPIM_MEMORY_BASE;

    while (sz)
    {
        u32Len = sz;
        if (u32Len > NVT_NVSPIM_PAGE_SIZE)
            u32Len = NVT_NVSPIM_PAGE_SIZE;

        NVT_NVSPIM_Read(addr, u32Len, data);

        addr += u32Len;
        data += u32Len;
        sz  -= u32Len;
    }

    return ARM_DRIVER_OK;
}

static int32_t ARM_Flash_ProgramData_SPIM(uint32_t addr, const void *data, uint32_t cnt)
{
    uint32_t u32Len;
    uint32_t sz;

    sz = cnt * data_width_byte[DriverCapabilities.data_width];

    // printf("[%s]: Write 0x%x (0x%x bytes)\n", __func__, addr, sz);

    if ((addr < NVT_NVSPIM_MEMORY_BASE) || ((addr + sz) > (NVT_NVSPIM_MEMORY_BASE + SPIM_DMM_SIZE)))
        return ARM_DRIVER_ERROR_PARAMETER;

    addr -= NVT_NVSPIM_MEMORY_BASE;

    while (sz)
    {
        u32Len = sz;
        if (u32Len > NVT_NVSPIM_PAGE_SIZE)
            u32Len = NVT_NVSPIM_PAGE_SIZE;

        if (NVT_NVSPIM_Program(addr, u32Len, (uint8_t *)data) != 0)
            return ARM_DRIVER_ERROR;

        addr += u32Len;
        data += u32Len;
        sz  -= u32Len;
    }

    return ARM_DRIVER_OK;
}

static int32_t ARM_Flash_EraseSector_SPIM(uint32_t addr)
{
    if ((addr < NVT_NVSPIM_MEMORY_BASE) || (addr > (NVT_NVSPIM_MEMORY_BASE + SPIM_DMM_SIZE)))
        return ARM_DRIVER_ERROR_PARAMETER;

    addr -= NVT_NVSPIM_MEMORY_BASE;

    if (NVT_NVSPIM_Erase(addr) != 0)
        return ARM_DRIVER_ERROR;

    return ARM_DRIVER_OK;
}

static ARM_FLASH_INFO * ARM_Flash_GetInfo_SPIM(void)
{
    return FLASH2_DEV->data;
}
#elif defined(USE_SPI)

static int32_t is_range_valid_SPI(uint32_t addr)
{
    return is_range_valid_internal(&ARM_FLASH2_DEV, addr);
}

static int32_t ARM_Flash_Initialize_SPI(ARM_Flash_SignalEvent_t cb_event)
{
    ARG_UNUSED(cb_event);

    if (DriverCapabilities.data_width >= DATA_WIDTH_ENUM_SIZE) {
        return ARM_DRIVER_ERROR;
    }

    if (NVT_NVSPI_Init() != 0) {
        return ARM_DRIVER_ERROR;
    }

    return ARM_DRIVER_OK;
}

static int32_t ARM_Flash_ReadData_SPI(uint32_t addr, void *data, uint32_t cnt)
{
    uint32_t u32Len;
    uint32_t sz;
    int32_t r;

    /* Conversion between data items and bytes */
    sz = cnt * data_width_byte[DriverCapabilities.data_width];

    // printf("[%s]: Read 0x%x (0x%x bytes)\n", __func__, addr, sz);

    r = is_range_valid_SPI(addr);
    if (r != 0)
    {
        return ARM_DRIVER_ERROR_PARAMETER;
    }

    addr -= NVT_NVSPI_MEMORY_BASE;

    while (sz)
    {
        u32Len = sz;
        if (u32Len > NVT_NVSPI_PAGE_SIZE)
            u32Len = NVT_NVSPI_PAGE_SIZE;

        r = NVT_NVSPI_Read(addr, u32Len, data);
        if (r != 0)
        {
            return ARM_DRIVER_ERROR;
        }

        addr += u32Len;
        data += u32Len;
        sz  -= u32Len;
    }

    return ARM_DRIVER_OK;
}

static int32_t ARM_Flash_ProgramData_SPI(uint32_t addr, const void *data, uint32_t cnt)
{
    uint32_t u32Len;
    uint32_t sz;
    int32_t r;

    sz = cnt * data_width_byte[DriverCapabilities.data_width];

    // printf("[%s]: Write 0x%x (0x%x bytes)\n", __func__, addr, sz);

    r = is_range_valid_SPI(addr);
    if (r != 0)
    {
        return ARM_DRIVER_ERROR_PARAMETER;
    }

    addr -= NVT_NVSPI_MEMORY_BASE;

    while (sz)
    {
        u32Len = sz;
        if (u32Len > NVT_NVSPI_PAGE_SIZE)
            u32Len = NVT_NVSPI_PAGE_SIZE;

        if (NVT_NVSPI_Program(addr, u32Len, (uint8_t *)data) != 0)
        {
            return ARM_DRIVER_ERROR;
        }
        if (NVT_NVSPI_WaitReady() < 0)
        {
            return ARM_DRIVER_ERROR;
        }

        addr += u32Len;
        data += u32Len;
        sz  -= u32Len;
    }

    return ARM_DRIVER_OK;
}

static int32_t ARM_Flash_EraseSector_SPI(uint32_t addr)
{
    int32_t r;

    r = is_range_valid_SPI(addr);
    if (r != 0)
    {
        return ARM_DRIVER_ERROR_PARAMETER;
    }

    addr -= NVT_NVSPI_MEMORY_BASE;

    if (NVT_NVSPI_Erase(addr) != 0)
    {
        return ARM_DRIVER_ERROR;
    }
    if (NVT_NVSPI_WaitReady() < 0)
    {
        return ARM_DRIVER_ERROR;
    }

    return ARM_DRIVER_OK;
}

static ARM_FLASH_INFO * ARM_Flash_GetInfo_SPI(void)
{
    return FLASH2_DEV->data;
}
#elif defined(USE_QSPI)
static int32_t is_range_valid_QSPI(uint32_t addr)
{
    return is_range_valid_internal(&ARM_FLASH2_DEV, addr);
}

static int32_t ARM_Flash_Initialize_QSPI(ARM_Flash_SignalEvent_t cb_event)
{
    ARG_UNUSED(cb_event);

    if (DriverCapabilities.data_width >= DATA_WIDTH_ENUM_SIZE) {
        return ARM_DRIVER_ERROR;
    }

    if (NVT_NVQSPI_Init() != 0) {
        return ARM_DRIVER_ERROR;
    }

    return ARM_DRIVER_OK;
}

static int32_t ARM_Flash_ReadData_QSPI(uint32_t addr, void *data, uint32_t cnt)
{
    uint32_t u32Len;
    uint32_t sz;
    int32_t r;

    /* Conversion between data items and bytes */
    sz = cnt * data_width_byte[DriverCapabilities.data_width];

    // printf("[%s]: Read 0x%x (0x%x bytes)\n", __func__, addr, sz);

    r = is_range_valid_QSPI(addr);
    if (r != 0)
    {
        return ARM_DRIVER_ERROR_PARAMETER;
    }

    addr -= NVT_NVQSPI_MEMORY_BASE;

    while (sz)
    {
        u32Len = sz;
        if (u32Len > NVT_NVQSPI_PAGE_SIZE)
            u32Len = NVT_NVQSPI_PAGE_SIZE;

        r = NVT_NVQSPI_Read(addr, u32Len, data);
        if (r != 0)
        {
            return ARM_DRIVER_ERROR;
        }

        addr += u32Len;
        data += u32Len;
        sz  -= u32Len;
    }

    return ARM_DRIVER_OK;
}

static int32_t ARM_Flash_ProgramData_QSPI(uint32_t addr, const void *data, uint32_t cnt)
{
    uint32_t u32Len;
    uint32_t sz;
    int32_t r;

    sz = cnt * data_width_byte[DriverCapabilities.data_width];

    // printf("[%s]: Write 0x%x (0x%x bytes)\n", __func__, addr, sz);

    r = is_range_valid_QSPI(addr);
    if (r != 0)
    {
        return ARM_DRIVER_ERROR_PARAMETER;
    }

    addr -= NVT_NVQSPI_MEMORY_BASE;

    while (sz)
    {
        u32Len = sz;
        if (u32Len > NVT_NVQSPI_PAGE_SIZE)
            u32Len = NVT_NVQSPI_PAGE_SIZE;

        if (NVT_NVQSPI_Program(addr, u32Len, (uint8_t *)data) != 0)
        {
            return ARM_DRIVER_ERROR;
        }
        if (NVT_NVQSPI_WaitReady() < 0)
        {
            return ARM_DRIVER_ERROR;
        }

        addr += u32Len;
        data += u32Len;
        sz  -= u32Len;
    }

    return ARM_DRIVER_OK;
}

static int32_t ARM_Flash_EraseSector_QSPI(uint32_t addr)
{
    int32_t r;

    r = is_range_valid_QSPI(addr);
    if (r != 0)
    {
        return ARM_DRIVER_ERROR_PARAMETER;
    }

    addr -= NVT_NVQSPI_MEMORY_BASE;

    if (NVT_NVQSPI_Erase(addr) != 0)
    {
        return ARM_DRIVER_ERROR;
    }
    if (NVT_NVQSPI_WaitReady() < 0)
    {
        return ARM_DRIVER_ERROR;
    }

    return ARM_DRIVER_OK;
}

static ARM_FLASH_INFO * ARM_Flash_GetInfo_QSPI(void)
{
    return FLASH2_DEV->data;
}
#endif
#endif

ARM_DRIVER_FLASH Driver_FLASH0 =
{
    ARM_Flash_GetVersion,
    ARM_Flash_GetCapabilities,
    ARM_Flash_Initialize,
    ARM_Flash_Uninitialize,
    ARM_Flash_PowerControl,
    ARM_Flash_ReadData_APROM,
    ARM_Flash_ProgramData_APROM,
    ARM_Flash_EraseSector_APROM,
    NULL,
    ARM_Flash_GetStatus,
    ARM_Flash_GetInfo_APROM
};

ARM_DRIVER_FLASH Driver_FLASH1 =
{
    ARM_Flash_GetVersion,
    ARM_Flash_GetCapabilities,
    ARM_Flash_Initialize,
    ARM_Flash_Uninitialize,
    ARM_Flash_PowerControl,
    ARM_Flash_ReadData_LDROM,
    ARM_Flash_ProgramData_LDROM,
    ARM_Flash_EraseSector_LDROM,
    NULL,
    ARM_Flash_GetStatus,
    ARM_Flash_GetInfo_LDROM
};

#ifdef SECONDARY_SLOT_IN_SPI_FLASH
#if defined(USE_SPIM)
ARM_DRIVER_FLASH Driver_FLASH2 =
{
    ARM_Flash_GetVersion,
    ARM_Flash_GetCapabilities,
    ARM_Flash_Initialize_SPIM,
    ARM_Flash_Uninitialize,
    ARM_Flash_PowerControl,
    ARM_Flash_ReadData_SPIM,
    ARM_Flash_ProgramData_SPIM,
    ARM_Flash_EraseSector_SPIM,
    NULL,
    ARM_Flash_GetStatus,
    ARM_Flash_GetInfo_SPIM
};
#elif defined(USE_SPI)
ARM_DRIVER_FLASH Driver_FLASH2 =
{
    ARM_Flash_GetVersion,
    ARM_Flash_GetCapabilities,
    ARM_Flash_Initialize_SPI,
    ARM_Flash_Uninitialize,
    ARM_Flash_PowerControl,
    ARM_Flash_ReadData_SPI,
    ARM_Flash_ProgramData_SPI,
    ARM_Flash_EraseSector_SPI,
    NULL,
    ARM_Flash_GetStatus,
    ARM_Flash_GetInfo_SPI
};
#elif defined(USE_QSPI)
ARM_DRIVER_FLASH Driver_FLASH2 =
{
    ARM_Flash_GetVersion,
    ARM_Flash_GetCapabilities,
    ARM_Flash_Initialize_QSPI,
    ARM_Flash_Uninitialize,
    ARM_Flash_PowerControl,
    ARM_Flash_ReadData_QSPI,
    ARM_Flash_ProgramData_QSPI,
    ARM_Flash_EraseSector_QSPI,
    NULL,
    ARM_Flash_GetStatus,
    ARM_Flash_GetInfo_QSPI
};
#endif
#endif
