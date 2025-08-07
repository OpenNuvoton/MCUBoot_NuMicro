add_library(bsp_lib STATIC)

target_include_directories(bsp_lib
    PUBLIC
        bsp/Library/StdDriver/inc
        bsp/Library/Device/Nuvoton/m460/Include
        bsp/Library/CMSIS/Include
)

target_sources(bsp_lib
    PRIVATE
        bsp/Library/Device/Nuvoton/m460/Source/ARM/startup_m460.s
        bsp/Library/Device/Nuvoton/m460/Source/system_m460.c
        bsp/Library/StdDriver/src/retarget.c
        bsp/Library/StdDriver/src/uart.c
        bsp/Library/StdDriver/src/clk.c
        bsp/Library/StdDriver/src/sys.c
        bsp/Library/StdDriver/src/fmc.c
        $<$<BOOL:${SECONDARY_SLOT_IN_SPI_FLASH}>:bsp/Library/StdDriver/src/spim.c>
)

set_source_files_properties(bsp/Library/Device/Nuvoton/m460/Source/ARM/startup_m460.s
    PROPERTIES
        COMPILE_FLAGS "-DStack_Size=0x4000"
)

target_compile_definitions(bsp_lib
    PUBLIC
        __MICROLIB
)
