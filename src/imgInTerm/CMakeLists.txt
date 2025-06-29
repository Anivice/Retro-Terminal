cmake_minimum_required(VERSION 3.30)

set(PNG_STATIC "True")
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/libpng)

project(tiv CXX)
add_library(tiv STATIC CImg.h tiv_lib.cpp tiv_lib.h libtiv.cpp embedded_pic.cpp
        1747807830_5d7ff0c2_fc3886495a6e3443.cpp 1747807833_9615059c_c4756facc140586c.cpp)
target_compile_options(tiv PRIVATE -std=c++17 -Wall -fexceptions)
target_include_directories(tiv PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/libpng ${CMAKE_CURRENT_BINARY_DIR}/libpng)
target_link_libraries(tiv PRIVATE png_static)
add_dependencies(tiv png_static)
