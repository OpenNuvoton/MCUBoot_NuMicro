add_library(bsp_lib STATIC)

target_include_directories(bsp_lib
    PUBLIC
        bsp/Library/StdDriver/inc
        bsp/Library/Device/Nuvoton/M55M1/Include
        bsp/Library/CMSIS/Core/Include
)

target_sources(bsp_lib
    PRIVATE
        bsp/Library/Device/Nuvoton/M55M1/Source/startup_M55M1.c
        bsp/Library/Device/Nuvoton/M55M1/Source/system_M55M1.c
        bsp/Library/StdDriver/src/retarget.c
        bsp/Library/StdDriver/src/uart.c
        bsp/Library/StdDriver/src/clk.c
        bsp/Library/StdDriver/src/sys.c
        bsp/Library/StdDriver/src/fmc.c
)

target_compile_definitions(bsp_lib
    PUBLIC
        __MICROLIB
        DEBUG_PORT=UART3  # PH, TEST CHIP
)
