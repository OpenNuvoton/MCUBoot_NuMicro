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

    SYS_UnlockReg();

    /* Enable HIRC clock */
    CLK_EnableXtalRC(CLK_PWRCTL_HIRCEN_Msk);

    /* Wait for HIRC clock ready */
    CLK_WaitClockReady(CLK_STATUS_HIRCSTB_Msk);

    /* Set core clock to 96MHz */
    CLK_SetCoreClock(96000000);

    /* Enable UART0 module clock */
    CLK_EnableModuleClock(UART0_MODULE);

    /* Select UART0 module clock source as HIRC and UART0 module clock divider as 1 */
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL2_UART0SEL_HIRC, CLK_CLKDIV0_UART0(1));

    /* Update System Core Clock */
    SystemCoreClockUpdate();

    /* Set multi-function pins for UART0 RXD and TXD */
    SET_UART0_RXD_PA6();
    SET_UART0_TXD_PA7();

    UART_Open(UART0, 115200);

#ifdef FLASH_DEV_NAME
    result = FLASH_DEV_NAME.Initialize(NULL);
    if (result != ARM_DRIVER_OK) {
        return 1;
    }
#endif /* FLASH_DEV_NAME */

    return 0;
}