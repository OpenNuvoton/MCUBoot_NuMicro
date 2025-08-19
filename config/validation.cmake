# Prevent double inclusion
include_guard(GLOBAL)

function(validate_config)
    if(SECONDARY_SLOT_IN_SPI_FLASH)
        if(NOT PLATFORM_NAME STREQUAL "m460")
            message(FATAL_ERROR "Secondary slot residing in SPI flash is only supported on M460")
        endif()
    endif()

    set(VALID_SPI_DRIVERS SPIM QSPI GENERIC)
    list(FIND VALID_SPI_DRIVERS "${SPI_DRIVER}" SPI_DRIVER_IDX)
    if(SPI_DRIVER_IDX EQUAL -1)
        message(FATAL_ERROR "Invalid SPI_DRIVER=${SPI_DRIVER}")
    endif()
endfunction()
