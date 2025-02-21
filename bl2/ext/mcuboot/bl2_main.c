#include "bootutil/bootutil_log.h"
#include "bootutil/bootutil.h"
#include "boot_hal.h"
#include "uart_stdout.h"

int main(void)
{
#if MCUBOOT_LOG_LEVEL > MCUBOOT_LOG_LEVEL_OFF
    stdio_init();
#endif
    
    /* Perform platform specific initialization */
    if (boot_platform_init() != 0) {
        BOOT_LOG_ERR("Platform init failed");
        FIH_PANIC;
    }

    BOOT_LOG_INF("Starting bootloader");

    FIH_PANIC;

    /* Dummy return to be compatible with some check tools */
    return FIH_FAILURE;
}