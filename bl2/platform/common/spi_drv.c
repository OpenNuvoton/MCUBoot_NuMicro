#include "nvspi_drv.h"
#include "NuMicro.h"

static void NVT_SPIM_IO_Write(uint8_t u8Data)
{
    SPIM->TX[0] = u8Data;

    SPIM_SET_GO();

    SPIM_WAIT_FREE();
}

static uint8_t NVT_SPIM_IO_Read(void)
{
    SPIM_SET_GO();

    SPIM_WAIT_FREE();

    return (SPIM->RX[0] & 0xFF);
}

static uint32_t SPIFLASH_ReadID(void)
{
    uint8_t u8ID0, u8ID1, u8ID2;

    SPIM_SET_SS_EN(1);                          // CS is   active (Low)

    SPIM->CTL0 = (SPIM->CTL0 & ~(SPIM_CTL0_OPMODE_Msk | SPIM_CTL0_QDIODIR_Msk)) | ((SPIM_CTL0_OPMODE_IO) | (1 << SPIM_CTL0_QDIODIR_Pos));

    NVT_SPIM_IO_Write(OPCODE_RDID);                 // Send Read ID instruction

    SPIM->CTL0 = (SPIM->CTL0 & ~(SPIM_CTL0_OPMODE_Msk | SPIM_CTL0_QDIODIR_Msk)) | ((SPIM_CTL0_OPMODE_IO) | (0 << SPIM_CTL0_QDIODIR_Pos));

    u8ID2 = NVT_SPIM_IO_Read();                     // Read a byte from the FLASH
    u8ID1 = NVT_SPIM_IO_Read();                     // Read a byte from the FLASH
    u8ID0 = NVT_SPIM_IO_Read();                     // Read a byte from the FLASH

    SPIM_SET_SS_EN(0);                          // CS is inactive (High)

    return ((u8ID2 << 16) | (u8ID1 << 8) | u8ID0);
}

static void NVT_SPIM_DMA_Write(uint32_t u32Addr, uint32_t u32Size, uint8_t *pu8Buf)
{
    SPIM->CTL0 = (SPIM->CTL0 & ~(SPIM_CTL0_CMDCODE_Msk | SPIM_CTL0_OPMODE_Msk)) | (CMD_NORMAL_PAGE_PROGRAM | SPIM_CTL0_OPMODE_PAGEWRITE);

    SPIM->SRAMADDR = (uint32_t)pu8Buf;          // SRAM Memory Address

    SPIM->DMACNT = u32Size;                     // DMA Transfer Byte Count

    SPIM->FADDR = u32Addr;                      // SPI Flash Address

    SPIM_SET_GO();                              // Start Transfer

    SPIM_WAIT_FREE();                           // Wait until SPI transfer is finished
}

static void NVT_SPIM_DMA_Read(uint32_t u32Addr, uint32_t u32Size, uint8_t *pu8Buf)
{
    SPIM->CTL0 = (SPIM->CTL0 & ~(SPIM_CTL0_CMDCODE_Msk | SPIM_CTL0_OPMODE_Msk)) | (CMD_DMA_NORMAL_READ | SPIM_CTL0_OPMODE_PAGEREAD);

    SPIM->SRAMADDR = (uint32_t)pu8Buf;          // SRAM Memory Address

    SPIM->DMACNT = u32Size;                     // DMA Transfer Byte Count

    SPIM->FADDR = u32Addr;                      // SPI Flash Address

    SPIM_SET_GO();                              // Start Transfer

    SPIM_WAIT_FREE();                           // Wait until SPI transfer is finished
}

static uint8_t SPIFLASH_CheckStatus(uint8_t u8Flag, uint8_t u8Value)
{
    uint8_t u8Status;

    SPIM_SET_SS_EN(1);                          // CS is active (Low)

    SPIM->CTL0 = (SPIM->CTL0 & ~(SPIM_CTL0_OPMODE_Msk | SPIM_CTL0_QDIODIR_Msk)) | ((SPIM_CTL0_OPMODE_IO) | (1 << SPIM_CTL0_QDIODIR_Pos));

    NVT_SPIM_IO_Write(OPCODE_RDSR);                 // Send read status register

    SPIM->CTL0 = (SPIM->CTL0 & ~(SPIM_CTL0_OPMODE_Msk | SPIM_CTL0_QDIODIR_Msk)) | ((SPIM_CTL0_OPMODE_IO) | (0 << SPIM_CTL0_QDIODIR_Pos));

    do
    {
        u8Status = NVT_SPIM_IO_Read();              // Read status
    }
    while ((u8Status & u8Flag) != u8Value);     // Write in progress

    SPIM_SET_SS_EN(0);                          // CS is inactive (High)

    return u8Status;
}

static uint8_t SPIFLASH_WriteEnable(void)
{
    uint8_t u8Status;

    SPIM_SET_SS_EN(1);                          // CS is active (Low)

    SPIM->CTL0 = (SPIM->CTL0 & ~(SPIM_CTL0_OPMODE_Msk | SPIM_CTL0_QDIODIR_Msk)) | ((SPIM_CTL0_OPMODE_IO) | (1 << SPIM_CTL0_QDIODIR_Pos));

    NVT_SPIM_IO_Write(OPCODE_WREN);                 // Send write enable

    SPIM_SET_SS_EN(0);                          // CS is inactive (High)

    u8Status = SPIFLASH_CheckStatus((SR_WEL | SR_WIP), SR_WEL);

    return u8Status;
}

int32_t NVT_NVSPI_Init(uint32_t fnc)
{
    uint32_t u32Reg, u32JEDEC_ID;

    SYS->REGLCTL = 0x59;                            // Unlock System Control Registers
    SYS->REGLCTL = 0x16;
    SYS->REGLCTL = 0x88;

    if (SYS->REGLCTL != 1)
        return (1);                                  // Not able to unlock

    if ((CLK->STATUS & CLK_PWRCTL_HIRCEN_Msk) == 0)
    {
        CLK->PWRCTL |= CLK_PWRCTL_HIRCEN_Msk;           // Enable HIRC

        while (!(CLK->STATUS & CLK_STATUS_HIRCSTB_Msk));
    }

    CLK->CLKSEL0 |= (0x07UL << CLK_CLKSEL0_HCLKSEL_Pos);
    CLK->CLKDIV0 &= ~CLK_CLKDIV0_HCLKDIV_Msk;

    CLK->AHBCLK0 |= CLK_AHBCLK0_SPIMCKEN_Msk;

    SYS->IPRST0 |= SYS_IPRST0_SPIMRST_Msk;
    SYS->IPRST0 &= ~SYS_IPRST0_SPIMRST_Msk;

    SPIM->CTL0 = SPIM_CTL0_OPMODE_IO | SPIM_CTL0_BITMODE_SING | (1 << SPIM_CTL0_QDIODIR_Pos) | (0 << SPIM_CTL0_BURSTNUM_Pos) | ((8-1) << SPIM_CTL0_DWIDTH_Pos);

    SPIM->CTL1 = (SPIM->CTL1 & ~(SPIM_CTL1_DIVIDER_Msk | SPIM_CTL1_SSACTPOL_Msk)) | SPIM_CTL1_SS_Msk;

    u32Reg = (fnc >> 24) & 0xFF;

    if (u32Reg == 0xAD)
    {
        uint32_t u32SS_SEL, u32CLK_SEL, u32MISO_SEL, u32MOSI_SEL;

        u32SS_SEL   = (fnc >>  0) & 0x0F;
        u32CLK_SEL  = (fnc >>  4) & 0x0F;
        u32MISO_SEL = (fnc >>  8) & 0x0F;
        u32MOSI_SEL = (fnc >> 12) & 0x0F;

        switch (u32SS_SEL)
        {
            case 0:
                SYS->GPA_MFP0 = (SYS->GPA_MFP0 & ~SYS_GPA_MFP0_PA3MFP_Msk) | (0x2 << SYS_GPA_MFP0_PA3MFP_Pos);
                break;
            case 1:
                SYS->GPC_MFP0 = (SYS->GPC_MFP0 & ~SYS_GPC_MFP0_PC3MFP_Msk) | (0x3 << SYS_GPC_MFP0_PC3MFP_Pos);
                break;
            case 2:
                SYS->GPE_MFP1 = (SYS->GPE_MFP1 & ~SYS_GPE_MFP1_PE5MFP_Msk) | (0x4 << SYS_GPE_MFP1_PE5MFP_Pos);
                break;
            case 3:
                SYS->GPG_MFP2 = (SYS->GPG_MFP2 & ~SYS_GPG_MFP2_PG11MFP_Msk) | (0x4 << SYS_GPG_MFP2_PG11MFP_Pos);
                break;
            case 4:
                SYS->GPI_MFP3 = (SYS->GPI_MFP3 & ~SYS_GPI_MFP3_PI12MFP_Msk) | (0x3 << SYS_GPI_MFP3_PI12MFP_Pos);
                break;
            case 5:
                SYS->GPJ_MFP2 = (SYS->GPJ_MFP2 & ~SYS_GPJ_MFP2_PJ8MFP_Msk) | (0x4 << SYS_GPJ_MFP2_PJ8MFP_Pos);
                break;
            default:
                return (1);
        }

        switch (u32CLK_SEL)
        {
            case 0:
                SYS->GPA_MFP0 = (SYS->GPA_MFP0 & ~SYS_GPA_MFP0_PA2MFP_Msk) | (0x2 << SYS_GPA_MFP0_PA2MFP_Pos);
                break;
            case 1:
                SYS->GPC_MFP0 = (SYS->GPC_MFP0 & ~SYS_GPC_MFP0_PC2MFP_Msk) | (0x3 << SYS_GPC_MFP0_PC2MFP_Pos);
                break;
            case 2:
                SYS->GPE_MFP1 = (SYS->GPE_MFP1 & ~SYS_GPE_MFP1_PE4MFP_Msk) | (0x4 << SYS_GPE_MFP1_PE4MFP_Pos);
                break;
            case 3:
                SYS->GPG_MFP3 = (SYS->GPG_MFP3 & ~SYS_GPG_MFP3_PG12MFP_Msk) | (0x4 << SYS_GPG_MFP3_PG12MFP_Pos);
                break;
            case 4:
                SYS->GPJ_MFP0 = (SYS->GPJ_MFP0 & ~SYS_GPJ_MFP0_PJ0MFP_Msk) | (0x3 << SYS_GPJ_MFP0_PJ0MFP_Pos);
                break;
            case 5:
                SYS->GPJ_MFP3 = (SYS->GPJ_MFP3 & ~SYS_GPJ_MFP3_PJ12MFP_Msk) | (0x4 << SYS_GPJ_MFP3_PJ12MFP_Pos);
                break;
            default:
                return (1);
        }

        switch (u32MISO_SEL)
        {
            case 0:
                SYS->GPA_MFP0 = (SYS->GPA_MFP0 & ~SYS_GPA_MFP0_PA1MFP_Msk) | (0x2 << SYS_GPA_MFP0_PA1MFP_Pos);
                break;
            case 1:
                SYS->GPC_MFP0 = (SYS->GPC_MFP0 & ~SYS_GPC_MFP0_PC1MFP_Msk) | (0x3 << SYS_GPC_MFP0_PC1MFP_Pos);
                break;
            case 2:
                SYS->GPE_MFP0 = (SYS->GPE_MFP0 & ~SYS_GPE_MFP0_PE3MFP_Msk) | (0x4 << SYS_GPE_MFP0_PE3MFP_Pos);
                break;
            case 3:
                SYS->GPG_MFP3 = (SYS->GPG_MFP3 & ~SYS_GPG_MFP3_PG13MFP_Msk) | (0x4 << SYS_GPG_MFP3_PG13MFP_Pos);
                break;
            case 4:
                SYS->GPI_MFP3 = (SYS->GPI_MFP3 & ~SYS_GPI_MFP3_PI13MFP_Msk) | (0x3 << SYS_GPI_MFP3_PI13MFP_Pos);
                break;
            case 5:
                SYS->GPJ_MFP2 = (SYS->GPJ_MFP2 & ~SYS_GPJ_MFP2_PJ9MFP_Msk) | (0x4 << SYS_GPJ_MFP2_PJ9MFP_Pos);
                break;
            default:
                return (1);
        }

        switch (u32MOSI_SEL)
        {
            case 0:
                SYS->GPA_MFP0 = (SYS->GPA_MFP0 & ~SYS_GPA_MFP0_PA0MFP_Msk) | (0x2 << SYS_GPA_MFP0_PA0MFP_Pos);
                break;
            case 1:
                SYS->GPC_MFP0 = (SYS->GPC_MFP0 & ~SYS_GPC_MFP0_PC0MFP_Msk) | (0x3 << SYS_GPC_MFP0_PC0MFP_Pos);
                break;
            case 2:
                SYS->GPE_MFP0 = (SYS->GPE_MFP0 & ~SYS_GPE_MFP0_PE2MFP_Msk) | (0x4 << SYS_GPE_MFP0_PE2MFP_Pos);
                break;
            case 3:
                SYS->GPG_MFP3 = (SYS->GPG_MFP3 & ~SYS_GPG_MFP3_PG14MFP_Msk) | (0x4 << SYS_GPG_MFP3_PG14MFP_Pos);
                break;
            case 4:
                SYS->GPJ_MFP0 = (SYS->GPJ_MFP0 & ~SYS_GPJ_MFP0_PJ1MFP_Msk) | (0x3 << SYS_GPJ_MFP0_PJ1MFP_Pos);
                break;
            case 5:
                SYS->GPJ_MFP3 = (SYS->GPJ_MFP3 & ~SYS_GPJ_MFP3_PJ13MFP_Msk) | (0x4 << SYS_GPJ_MFP3_PJ13MFP_Pos);
                break;
            case 6:
                SYS->GPA_MFP3 = (SYS->GPA_MFP3 & ~SYS_GPA_MFP3_PA15MFP_Msk) | (0x4 << SYS_GPA_MFP3_PA15MFP_Pos);
                break;
            default:
                return (1);
        }
    }
    else
    {
        SYS->GPA_MFP0 = (SYS->GPA_MFP0 & ~(SYS_GPA_MFP0_PA0MFP_Msk | SYS_GPA_MFP0_PA1MFP_Msk | SYS_GPA_MFP0_PA2MFP_Msk | SYS_GPA_MFP0_PA3MFP_Msk)) | ((0x2 << SYS_GPA_MFP0_PA0MFP_Pos) | (0x2 << SYS_GPA_MFP0_PA1MFP_Pos) | (0x2 << SYS_GPA_MFP0_PA2MFP_Pos) | (0x2 << SYS_GPA_MFP0_PA3MFP_Pos));
        SYS->GPA_MFP1 = (SYS->GPA_MFP1 & ~(SYS_GPA_MFP1_PA4MFP_Msk | SYS_GPA_MFP1_PA5MFP_Msk)) | ((0x2 << SYS_GPA_MFP1_PA4MFP_Pos) | (0x2 << SYS_GPA_MFP1_PA5MFP_Pos));
    }

    u32JEDEC_ID = SPIFLASH_ReadID();

    if ((u32JEDEC_ID == 0x000000) || (u32JEDEC_ID == 0xFFFFFF))
        return (1);

    return (0);
}

int32_t NVT_NVSPI_Read(uint32_t addr, uint32_t size, uint32_t *data)
{
    SPIM_SET_SS_EN(1);                              // CS is active (Low)

    NVT_SPIM_DMA_Read(addr, size, (uint8_t *)data);

    SPIM_SET_SS_EN(0);                              // CS is inactive (High)

    return (0);
}

int32_t NVT_NVSPI_Program(uint32_t addr, uint32_t size, uint8_t *buf)
{
    uint8_t u8Status;

    u8Status = SPIFLASH_WriteEnable();

    if ((u8Status & SR_WEL) == 0)
        return (1);

    SPIM_SET_SS_EN(1);                              // CS is active (Low)

    NVT_SPIM_DMA_Write(addr, size, buf);

    SPIM_SET_SS_EN(0);                              // CS is inactive (High)

    u8Status = SPIFLASH_CheckStatus(SR_WIP, 0);

    if (u8Status != 0)
        return (1);

    return (0);
}

int32_t NVT_NVSPI_Erase(uint32_t addr)
{
    uint8_t u8Status;

    SPIM_WAIT_FREE();

    u8Status = SPIFLASH_WriteEnable();

    if ((u8Status & SR_WEL) == 0)
        return (1);

    SPIM_SET_SS_EN(1);                              // CS is active (Low)

    SPIM->CTL0 = (SPIM->CTL0 & ~(SPIM_CTL0_OPMODE_Msk | SPIM_CTL0_QDIODIR_Msk)) | ((SPIM_CTL0_OPMODE_IO) | (1 << SPIM_CTL0_QDIODIR_Pos));

    NVT_SPIM_IO_Write(OPCODE_SE_4K);                    // Send Sector Erase instruction

    NVT_SPIM_IO_Write((addr >> 16) & 0xFF);
    NVT_SPIM_IO_Write((addr >>  8) & 0xFF);
    NVT_SPIM_IO_Write((addr      ) & 0xFF);

    SPIM_SET_SS_EN(0);                              // CS is inactive (High)

    u8Status = SPIFLASH_CheckStatus(SR_WIP, 0);

    return (0);
}
