#include "NuMicro.h"
#include "boot_hal.h"
#include "Driver_Flash.h"
#include "flash_layout.h"
#include <stdio.h>

#ifdef FLASH_DEV_NAME
extern ARM_DRIVER_FLASH FLASH_DEV_NAME;
#endif /* FLASH_DEV_NAME */

void abort(void);

__attribute__((noreturn))
void __aeabi_assert(const char* expr, const char* file, int line)
{
    char str[12], * p;

    fputs("*** assertion failed: ", stderr);
    fputs(expr, stderr);
    fputs(", file ", stderr);
    fputs(file, stderr);
    fputs(", line ", stderr);

    p = str + sizeof(str);
    *--p = '\0';
    *--p = '\n';
    while(line > 0)
    {
        *--p = '0' + (line % 10);
        line /= 10;
    }
    fputs(p, stderr);

    abort();
}

void abort(void)
{
    for(;;);
}

/* bootloader platform-specific hw initialization */
int32_t boot_platform_init(void)
{
    int32_t result;

    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Set XT1_OUT(PF.2) and XT1_IN(PF.3) to input mode */
    PF->MODE &= ~(GPIO_MODE_MODE2_Msk | GPIO_MODE_MODE3_Msk);

    /* Enable HIRC clock */
    CLK_EnableXtalRC(CLK_PWRCTL_HIRCEN_Msk);

    /* Wait for HIRC clock ready */
    CLK_WaitClockReady(CLK_STATUS_HIRCSTB_Msk);

    /* Set core clock as 72MHz from PLL */
    CLK_SetCoreClock(FREQ_72MHZ);

    /* Set PCLK0/PCLK1 to HCLK/2 */
    CLK->PCLKDIV = (CLK_PCLKDIV_APB0DIV_DIV2 | CLK_PCLKDIV_APB1DIV_DIV2);

    /* Update System Core Clock */
    /* User can use SystemCoreClockUpdate() to calculate SystemCoreClock. */
    SystemCoreClockUpdate();

    /* Select UART clock source from HIRC */
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL4_UART0SEL_HIRC, CLK_CLKDIV0_UART0(1));

    /* Enable UART clock */
    CLK_EnableModuleClock(UART0_MODULE);

    /*----------------------------------------------------------------------*/
    /* Init I/O Multi-function                                              */
    /*----------------------------------------------------------------------*/
    /* Set multi-function pins */
    Uart0DefaultMPF();

    /* Reset UART0 */
    SYS_ResetModule(UART0_RST);

    UART_Open(UART0, 115200);

#ifdef FLASH_DEV_NAME
    result = FLASH_DEV_NAME.Initialize(NULL);
    if (result != ARM_DRIVER_OK) {
        return 1;
    }
#endif /* FLASH_DEV_NAME */

    return 0;
}