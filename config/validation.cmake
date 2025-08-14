# Prevent double inclusion
include_guard(GLOBAL)

function(validate_config)
    if(SECONDARY_SLOT_IN_SPI_FLASH)
        if(NOT PLATFORM_NAME STREQUAL "m460")
            message(FATAL_ERROR "Secondary slot residing in SPI flash is only supported on M460")
        endif()
    endif()
endfunction()
