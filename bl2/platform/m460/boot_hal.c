#include "NuMicro.h"
#include "boot_hal.h"
#include "Driver_Flash.h"
#include "flash_layout.h"

#ifdef FLASH_DEV_NAME
extern ARM_DRIVER_FLASH FLASH_DEV_NAME;
#endif /* FLASH_DEV_NAME */

/* bootloader platform-specific hw initialization */
int32_t boot_platform_init(void)
{
    int32_t result;

    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Set PCLK0 and PCLK1 to HCLK/2 */
    CLK->PCLKDIV = (CLK_PCLKDIV_APB0DIV_DIV2 | CLK_PCLKDIV_APB1DIV_DIV2);

    /* Set core clock to 200MHz */
    CLK_SetCoreClock(200000000);

    /* Enable UART0 module clock */
    CLK_EnableModuleClock(UART0_MODULE);

    /* Select UART0 module clock source as HIRC and UART0 module clock divider as 1 */
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UART0SEL_HIRC, CLK_CLKDIV0_UART0(1));

    /* Set GPB multi-function pins for UART0 RXD and TXD */
    SET_UART0_RXD_PB12();
    SET_UART0_TXD_PB13();

    UART_Open(UART0, 115200);

#ifdef FLASH_DEV_NAME
    result = FLASH_DEV_NAME.Initialize(NULL);
    if (result != ARM_DRIVER_OK) {
        return 1;
    }
#endif /* FLASH_DEV_NAME */

    return 0;
}