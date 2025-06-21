cmake_minimum_required(VERSION 3.31)
project(Retro_Terminal CXX)

set(CMAKE_CXX_STANDARD 23)

if ("X${CMAKE_BUILD_TYPE}" STREQUAL "XDebug")
    add_compile_definitions(DEBUG=1)
endif ()

add_executable(Retro_Terminal
        main.cpp
        log.cpp         include/log.h
        file.cpp        include/file.h
)
target_include_directories(Retro_Terminal PRIVATE ${CMAKE_SOURCE_DIR}/include)
