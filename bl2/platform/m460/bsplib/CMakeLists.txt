add_library(bsp_lib STATIC)

target_include_directories(bsp_lib
    PUBLIC
        bsp/Library/StdDriver/inc
        bsp/Library/Device/Nuvoton/m460/Include
        bsp/Library/CMSIS/Include
)

target_sources(bsp_lib
    PRIVATE
        bsp/Library/Device/Nuvoton/m460/Source/system_m460.c
        bsp/Library/StdDriver/src/retarget.c
        bsp/Library/StdDriver/src/uart.c
        bsp/Library/StdDriver/src/clk.c
        bsp/Library/StdDriver/src/sys.c
        bsp/Library/StdDriver/src/fmc.c
)

if(SECONDARY_SLOT_IN_SPI_FLASH)
    if(USE_SPIM)
        target_sources(bsp_lib
            PRIVATE
                bsp/Library/StdDriver/src/spim.c
        )
    else()
        target_sources(bsp_lib
            PRIVATE
                bsp/Library/StdDriver/src/spi.c
                bsp/Library/StdDriver/src/gpio.c
        )
    endif()
endif()

if (CMAKE_C_COMPILER_ID STREQUAL "ARMClang")
target_sources(bsp_lib
    PRIVATE
        bsp/Library/Device/Nuvoton/m460/Source/ARM/startup_m460.s
)
target_compile_options(bsp_lib
    PRIVATE
        $<$<COMPILE_LANGUAGE:ASM>:-DStack_Size=0x4000>
)
elseif (CMAKE_C_COMPILER_ID STREQUAL "GNU")
target_sources(bsp_lib
    PRIVATE
        bsp/Library/Device/Nuvoton/m460/Source/GCC/startup_m460.S
)
target_compile_options(bsp_lib
    PRIVATE
        $<$<COMPILE_LANGUAGE:ASM>:-D__STACK_SIZE=0x4000>
)
endif()

target_compile_definitions(bsp_lib
    PUBLIC
        __MICROLIB
)
