add_library(platform_bl2_lib STATIC)
add_library(platform_region_defs INTERFACE)
add_library(platform_common_interface INTERFACE)

add_subdirectory(${PLATFORM_NAME})

######################### Platform Common Interface ############################

target_include_directories(platform_common_interface
    INTERFACE
        common
        driver
        include
)

######################### Platform BL2 #########################################

# target_include_directories(platform_bl2_lib
#     PUBLIC
# )

target_sources(platform_bl2_lib
    PRIVATE
        common/common.c
)

target_link_libraries(platform_bl2_lib
    PUBLIC
        platform_common_interface
        platform_region_defs
)

# target_compile_definitions(platform_bl2_lib
#     PUBLIC    
# )