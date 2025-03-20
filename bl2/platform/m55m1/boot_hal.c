#include "NuMicro.h"
#include "boot_hal.h"
#include "Driver_Flash.h"
#include "flash_layout.h"

#ifdef FLASH_DEV_NAME
extern ARM_DRIVER_FLASH FLASH_DEV_NAME;
#endif /* FLASH_DEV_NAME */

extern uint32_t Image$$RW_NOINIT$$Base[];
extern uint32_t Image$$ARM_LIB_HEAP$$ZI$$Limit[];
extern uint32_t Image$$DTCM$$Base[];
extern uint32_t Image$$ARM_LIB_STACK$$ZI$$Limit[];

__attribute__((naked)) void boot_clear_sram_area(void)
{
    __ASM volatile(
        ".syntax unified                             \n"
        "movs    r0, #0                              \n"
        "ldr     r1, =Image$$RW_NOINIT$$Base         \n"
        "ldr     r2, =Image$$ARM_LIB_HEAP$$ZI$$Limit \n"
        "subs    r2, r2, r1                          \n"
        "Loop:                                       \n"
        "subs    r2, #4                              \n"
        "blt     Clear_done                          \n"
        "str     r0, [r1, r2]                        \n"
        "b       Loop                                \n"
        "Clear_done:                                 \n"
        "bx      lr                                  \n"
         : : : "r0" , "r1" , "r2" , "memory"
    );
}

__attribute__((naked)) void boot_clear_dtcm_area(void)
{
    __ASM volatile(
        ".syntax unified                             \n"
        "movs    r0, #0                              \n"
        "ldr     r1, =Image$$DTCM$$Base              \n"
        "ldr     r2, =Image$$ARM_LIB_STACK$$ZI$$Limit\n"
        "subs    r2, r2, r1                          \n"
        "Do_clear:                                   \n"
        "subs    r2, #4                              \n"
        "blt     Done                                \n"
        "str     r0, [r1, r2]                        \n"
        "b       Do_clear                            \n"
        "Done:                                       \n"
        "bx      lr                                  \n"
         : : : "r0" , "r1" , "r2" , "memory"
    );
}


__attribute__((naked)) void boot_jump_to_next_image(uint32_t reset_handler_addr)
{
    __ASM volatile(
        ".syntax unified                 \n"
        "mov     r7, r0                  \n"
        "bl      boot_clear_sram_area    \n" /* Clear SRAM before jump */
        "bl      boot_clear_dtcm_area    \n" /* Clear DTCM before jump */
        "movs    r0, #0                  \n" /* Clear registers: R0-R12, */
        "mov     r1, r0                  \n" /* except R7 */
        "mov     r2, r0                  \n"
        "mov     r3, r0                  \n"
        "mov     r4, r0                  \n"
        "mov     r5, r0                  \n"
        "mov     r6, r0                  \n"
        "mov     r8, r0                  \n"
        "mov     r9, r0                  \n"
        "mov     r10, r0                 \n"
        "mov     r11, r0                 \n"
        "mov     r12, r0                 \n"
        "mov     lr,  r0                 \n"
        "bx      r7                      \n" /* Jump to Reset_handler */
    );
}

/* bootloader platform-specific hw initialization */
int32_t boot_platform_init(void)
{
    int32_t result;

    /* Unlock protected registers */
    SYS_UnlockReg();

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init System Clock                                                                                       */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Enable PLL0 220MHz clock from HIRC and switch SCLK clock source to PLL0 */
    CLK_SetBusClock(CLK_SCLKSEL_SCLKSEL_APLL0, CLK_APLLCTL_APLLSRC_HIRC, FREQ_220MHZ);

    /* Update System Core Clock */
    /* User can use SystemCoreClockUpdate() to calculate SystemCoreClock. */
    SystemCoreClockUpdate();

#if 1 // PH, TEST CHIP
    /* Enable External HXT clock */
    CLK_EnableXtalRC(CLK_SRCCTL_HXTEN_Msk);

    /* Waiting for HXT clock ready */
    CLK_WaitClockReady(CLK_STATUS_HXTSTB_Msk);

    /* Select UART6 clock source from HXT */
    CLK_SetModuleClock(UART3_MODULE, CLK_UARTSEL0_UART3SEL_HXT, CLK_UARTDIV0_UART3DIV(1));

    /* Enable UART clock */
    CLK_EnableModuleClock(UART3_MODULE);

    /* Reset UART module */
    SYS_ResetModule(SYS_UART3RST);

    /* Set multi-function pins for UART0 RXD and TXD */
    SET_UART3_RXD_PE0();
    SET_UART3_TXD_PE1();

    /* Init UART to 115200-8n1 for print message */
    UART_Open(UART3, 115200);
#endif

#ifdef FLASH_DEV_NAME
    result = FLASH_DEV_NAME.Initialize(NULL);
    if (result != ARM_DRIVER_OK) {
        return 1;
    }
#endif /* FLASH_DEV_NAME */

    printf("Image$$RW_NOINIT$$Base=%x\n", (uint32_t)Image$$RW_NOINIT$$Base);
    printf("Image$$ARM_LIB_HEAP$$ZI$$Limit=%x\n", (uint32_t)Image$$ARM_LIB_HEAP$$ZI$$Limit);
    printf("Image$$DTCM$$Base=%x\n", (uint32_t)Image$$DTCM$$Base);
    printf("Image$$ARM_LIB_STACK$$ZI$$Limit=%x\n", (uint32_t)Image$$ARM_LIB_STACK$$ZI$$Limit);

    return 0;
}