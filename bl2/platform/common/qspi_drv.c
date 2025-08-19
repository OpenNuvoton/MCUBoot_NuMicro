#include "nvqspi_drv.h"
#include "NuMicro.h"

__STATIC_INLINE void wait_QSPI_IS_BUSY(QSPI_T *qspi)
{
    uint32_t u32TimeOutCnt = SystemCoreClock; /* 1 second time-out */

    while(QSPI_IS_BUSY(qspi))
    {
        if(--u32TimeOutCnt == 0)
        {
            printf("Wait for QSPI time-out!\n");
            break;
        }
    }
}

static uint32_t SpiFlash_ReadJedecID(void)
{
    uint8_t u8RxData[4], u8IDCnt = 0;

    // /CS: active
    QSPI_SET_SS_LOW(QSPI_FLASH_PORT);

    // send Command: 0x9F, Read JEDEC ID
    QSPI_WRITE_TX(QSPI_FLASH_PORT, 0x9F);

    // receive 32-bit
    QSPI_WRITE_TX(QSPI_FLASH_PORT, 0x00);
    QSPI_WRITE_TX(QSPI_FLASH_PORT, 0x00);
    QSPI_WRITE_TX(QSPI_FLASH_PORT, 0x00);
    QSPI_WRITE_TX(QSPI_FLASH_PORT, 0x00);

    // wait tx finish
    wait_QSPI_IS_BUSY(QSPI_FLASH_PORT);

    // /CS: de-active
    QSPI_SET_SS_HIGH(QSPI_FLASH_PORT);

    while(!QSPI_GET_RX_FIFO_EMPTY_FLAG(QSPI_FLASH_PORT))
        u8RxData[u8IDCnt++] = (uint8_t)QSPI_READ_RX(QSPI_FLASH_PORT);

    return (uint32_t)((u8RxData[1] << 16) | (u8RxData[2] << 8) | u8RxData[3]);
}

static uint8_t SpiFlash_ReadStatusReg(void)
{
    uint8_t u8Val;

    QSPI_ClearRxFIFO(QSPI_FLASH_PORT);

    // /CS: active
    QSPI_SET_SS_LOW(QSPI_FLASH_PORT);

    // send Command: 0x05, Read status register
    QSPI_WRITE_TX(QSPI_FLASH_PORT, 0x05);

    // read status
    QSPI_WRITE_TX(QSPI_FLASH_PORT, 0x00);

    // wait tx finish
    wait_QSPI_IS_BUSY(QSPI_FLASH_PORT);

    // /CS: de-active
    QSPI_SET_SS_HIGH(QSPI_FLASH_PORT);

    // skip first rx data
    u8Val = (uint8_t)QSPI_READ_RX(QSPI_FLASH_PORT);
    u8Val = (uint8_t)QSPI_READ_RX(QSPI_FLASH_PORT);

    return u8Val;
}

uint8_t SpiFlash_ReadStatusReg2(void)
{
    uint8_t u8Val;

    QSPI_ClearRxFIFO(QSPI_FLASH_PORT);

    // /CS: active
    QSPI_SET_SS_LOW(QSPI_FLASH_PORT);

    // send Command: 0x35, Read status register
    QSPI_WRITE_TX(QSPI_FLASH_PORT, 0x35);

    // read status
    QSPI_WRITE_TX(QSPI_FLASH_PORT, 0x00);

    // wait tx finish
    wait_QSPI_IS_BUSY(QSPI_FLASH_PORT);

    // /CS: de-active
    QSPI_SET_SS_HIGH(QSPI_FLASH_PORT);

    // skip first rx data
    u8Val = (uint8_t)QSPI_READ_RX(QSPI_FLASH_PORT);
    u8Val = (uint8_t)QSPI_READ_RX(QSPI_FLASH_PORT);

    return u8Val;
}

static void SpiFlash_WriteStatusReg(uint8_t u8Value1, uint8_t u8Value2)
{
    // /CS: active
    QSPI_SET_SS_LOW(QSPI_FLASH_PORT);

    // send Command: 0x06, Write enable
    QSPI_WRITE_TX(QSPI_FLASH_PORT, 0x06);

    // wait tx finish
    wait_QSPI_IS_BUSY(QSPI_FLASH_PORT);

    // /CS: de-active
    QSPI_SET_SS_HIGH(QSPI_FLASH_PORT);

    ///////////////////////////////////////

    // /CS: active
    QSPI_SET_SS_LOW(QSPI_FLASH_PORT);

    // send Command: 0x01, Write status register
    QSPI_WRITE_TX(QSPI_FLASH_PORT, 0x01);

    // write status
    QSPI_WRITE_TX(QSPI_FLASH_PORT, u8Value1);
    QSPI_WRITE_TX(QSPI_FLASH_PORT, u8Value2);

    // wait tx finish
    wait_QSPI_IS_BUSY(QSPI_FLASH_PORT);

    // /CS: de-active
    QSPI_SET_SS_HIGH(QSPI_FLASH_PORT);
}

static int32_t SpiFlash_WaitReady(void)
{
    volatile uint8_t u8ReturnValue;
    uint32_t u32TimeOutCnt = SystemCoreClock; /* 1 second time-out */

    do
    {
        if(--u32TimeOutCnt == 0)
        {
            printf("Wait for QSPI time-out!\n");
            return -1;
        }

        u8ReturnValue = SpiFlash_ReadStatusReg();
        u8ReturnValue = u8ReturnValue & 1;
    }
    while(u8ReturnValue != 0); // check the BUSY bit

    return 0;
}

static void spiFlash_EnableQEBit(void)
{
    uint8_t u8Status1 = SpiFlash_ReadStatusReg();
    uint8_t u8Status2 = SpiFlash_ReadStatusReg2();

    u8Status2 |= 0x2;
    SpiFlash_WriteStatusReg(u8Status1, u8Status2);
    SpiFlash_WaitReady();
}

static void spiFlash_DisableQEBit(void)
{
    uint8_t u8Status1 = SpiFlash_ReadStatusReg();
    uint8_t u8Status2 = SpiFlash_ReadStatusReg2();

    u8Status2 &= ~0x2;
    SpiFlash_WriteStatusReg(u8Status1, u8Status2);
    SpiFlash_WaitReady();
}

static void D2D3_SwitchToNormalMode(void)
{
    SYS->GPA_MFP1 =  SYS->GPA_MFP1 & ~(SYS_GPA_MFP1_PA4MFP_Msk | SYS_GPA_MFP1_PA5MFP_Msk);
    GPIO_SetMode(PA, BIT4, GPIO_MODE_OUTPUT);
    GPIO_SetMode(PA, BIT5, GPIO_MODE_OUTPUT);
    PA4 = 1;
    PA5 = 1;
}

static void D2D3_SwitchToQuadMode(void)
{
    SET_QSPI0_MOSI1_PA4();
    SET_QSPI0_MISO1_PA5();
}

int32_t NVT_NVQSPI_Init(void)
{
    uint32_t u32ID;

    SYS_UnlockReg();

    CLK_EnableModuleClock(GPA_MODULE);

    CLK_EnableModuleClock(QSPI0_MODULE);

    CLK_SetModuleClock(QSPI0_MODULE, CLK_CLKSEL2_QSPI0SEL_PCLK0, MODULE_NoMsk);

    /* Setup QSPI0 multi-function pins */
    SET_QSPI0_MOSI0_PA0();
    SET_QSPI0_MISO0_PA1();
    SET_QSPI0_CLK_PA2();
    SET_QSPI0_SS_PA3();
    SET_QSPI0_MOSI1_PA4();
    SET_QSPI0_MISO1_PA5();

    /* Enable QSPI0 clock pin (PA2) schmitt trigger */
    PA->SMTEN |= GPIO_SMTEN_SMTEN2_Msk;

    /* Enable QSPI0 I/O high slew rate */
    GPIO_SetSlewCtl(PA, BIT0 | BIT1 | BIT2 | BIT3 | BIT4 | BIT5, GPIO_SLEWCTL_HIGH);

    D2D3_SwitchToNormalMode();

    /* Configure SPI_FLASH_PORT as a master, MSB first, 8-bit transaction, QSPI Mode-0 timing, clock is 2MHz */
    QSPI_Open(QSPI_FLASH_PORT, QSPI_MASTER, QSPI_MODE_0, 8, 2000000);

    /* Disable auto SS function, control SS signal manually. */
    QSPI_DisableAutoSS(QSPI_FLASH_PORT);

    u32ID = SpiFlash_ReadJedecID();

    switch(u32ID)
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


int32_t NVT_NVQSPI_Read(uint32_t addr, uint32_t len, uint8_t *data)
{
    uint32_t u32Cnt;

#if ENABLE_QUAD_MODE
    // enable quad mode
    spiFlash_EnableQEBit();
    // /CS: active
    QSPI_SET_SS_LOW(QSPI_FLASH_PORT);

    // Command: 0xEB, Fast Read quad data
    QSPI_WRITE_TX(QSPI_FLASH_PORT, 0xEB);

    wait_QSPI_IS_BUSY(QSPI_FLASH_PORT);

    // enable SPI quad IO mode and set direction to input
    D2D3_SwitchToQuadMode();
    QSPI_ENABLE_QUAD_OUTPUT_MODE(QSPI_FLASH_PORT);

    // send 24-bit start address
    QSPI_WRITE_TX(QSPI_FLASH_PORT, (addr >> 16) & 0xFF);
    QSPI_WRITE_TX(QSPI_FLASH_PORT, (addr >> 8)  & 0xFF);
    QSPI_WRITE_TX(QSPI_FLASH_PORT, addr       & 0xFF);

    // dummy byte
    QSPI_WRITE_TX(QSPI_FLASH_PORT, 0x00);
    QSPI_WRITE_TX(QSPI_FLASH_PORT, 0x00);
    QSPI_WRITE_TX(QSPI_FLASH_PORT, 0x00);

    wait_QSPI_IS_BUSY(QSPI_FLASH_PORT);
    QSPI_ENABLE_QUAD_INPUT_MODE(QSPI_FLASH_PORT);

    // clear RX buffer
    QSPI_ClearRxFIFO(QSPI_FLASH_PORT);

    // read data
    for(u32Cnt = 0; u32Cnt < len; u32Cnt++)
    {
        QSPI_WRITE_TX(QSPI_FLASH_PORT, 0x00);
        wait_QSPI_IS_BUSY(QSPI_FLASH_PORT);
        data[u32Cnt] = (uint8_t)QSPI_READ_RX(QSPI_FLASH_PORT);
    }

    // wait tx finish
    wait_QSPI_IS_BUSY(QSPI_FLASH_PORT);

    // /CS: de-active
    QSPI_SET_SS_HIGH(QSPI_FLASH_PORT);

    QSPI_DISABLE_QUAD_MODE(QSPI_FLASH_PORT);

    D2D3_SwitchToNormalMode();

    // disable quad mode
    spiFlash_DisableQEBit();
#else
    // /CS: active
    QSPI_SET_SS_LOW(QSPI_FLASH_PORT);

    // Command: 0x03, Read Data
    QSPI_WRITE_TX(QSPI_FLASH_PORT, 0x03);

    wait_QSPI_IS_BUSY(QSPI_FLASH_PORT);

    // send 24-bit start address
    QSPI_WRITE_TX(QSPI_FLASH_PORT, (addr >> 16) & 0xFF);
    QSPI_WRITE_TX(QSPI_FLASH_PORT, (addr >> 8)  & 0xFF);
    QSPI_WRITE_TX(QSPI_FLASH_PORT, addr       & 0xFF);

    wait_QSPI_IS_BUSY(QSPI_FLASH_PORT);

    // clear RX buffer
    QSPI_ClearRxFIFO(QSPI_FLASH_PORT);

    // read data
    for(u32Cnt = 0; u32Cnt < len; u32Cnt++)
    {
        QSPI_WRITE_TX(QSPI_FLASH_PORT, 0x00);
        wait_QSPI_IS_BUSY(QSPI_FLASH_PORT);
        data[u32Cnt] = (uint8_t)QSPI_READ_RX(QSPI_FLASH_PORT);
    }

    // wait tx finish
    wait_QSPI_IS_BUSY(QSPI_FLASH_PORT);

    // /CS: de-active
    QSPI_SET_SS_HIGH(QSPI_FLASH_PORT);
#endif

    return 0;
}

int32_t NVT_NVQSPI_Program(uint32_t addr, uint32_t len, uint8_t *buf)
{
    uint32_t i = 0;

    // /CS: active
    QSPI_SET_SS_LOW(QSPI_FLASH_PORT);

    // send Command: 0x06, Write enable
    QSPI_WRITE_TX(QSPI_FLASH_PORT, 0x06);

    // wait tx finish
    wait_QSPI_IS_BUSY(QSPI_FLASH_PORT);

    // /CS: de-active
    QSPI_SET_SS_HIGH(QSPI_FLASH_PORT);


    // /CS: active
    QSPI_SET_SS_LOW(QSPI_FLASH_PORT);

    // send Command: 0x02, Page program
    QSPI_WRITE_TX(QSPI_FLASH_PORT, 0x02);

    // send 24-bit start address
    QSPI_WRITE_TX(QSPI_FLASH_PORT, (addr >> 16) & 0xFF);
    QSPI_WRITE_TX(QSPI_FLASH_PORT, (addr >> 8)  & 0xFF);
    QSPI_WRITE_TX(QSPI_FLASH_PORT, addr       & 0xFF);

    // write data
    while(1)
    {
        if(!QSPI_GET_TX_FIFO_FULL_FLAG(QSPI_FLASH_PORT))
        {
            for (i = 0; i < len; i++)
            {
                QSPI_WRITE_TX(QSPI_FLASH_PORT, buf[len]);
            }
        }
    }

    // wait tx finish
    wait_QSPI_IS_BUSY(QSPI_FLASH_PORT);

    // /CS: de-active
    QSPI_SET_SS_HIGH(QSPI_FLASH_PORT);

    QSPI_ClearRxFIFO(QSPI_FLASH_PORT);

    return 0;
}

int32_t NVT_NVQSPI_Erase(uint32_t addr)
{
    // /CS: active
    QSPI_SET_SS_LOW(QSPI_FLASH_PORT);

    // send Command: 0x06, Write enable
    QSPI_WRITE_TX(QSPI_FLASH_PORT, 0x06);

    // wait tx finish
    wait_QSPI_IS_BUSY(QSPI_FLASH_PORT);

    // /CS: de-active
    QSPI_SET_SS_HIGH(QSPI_FLASH_PORT);

    // /CS: active
    QSPI_SET_SS_LOW(QSPI_FLASH_PORT);

    // send Command: 0x20, sector erase
    QSPI_WRITE_TX(QSPI_FLASH_PORT, 0x20);

    // send 24-bit address
    QSPI_WRITE_TX(QSPI_FLASH_PORT, (addr >> 16) & 0xFF);
    QSPI_WRITE_TX(QSPI_FLASH_PORT, (addr >> 8)  & 0xFF);
    QSPI_WRITE_TX(QSPI_FLASH_PORT,  addr        & 0xFF);

    // wait tx finish
    wait_QSPI_IS_BUSY(QSPI_FLASH_PORT);

    QSPI_ClearRxFIFO(QSPI_FLASH_PORT);

    // /CS: de-active
    QSPI_SET_SS_HIGH(QSPI_FLASH_PORT);

    return 0;
}

int32_t NVT_NVQSPI_WaitReady(void)
{
    volatile uint8_t u8ReturnValue;
    uint32_t u32TimeOutCnt = SystemCoreClock; /* 1 second time-out */

    do
    {
        if(--u32TimeOutCnt == 0)
        {
            printf("Wait for QSPI time-out!\n");
            return -1;
        }

        u8ReturnValue = SpiFlash_ReadStatusReg();
        u8ReturnValue = u8ReturnValue & 1;
    }
    while(u8ReturnValue != 0); // check the BUSY bit

    return 0;
}
