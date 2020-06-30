cmake_minimum_required(VERSION 3.8)
project(net-dbg)

set(CMAKE_C_STANDARD 99)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/source/)

FILE(GLOB SOURCE_FILES RELATIVE
        "${CMAKE_CURRENT_SOURCE_DIR}"
        "${CMAKE_CURRENT_SOURCE_DIR}/source/*.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/source/net/*.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/source/luaapi/*.c"
    )


add_executable(net-dbg ${SOURCE_FILES})
target_link_libraries(net-dbg lua5.2)
