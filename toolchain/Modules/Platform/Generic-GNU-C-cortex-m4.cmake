include(${CMAKE_CURRENT_LIST_DIR}/Generic-GNU-cortex-m-common.cmake)

set(CMAKE_C_FLAGS_INIT "-mcpu=cortex-m4 -mfloat-abi=hard ${CMAKE_C_FLAGS_INIT}")
