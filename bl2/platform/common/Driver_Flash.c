#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "Driver_Flash.h"
#include "NuMicro.h"
#include "fmc_drv.h"
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

static int32_t is_range_valid(struct arm_flash_dev_t *flash_dev,
                              uint32_t offset)
{
    uint32_t flash_size = 0;
    int32_t rc = 0;

    flash_size = (flash_dev->data->sector_count * flash_dev->data->sector_size);

    if(offset >= flash_size)
    {
        rc = -1;
    }
    return rc;
}

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
    .sector_count = FMC_APROM_SIZE / FMC_FLASH_PAGE_SIZE,
    .sector_size  = FMC_FLASH_PAGE_SIZE,
    .page_size    = FMC_FLASH_PAGE_SIZE,
    .program_unit = 8,
    .erased_value = 0xFF
};

static struct arm_flash_dev_t ARM_FLASH0_DEV =
{
    .memory_base = FMC_APROM_BASE,
    .data        = &(ARM_FLASH0_DEV_DATA)
};

struct arm_flash_dev_t *FLASH0_DEV = &ARM_FLASH0_DEV;

/*
 * Functions
 */

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

    FMC_Open();
    FMC_ENABLE_AP_UPDATE();
    FMC_ENABLE_LD_UPDATE();

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


static int32_t ARM_Flash_EraseSector(uint32_t addr)
{
    volatile uint32_t mem_base = FLASH0_DEV->memory_base;
    uint32_t start_addr = mem_base + addr;
    uint32_t rc = 0;
    int32_t ret;

    rc  = is_range_valid(FLASH0_DEV, addr);
    rc |= is_sector_aligned(FLASH0_DEV, addr);
    if(rc != 0)
    {
        return ARM_DRIVER_ERROR_PARAMETER;
    }

    ret = NVT_FMC_Erase(addr);
    if (ret != NVT_FMC_OK)
    {
        return ARM_DRIVER_ERROR;
    }

    return ARM_DRIVER_OK;
}

static int32_t ARM_Flash_ReadData(uint32_t addr, void *data, uint32_t cnt)
{
    uint32_t start_addr = FLASH0_DEV->memory_base + addr;
    int32_t rc = 0;
    uint8_t* pu8;
    uint32_t i, taddr;

    /* Conversion between data items and bytes */
    cnt *= data_width_byte[DriverCapabilities.data_width];

    /* Check flash memory boundaries */
    rc = is_range_valid(FLASH0_DEV, addr + cnt);
    if (rc != 0) {
        return ARM_DRIVER_ERROR_PARAMETER;
    }

    pu8 = (uint8_t*)data;

    for(i = 0; i < cnt; i++)
    {
        taddr = start_addr + i;
        pu8[i] = *(uint8_t*)taddr;
    }

    cnt /= data_width_byte[DriverCapabilities.data_width];
    return cnt;
}

static int32_t ARM_Flash_ProgramData(uint32_t addr, const void *data, uint32_t cnt)
{
    volatile uint32_t mem_base = FLASH0_DEV->memory_base;
    uint32_t start_addr = mem_base + addr;
    int32_t rc = 0;
    uint32_t dst_offst;
    uint32_t src_offst;
    int32_t ret;
    uint64_t dword;

    /* Conversion between data items and bytes */
    cnt *= data_width_byte[DriverCapabilities.data_width];

    /* Check flash memory boundaries and alignment with minimal write size */
    rc  = is_range_valid(FLASH0_DEV, addr + cnt);
    rc |= is_write_aligned(FLASH0_DEV, addr);
    rc |= is_write_aligned(FLASH0_DEV, cnt);
    if(rc != 0)
    {
        return ARM_DRIVER_ERROR_PARAMETER;
    }

    src_offst = 0;
    for (dst_offst = 0; dst_offst < cnt; dst_offst += sizeof(dword))
    {
        /* Create local copy to avoid unaligned access */
        memcpy(&dword, data + src_offst, sizeof(dword));

        ret = NVT_FMC_Program(start_addr + dst_offst, dword);
        if (ret != 0)
        {
            return ARM_DRIVER_ERROR;
        }

        src_offst += sizeof(dword);
    }

    cnt /= data_width_byte[DriverCapabilities.data_width];

    return cnt;
}

static int32_t ARM_Flash_EraseChip(void)
{
    uint32_t i;
    uint32_t addr = FLASH0_DEV->memory_base;
    int32_t rc = ARM_DRIVER_ERROR_UNSUPPORTED;
    int32_t ret;

    /* Check driver capability erase_chip bit */
    if(DriverCapabilities.erase_chip == 1)
    {
        for(i = 0; i < FLASH0_DEV->data->sector_count; i++)
        {
            ret = NVT_FMC_Erase(addr);
            if (ret != NVT_FMC_OK)
            {
                rc = ARM_DRIVER_ERROR;
                goto finish;
            }
            addr += FLASH0_DEV->data->sector_size;
        }
    }

    rc = ARM_DRIVER_OK;
finish:
    return rc;
}

static ARM_FLASH_STATUS ARM_Flash_GetStatus(void)
{
    return FlashStatus;
}

static ARM_FLASH_INFO * ARM_Flash_GetInfo(void)
{
    return FLASH0_DEV->data;
}

ARM_DRIVER_FLASH Driver_FLASH0 =
{
    ARM_Flash_GetVersion,
    ARM_Flash_GetCapabilities,
    ARM_Flash_Initialize,
    ARM_Flash_Uninitialize,
    ARM_Flash_PowerControl,
    ARM_Flash_ReadData,
    ARM_Flash_ProgramData,
    ARM_Flash_EraseSector,
    ARM_Flash_EraseChip,
    ARM_Flash_GetStatus,
    ARM_Flash_GetInfo
};
