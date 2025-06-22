cmake_minimum_required(VERSION 3.31)
project(lfs CXX)

set(CMAKE_CXX_STANDARD 23)

if ("X${CMAKE_BUILD_TYPE}" STREQUAL "XDebug")
    add_compile_definitions(DEBUG=1)
endif ()

add_executable(lfs
        main.cpp
        log.cpp         include/log.h
        file.cpp        include/file.h
        include/cpp_assert.h
        backtrace.cpp   include/backtrace.h
        execute.cpp     include/execute.h
        include/err_type.h
        color.cpp
        include/color.h
        get_env.cpp
        include/get_env.h
)
target_include_directories(lfs PRIVATE ${CMAKE_SOURCE_DIR}/include)
