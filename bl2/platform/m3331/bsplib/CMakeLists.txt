add_library(bsp_lib STATIC)

target_include_directories(bsp_lib
    PUBLIC
        bsp/Library/StdDriver/inc
        bsp/Library/Device/Nuvoton/m3331/Include
        bsp/Library/CMSIS/Core/Include
)

target_sources(bsp_lib
    PRIVATE
        bsp/Library/Device/Nuvoton/m3331/Source/ARM/startup_m3331.S
        bsp/Library/Device/Nuvoton/m3331/Source/system_m3331.c
        bsp/Library/StdDriver/src/retarget.c
        bsp/Library/StdDriver/src/uart.c
        bsp/Library/StdDriver/src/clk.c
        bsp/Library/StdDriver/src/sys.c
)

set_source_files_properties(bsp/Library/Device/Nuvoton/m3331/Source/ARM/startup_m3331.S
    PROPERTIES
        COMPILE_FLAGS "-DStack_Size=0x1000"
)

target_compile_definitions(bsp_lib
    PUBLIC
        # DEBUG_PORT=UART1
        __MICROLIB
)
