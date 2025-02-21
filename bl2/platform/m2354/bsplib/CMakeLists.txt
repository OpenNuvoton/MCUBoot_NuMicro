add_library(bsp_lib STATIC)

target_include_directories(bsp_lib
    PUBLIC
        bsp/Library/StdDriver/inc
        bsp/Library/Device/Nuvoton/M2354/Include
        bsp/Library/CMSIS/Include
)

target_sources(bsp_lib
    PRIVATE
        bsp/Library/Device/Nuvoton/M2354/Source/ARM/startup_M2354.s
        bsp/Library/Device/Nuvoton/M2354/Source/system_M2354.c
        bsp/Library/StdDriver/src/retarget.c
        bsp/Library/StdDriver/src/uart.c
        bsp/Library/StdDriver/src/clk.c
        bsp/Library/StdDriver/src/sys.c
)

set_source_files_properties(bsp/Library/Device/Nuvoton/M2354/Source/ARM/startup_M2354.s
    PROPERTIES
        COMPILE_FLAGS "-Wa,--pd,\"Stack_Size EQU 0x1000\""
)

target_compile_definitions(bsp_lib
    PUBLIC
        # DEBUG_PORT=UART1
        __MICROLIB
)
