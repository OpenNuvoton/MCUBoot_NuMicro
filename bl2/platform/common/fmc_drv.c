#include "fmc_drv.h"
#include <string.h>

int32_t NVT_FMC_Read(uint32_t addr, uint32_t *data)
{
    int32_t timeout_cnt;

    if (data == NULL)
    {
        return NVT_FMC_PARAMETER;
    }

    FMC->ISPCMD = FMC_ISPCMD_READ;
    FMC->ISPADDR = addr;
    FMC->ISPTRG = FMC_ISPTRG_ISPGO_Msk;

    timeout_cnt = FMC_TIMEOUT_READ;

    while(FMC->ISPSTS & FMC_ISPSTS_ISPBUSY_Msk)
    {
        if(timeout_cnt-- <= 0)
        {
            return NVT_FMC_TIMEOUT;
        }
    }

    if(FMC->ISPSTS & FMC_ISPSTS_ISPFF_Msk)
    {
        FMC->ISPSTS |= FMC_ISPSTS_ISPFF_Msk;
        return NVT_FMC_ISPFF;
    }

    *data = FMC->ISPDAT;

    return NVT_FMC_OK;
}

int32_t NVT_FMC_Program(uint32_t addr, volatile uint64_t dword)
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
            return NVT_FMC_TIMEOUT;
        }
    }

    if(FMC->ISPSTS & FMC_ISPSTS_ISPFF_Msk)
    {
        FMC->ISPSTS |= FMC_ISPSTS_ISPFF_Msk;
        return NVT_FMC_ISPFF;
    }

    return NVT_FMC_OK;
}

int32_t NVT_FMC_Erase(uint32_t addr)
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
            return NVT_FMC_TIMEOUT;
        }
    }

    if(FMC->ISPSTS & FMC_ISPSTS_ISPFF_Msk)
    {
        FMC->ISPSTS |= FMC_ISPSTS_ISPFF_Msk;
        return NVT_FMC_ISPFF;
    }

    return NVT_FMC_OK;
}
