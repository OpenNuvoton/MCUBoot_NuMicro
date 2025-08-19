#include "boot_hal.h"
#include "Driver_Flash.h"
#include "NuMicro.h"

#ifdef FLASH_DEV_NAME
extern ARM_DRIVER_FLASH FLASH_DEV_NAME;
#endif /* FLASH_DEV_NAME */

#if defined(__ARMCC_VERSION)
extern uint32_t Image$$RW_RAM$$Base[];
extern uint32_t Image$$RW_RAM$$ZI$$Limit[];
#else
extern uint32_t __data_start__[];
extern uint32_t __StackTop[];
#endif

__WEAK __attribute__((naked)) void boot_clear_ram_area(void)
{
    __ASM volatile(
        ".syntax unified                             \n"
        "movs    r0, #0                              \n"
#if defined(__ARMCC_VERSION)
        "ldr     r1, =Image$$RW_RAM$$Base            \n"
        "ldr     r2, =Image$$RW_RAM$$ZI$$Limit       \n"
#else
        "ldr     r1, =__data_start__                 \n"
        "ldr     r2, =__StackTop                     \n"
#endif
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

__WEAK __attribute__((naked)) void boot_jump_to_next_image(uint32_t reset_handler_addr)
{
    __ASM volatile(
        ".syntax unified                 \n"
        "mov     r7, r0                  \n"
        "bl      boot_clear_ram_area     \n" /* Clear RAM before jump */
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

__WEAK void boot_platform_quit(struct boot_arm_vector_table *vt)
{
    /* Clang at O0, stores variables on the stack with SP relative addressing.
     * When manually set the SP then the place of reset vector is lost.
     * Static variables are stored in 'data' or 'bss' section, change of SP has
     * no effect on them.
     */
    static struct boot_arm_vector_table *vt_cpy;
    int32_t result;

#ifdef FLASH_DEV_NAME
    result = FLASH_DEV_NAME.Uninitialize();
    if (result != ARM_DRIVER_OK) {
        while(1) {}
    }
#endif /* FLASH_DEV_NAME */

    vt_cpy = vt;

    SCB->VTOR = vt_cpy->reset;

#if defined(__ARM_ARCH_8M_MAIN__) || defined(__ARM_ARCH_8M_BASE__) \
 || defined(__ARM_ARCH_8_1M_MAIN__)
    /* Restore the Main Stack Pointer Limit register's reset value
     * before passing execution to runtime firmware to make the
     * bootloader transparent to it.
     */
    __set_MSPLIM(0);
#endif

    __set_MSP(vt_cpy->msp);
    __DSB();
    __ISB();

    boot_jump_to_next_image(vt_cpy->reset);
}
