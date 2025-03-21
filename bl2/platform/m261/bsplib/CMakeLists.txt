add_library(bsp_lib STATIC)

target_include_directories(bsp_lib
    PUBLIC
        bsp/Library/StdDriver/inc
        bsp/Library/Device/Nuvoton/M261/Include
        bsp/Library/CMSIS/Include
)

target_sources(bsp_lib
    PRIVATE
        bsp/Library/Device/Nuvoton/M261/Source/ARM/startup_M261.s
        bsp/Library/Device/Nuvoton/M261/Source/system_M261.c
        bsp/Library/StdDriver/src/retarget.c
        bsp/Library/StdDriver/src/uart.c
        bsp/Library/StdDriver/src/clk.c
        bsp/Library/StdDriver/src/sys.c
        bsp/Library/StdDriver/src/fmc.c
)

set_source_files_properties(bsp/Library/Device/Nuvoton/M261/Source/ARM/startup_M261.s
    PROPERTIES
        COMPILE_FLAGS "-Wa,--pd,\"Stack_Size EQU 0x4000\""
)

target_compile_definitions(bsp_lib
    PUBLIC
        __MICROLIB
)
