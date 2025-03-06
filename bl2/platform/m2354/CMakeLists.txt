add_subdirectory(bsplib)

######################### Platform region defs  ################################

target_include_directories(platform_region_defs
    INTERFACE
        partition
)

######################### Platform BL2 #########################################

# target_include_directories(platform_bl2_lib
#     PUBLIC
# )

target_sources(platform_bl2_lib
    PRIVATE
        boot_hal.c
)

target_link_libraries(platform_bl2_lib
    PUBLIC
        bsp_lib
)

