#include "NuMicro.h"
#include "nvspi_drv.h"

__STATIC_INLINE void wait_SPI_IS_BUSY(SPI_T *spi)
{
    uint32_t u32TimeOutCnt = SystemCoreClock; /* 1 second time-out */

    while(SPI_IS_BUSY(spi))
    {
        if(--u32TimeOutCnt == 0)
        {
            printf("Wait for SPI time-out!\n");
            break;
        }
    }
}

static uint8_t SpiFlash_ReadStatusReg(void)
{
    // /CS: active
    SPI_SET_SS_LOW(SPI_FLASH_PORT);

    // send Command: 0x05, Read status register
    SPI_WRITE_TX(SPI_FLASH_PORT, 0x05);

    // read status
    SPI_WRITE_TX(SPI_FLASH_PORT, 0x00);

    // wait tx finish
    wait_SPI_IS_BUSY(SPI_FLASH_PORT);

    // /CS: de-active
    SPI_SET_SS_HIGH(SPI_FLASH_PORT);

    // skip first rx data
    SPI_READ_RX(SPI_FLASH_PORT);

    return (SPI_READ_RX(SPI_FLASH_PORT) & 0xff);
}

int32_t NVT_NVSPI_WaitReady(void)
{
    uint8_t u8ReturnValue;
    uint32_t u32TimeOutCnt = SystemCoreClock; /* 1 second time-out */

    do
    {
        if(--u32TimeOutCnt == 0)
        {
            printf("Wait for SPI time-out!\n");
            return -1;
        }

        u8ReturnValue = SpiFlash_ReadStatusReg();
        u8ReturnValue = u8ReturnValue & 1;
    }
    while(u8ReturnValue != 0); // check the BUSY bit

    return 0;
}

uint32_t SpiFlash_ReadJedecID(void)
{
    uint8_t u8RxData[4], u8IDCnt = 0;

    // /CS: active
    SPI_SET_SS_LOW(SPI_FLASH_PORT);

    // send Command: 0x9F, Read JEDEC ID
    SPI_WRITE_TX(SPI_FLASH_PORT, 0x9F);

    // receive 32-bit
    SPI_WRITE_TX(SPI_FLASH_PORT, 0x00);
    SPI_WRITE_TX(SPI_FLASH_PORT, 0x00);
    SPI_WRITE_TX(SPI_FLASH_PORT, 0x00);
    SPI_WRITE_TX(SPI_FLASH_PORT, 0x00);

    // wait tx finish
    wait_SPI_IS_BUSY(SPI_FLASH_PORT);

    // /CS: de-active
    SPI_SET_SS_HIGH(SPI_FLASH_PORT);

    while(!SPI_GET_RX_FIFO_EMPTY_FLAG(SPI_FLASH_PORT))
        u8RxData[u8IDCnt++] = (uint8_t)SPI_READ_RX(SPI_FLASH_PORT);

    return (uint32_t)((u8RxData[1] << 16) | (u8RxData[2] << 8) | u8RxData[3]);
}

int32_t NVT_NVSPI_Init(void)
{
    uint32_t id;

    SYS_UnlockReg();

    CLK_EnableModuleClock(GPA_MODULE);

    CLK_EnableModuleClock(SPI2_MODULE);

    CLK_SetModuleClock(SPI2_MODULE, CLK_CLKSEL3_SPI2SEL_PCLK1, MODULE_NoMsk);

    SET_SPI2_MOSI_PA8();
    SET_SPI2_MISO_PA9();
    SET_SPI2_CLK_PA10();
    SET_SPI2_SS_PA11();
    
    /* Enable SPI clock pin (PA10) schmitt trigger */
    PA->SMTEN |= GPIO_SMTEN_SMTEN10_Msk;

    /* Enable SPI I/O high slew rate */
    GPIO_SetSlewCtl(PA, BIT8 | BIT9 | BIT10 | BIT11, GPIO_SLEWCTL_HIGH);

    /* Configure SPI_FLASH_PORT as a master, MSB first, 8-bit transaction, SPI Mode-0 timing, clock is 2MHz */
    SPI_Open(SPI_FLASH_PORT, SPI_MASTER, SPI_MODE_0, 8, 2000000);

    /* Disable auto SS function, control SS signal manually. */
    SPI_DisableAutoSS(SPI_FLASH_PORT);

    id = SpiFlash_ReadJedecID();

    switch(id)
    {
        case 0xEF4014:
        case 0xEF4015:
        case 0xEF4016:
        case 0xEF4017:
        case 0xEF4018:
            break;
        default:
            return -1;
    }

    return 0;
}

int32_t NVT_NVSPI_Read(uint32_t addr, uint32_t len, uint8_t *data)
{
    uint32_t u32Cnt;

    // /CS: active
    SPI_SET_SS_LOW(SPI_FLASH_PORT);

    // send Command: 0x03, Read data
    SPI_WRITE_TX(SPI_FLASH_PORT, 0x03);

    // send 24-bit start address
    SPI_WRITE_TX(SPI_FLASH_PORT, (addr >> 16) & 0xFF);
    SPI_WRITE_TX(SPI_FLASH_PORT, (addr >> 8)  & 0xFF);
    SPI_WRITE_TX(SPI_FLASH_PORT, addr       & 0xFF);

    wait_SPI_IS_BUSY(SPI_FLASH_PORT);
    // clear RX buffer
    SPI_ClearRxFIFO(SPI_FLASH_PORT);

    // read data
    for(u32Cnt = 0; u32Cnt < len; u32Cnt++)
    {
        SPI_WRITE_TX(SPI_FLASH_PORT, 0x00);
        wait_SPI_IS_BUSY(SPI_FLASH_PORT);
        data[u32Cnt] = (uint8_t)SPI_READ_RX(SPI_FLASH_PORT);
    }

    // wait tx finish
    wait_SPI_IS_BUSY(SPI_FLASH_PORT);

    // /CS: de-active
    SPI_SET_SS_HIGH(SPI_FLASH_PORT);

    return 0;
}

int32_t NVT_NVSPI_Program(uint32_t addr, uint32_t len, uint8_t *buf)
{
    uint32_t i;

    // /CS: active
    SPI_SET_SS_LOW(SPI_FLASH_PORT);

    // send Command: 0x06, Write enable
    SPI_WRITE_TX(SPI_FLASH_PORT, 0x06);

    // wait tx finish
    wait_SPI_IS_BUSY(SPI_FLASH_PORT);

    // /CS: de-active
    SPI_SET_SS_HIGH(SPI_FLASH_PORT);


    // /CS: active
    SPI_SET_SS_LOW(SPI_FLASH_PORT);

    // send Command: 0x02, Page program
    SPI_WRITE_TX(SPI_FLASH_PORT, 0x02);

    // send 24-bit start address
    SPI_WRITE_TX(SPI_FLASH_PORT, (addr >> 16) & 0xFF);
    SPI_WRITE_TX(SPI_FLASH_PORT, (addr >> 8)  & 0xFF);
    SPI_WRITE_TX(SPI_FLASH_PORT,  addr        & 0xFF);

    // write data
    while(1)
    {
        if(!SPI_GET_TX_FIFO_FULL_FLAG(SPI_FLASH_PORT))
        {
            for (i = 0; i < len; i++)
            {
                SPI_WRITE_TX(SPI_FLASH_PORT, buf[len]);
            }
        }
    }

    // wait tx finish
    wait_SPI_IS_BUSY(SPI_FLASH_PORT);

    SPI_ClearRxFIFO(SPI_FLASH_PORT);

    // /CS: de-active
    SPI_SET_SS_HIGH(SPI_FLASH_PORT);


    return 0;
}

int32_t NVT_NVSPI_Erase(uint32_t addr)
{
    // /CS: active
    SPI_SET_SS_LOW(SPI_FLASH_PORT);

    // send Command: 0x06, Write enable
    SPI_WRITE_TX(SPI_FLASH_PORT, 0x06);

    // wait tx finish
    wait_SPI_IS_BUSY(SPI_FLASH_PORT);

    // /CS: de-active
    SPI_SET_SS_HIGH(SPI_FLASH_PORT);

    // /CS: active
    SPI_SET_SS_LOW(SPI_FLASH_PORT);

    // send Command: 0x20, sector erase
    SPI_WRITE_TX(SPI_FLASH_PORT, 0x20);

    // send 24-bit address
    SPI_WRITE_TX(SPI_FLASH_PORT, (addr >> 16) & 0xFF);
    SPI_WRITE_TX(SPI_FLASH_PORT, (addr >> 8)  & 0xFF);
    SPI_WRITE_TX(SPI_FLASH_PORT,  addr        & 0xFF);

    // wait tx finish
    wait_SPI_IS_BUSY(SPI_FLASH_PORT);

    SPI_ClearRxFIFO(SPI_FLASH_PORT);

    // /CS: de-active
    SPI_SET_SS_HIGH(SPI_FLASH_PORT);

    return 0;
}
