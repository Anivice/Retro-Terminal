cmake_minimum_required(VERSION 3.31)
project(lfs CXX)

set(CMAKE_CXX_STANDARD 23)

find_program(CCACHE_PROGRAM ccache)
if (NOT "${CCACHE_PROGRAM}" STREQUAL "")
    set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
    set(CMAKE_C_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
    message(STATUS "Using ${CCACHE_PROGRAM}")
endif ()

if ("X${CMAKE_BUILD_TYPE}" STREQUAL "XDebug")
    add_compile_definitions(DEBUG=1)
    set(compiler_options
            # Warnings and diagnostics
            -Wall                                   # Enable common warnings
            -Wextra                                 # Enable extra warnings
            -Wpedantic                              # Strict compliance with the standard
            -Wunused                                # Warn about unused variables, functions, etc.
            -Wuninitialized                         # Warn if variables are used uninitialized
            -fdiagnostics-show-option               # Show which option triggered the warning
            -fdiagnostics-color=always              # Enable colored diagnostics for better readability

            # Debugging and stack protection
            -g3                                     # Maximum debug information, including macro expansions
            -O0
            -fstack-usage                           # Generate stack usage info for each function
            -fstack-protector-all                   # Protect all functions with a stack canary to prevent stack overflow attacks
            # -D_FORTIFY_SOURCE=2                   # Buffer overflow detection on safer libc functions (e.g., memcpy).
            # You need to enable optimization for _FORTIFY_SOURCE to work!
            -gdwarf-4                               # Generate DWARF version 4 debug information

            -fno-eliminate-unused-debug-types
            -fno-omit-frame-pointer
            -Wl,-z,relro -Wl,-z,now
            -fstack-protector-all
    )

    set(linker_options
            # Linker options for memory safety, thread safety, and verbose debugging
            -Wl,--no-omagic                         # Prevent the generation of object files in memory; useful for debugging
            -Wl,--as-needed                         # Only link libraries that are actually needed to reduce binary size
            -Wl,--fatal-warnings                    # Treat all linker warnings as errors to catch issues early
            -Wl,-z,relro                            # Read-only relocations to prevent certain memory exploits (optional)
            -Wl,-z,now                              # Fully resolve all symbols during the link time for extra safety
            -Wl,-z,noexecstack                      # Prevent execution of code on the stack (security hardening)
            -Wl,-z,defs                             # Ensure all symbols are defined, and prevent undefined symbols
            -Wl,-O0

            -gdwarf-4                               # Generate detailed debug information for the linker
            -fno-eliminate-unused-debug-types
            -fno-omit-frame-pointer

            # Stack protection
            -fstack-protector-all                   # Link with stack protection for all functions
            -Wl,-z,relro -Wl,-z,now
    )

#    list(APPEND compiler_options
#            -fsanitize=address
#            -fsanitize=undefined
#    )
#
#    list(APPEND linker_options
#            -fsanitize=address
#            -fsanitize=undefined
#            -lasan -lubsan
#    )

#    list(APPEND compiler_options
#            -fsanitize=thread
#            -fsanitize=undefined
#    )
#
#    list(APPEND linker_options
#            -fsanitize=thread
#            -fsanitize=undefined
#            -ltsan -lubsan
#    )

    add_compile_options(${compiler_options})
    add_link_options(${linker_options})
else ()
    add_compile_definitions(DEBUG=0)
endif ()

include_directories(src/include)
add_compile_definitions(VERSION="0.0.1")
add_library(lfs SHARED
        src/helper/log.cpp           src/include/helper/log.h
        src/include/helper/cpp_assert.h
        src/helper/backtrace.cpp     src/include/helper/backtrace.h
        src/helper/execute.cpp       src/include/helper/execute.h
        src/include/helper/err_type.h
        src/helper/color.cpp         src/include/helper/color.h
        src/helper/get_env.cpp       src/include/helper/get_env.h
        src/helper/arg_parser.cpp    src/include/helper/arg_parser.h
        src/helper/configuration.cpp src/include/helper/configuration.h
        src/helper/socket.cpp        src/include/helper/socket.h
        src/helper/file.cpp          src/include/helper/file.h
        src/core/directory.cpp       src/include/core/directory.h
        src/core/file.cpp            src/include/core/file.h
        src/core/cache.cpp           src/include/core/cache.h
        src/core/basic_file_ops.cpp  src/include/core/basic_file_ops.h
        src/include/helper/WorkerThread.h
        src/core/compress.cpp        src/include/core/compress.h
        src/core/g_global_config_t.cpp
        src/include/core/g_global_config_t.h
)

add_executable(server src/server/main.cpp)
target_link_libraries(server PRIVATE lfs)

add_executable(client src/client/main.cpp)
target_link_libraries(client PRIVATE lfs)

add_executable(compress src/tests/compress.cpp)
target_link_libraries(compress PRIVATE lfs)
add_executable(decompress src/tests/decompress.cpp)
target_link_libraries(decompress PRIVATE lfs)
