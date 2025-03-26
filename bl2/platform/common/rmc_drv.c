#include "nvmc_drv.h"
#include <string.h>

static int32_t NVT_RMC_Init(void);
static int32_t NVT_RMC_Read(uint32_t addr, uint32_t *data);
static int32_t NVT_RMC_Program(uint32_t addr, volatile uint64_t dword);
static int32_t NVT_RMC_Erase(uint32_t addr);

NVT_NVMC_T NVT_RMC = 
{
    .init = NVT_RMC_Init,
    .read = NVT_RMC_Read,
    .program = NVT_RMC_Program,
    .erase = NVT_RMC_Erase,
};

static int32_t NVT_RMC_Init(void)
{
    RMC_Open();
    RMC_ENABLE_LD_UPDATE();
    RMC_ENABLE_AP_UPDATE();

    return NVT_NVMC_OK;
}

static int32_t NVT_RMC_Read(uint32_t addr, uint32_t *data)
{
    *data = RMC_Read(addr);
    if (g_RMC_i32ErrCode != 0)
    {
        return NVT_NVMC_ERROR;
    }

    return NVT_NVMC_OK;
}

static int32_t NVT_RMC_Program(uint32_t addr, volatile uint64_t dword)
{
    int32_t ret;
    uint32_t word = (dword & 0xFFFFFFFF);

    ret = RMC_Write(addr, word);
    if (ret == -1)
    {
        return NVT_NVMC_ERROR;
    }

    return NVT_NVMC_OK;
}

static int32_t NVT_RMC_Erase(uint32_t addr)
{
    int32_t ret;

    ret = RMC_Erase(addr);
    if (ret == -2)
    {
        return NVT_NVMC_PARAMETER;
    }
    else if (ret == -1)
    {
        return NVT_NVMC_ERROR;
    }

    return NVT_NVMC_OK;
}

int32_t NVT_NVMC_Init(void)
{
    return NVT_RMC.init();
}

int32_t NVT_NVMC_Read(uint32_t addr, uint32_t *data)
{
    return NVT_RMC.read(addr, data);
}

int32_t NVT_NVMC_Program(uint32_t addr, uint64_t dword)
{
    return NVT_RMC.program(addr, dword);
}

int32_t NVT_NVMC_Erase(uint32_t addr)
{
    return NVT_RMC.erase(addr);
}
