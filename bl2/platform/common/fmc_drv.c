#include "nvmc_drv.h"
#include <string.h>

static int32_t NVT_FMC_Init(void);
static int32_t NVT_FMC_Read(uint32_t addr, uint32_t *data);
static int32_t NVT_FMC_Program(uint32_t addr, volatile uint64_t dword);
static int32_t NVT_FMC_Erase(uint32_t addr);

NVT_NVMC_T NVT_FMC = 
{
    .init = NVT_FMC_Init,
    .read = NVT_FMC_Read,
    .program = NVT_FMC_Program,
    .erase = NVT_FMC_Erase,
};

static int32_t NVT_FMC_Init(void)
{
    FMC_Open();
    FMC_ENABLE_AP_UPDATE();
    FMC_ENABLE_LD_UPDATE();

    return NVT_NVMC_OK;
}

static int32_t NVT_FMC_Read(uint32_t addr, uint32_t *data)
{
    int32_t timeout_cnt;

    if (data == NULL)
    {
        return NVT_NVMC_PARAMETER;
    }

    FMC->ISPCMD = FMC_ISPCMD_READ;
    FMC->ISPADDR = addr;
    FMC->ISPTRG = FMC_ISPTRG_ISPGO_Msk;

    timeout_cnt = FMC_TIMEOUT_READ;

    while(FMC->ISPSTS & FMC_ISPSTS_ISPBUSY_Msk)
    {
        if(timeout_cnt-- <= 0)
        {
            return NVT_NVMC_TIMEOUT;
        }
    }

    if(FMC->ISPSTS & FMC_ISPSTS_ISPFF_Msk)
    {
        FMC->ISPSTS |= FMC_ISPSTS_ISPFF_Msk;
        return NVT_NVMC_ISPFF;
    }

    *data = FMC->ISPDAT;

    return NVT_NVMC_OK;
}

static int32_t NVT_FMC_Program(uint32_t addr, volatile uint64_t dword)
{
    int32_t timeout_cnt;
    uint8_t *src = (uint8_t *)dword;
    volatile uint64_t ullbuf;
    volatile uint32_t ulbuf;

    FMC->ISPCMD  = FMC_ISPCMD_PROGRAM_64;
    FMC->ISPADDR = addr;
    ulbuf = (dword & 0xFFFFFFFF);
    FMC->MPDAT0 = ulbuf;
    ullbuf = (dword & 0xFFFFFFFF00000000);
    ulbuf = ullbuf >> 32;
    FMC->MPDAT1 = ulbuf;
    FMC->ISPTRG  = FMC_ISPTRG_ISPGO_Msk;

    timeout_cnt = FMC_TIMEOUT_WRITE;
    while(FMC->ISPSTS & FMC_ISPSTS_ISPBUSY_Msk)
    {
        if(timeout_cnt-- <= 0)
        {
            return NVT_NVMC_TIMEOUT;
        }
    }

    if(FMC->ISPSTS & FMC_ISPSTS_ISPFF_Msk)
    {
        FMC->ISPSTS |= FMC_ISPSTS_ISPFF_Msk;
        return NVT_NVMC_ISPFF;
    }

    return NVT_NVMC_OK;
}

static int32_t NVT_FMC_Erase(uint32_t addr)
{
    int32_t timeout_cnt;

    FMC->ISPCMD = FMC_ISPCMD_PAGE_ERASE;
    FMC->ISPADDR = addr;
    FMC->ISPTRG = FMC_ISPTRG_ISPGO_Msk;

    timeout_cnt = FMC_TIMEOUT_ERASE;

    while(FMC->ISPSTS & FMC_ISPSTS_ISPBUSY_Msk)
    {
        if(timeout_cnt-- <= 0)
        {
            return NVT_NVMC_TIMEOUT;
        }
    }

    if(FMC->ISPSTS & FMC_ISPSTS_ISPFF_Msk)
    {
        FMC->ISPSTS |= FMC_ISPSTS_ISPFF_Msk;
        return NVT_NVMC_ISPFF;
    }

    return NVT_NVMC_OK;
}

int32_t NVT_NVMC_Init(void)
{
    return NVT_FMC.init();
}

int32_t NVT_NVMC_Read(uint32_t addr, uint32_t *data)
{
    return NVT_FMC.read(addr, data);
}

int32_t NVT_NVMC_Program(uint32_t addr, uint64_t dword)
{
    return NVT_FMC.program(addr, dword);
}

int32_t NVT_NVMC_Erase(uint32_t addr)
{
    return NVT_FMC.erase(addr);
}
