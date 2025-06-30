cmake_minimum_required(VERSION 3.30)

if ("X${CMAKE_BUILD_TYPE}" STREQUAL "XDebug")
    add_subdirectory(src/imgInTerm/)
endif ()

project(fss CXX C)

find_program(CCACHE_PROGRAM ccache)
if (NOT "${CCACHE_PROGRAM}" STREQUAL "CCACHE_PROGRAM-NOTFOUND")
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
            -march=native
            -mtune=native

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
    set(optimization_flags
            -O3
            -march=native
            -mtune=native
            -flto
            -fomit-frame-pointer
            -ffast-math
            -fstrict-aliasing
            -fdata-sections
            -ffunction-sections
            -D_FORTIFY_SOURCE=2
            -fstack-protector-strong
            -Wl,-z,relro -Wl,-z,now
            -s
    )

    set(optimization_link_flags
            -O3
            -march=native
            -mtune=native
            -flto
            -fomit-frame-pointer
            -ffast-math
            -fstrict-aliasing
            -fdata-sections
            -ffunction-sections
            -Wl,--gc-sections
            -D_FORTIFY_SOURCE=2
            -fstack-protector-strong
            -Wl,-z,relro -Wl,-z,now
            -s
    )

    add_compile_definitions(DEBUG=0)
    add_compile_options(${optimization_flags})
    add_link_options(${optimization_link_flags})
endif ()
set(CMAKE_CXX_STANDARD 23)
include_directories(src/include)
add_compile_definitions(CORE_VERSION="0.0.1")
add_compile_definitions(BACKEND_VERSION="0.0.1")
add_library(core SHARED
        src/include/helper/cpp_assert.h
        src/include/helper/WorkerThread.h
        src/include/helper/err_type.h
        src/helper/log.cpp              src/include/helper/log.h
        src/helper/backtrace.cpp        src/include/helper/backtrace.h
        src/helper/execute.cpp          src/include/helper/execute.h
        src/helper/color.cpp            src/include/helper/color.h
        src/helper/get_env.cpp          src/include/helper/get_env.h
        src/helper/arg_parser.cpp       src/include/helper/arg_parser.h
        src/core/configuration.cpp      src/include/core/configuration.h
        src/core/cache.cpp              src/include/core/cache.h
        src/core/g_global_config_t.cpp  src/include/core/g_global_config_t.h
        src/helper/lz4.c                src/include/helper/lz4.h
        src/helper/lz4frame.c           src/include/helper/lz4frame.h src/include/helper/lz4frame_static.h
        src/helper/lz4hc.c              src/include/helper/lz4hc.h
        src/helper/xxhash.c             src/include/helper/xxhash.h
        src/core/directory.cpp          src/include/core/directory.h
        src/core/bin2hex.cpp
        src/core/crc64sum.cpp
)

if ("X${CMAKE_BUILD_TYPE}" STREQUAL "XDebug")
    add_executable(test.exe src/utest/test.cpp src/include/test/test.h src/utest/main.cpp)
    target_link_libraries(test.exe PRIVATE core tiv)
    target_compile_definitions(test.exe PRIVATE SOURCE_DIR="${CMAKE_SOURCE_DIR}")

    add_custom_target(test
            COMMAND ${CMAKE_BINARY_DIR}/test.exe
            USES_TERMINAL
            DEPENDS test.exe
    )
endif ()

add_subdirectory(src/json)
add_executable(backend
        src/backend/main.cpp
        src/backend/service.cpp
        src/backend/instances.cpp       src/backend/instance.h
        src/backend/CrowLog.cpp         src/backend/CrowLog.h
        src/backend/dictionary.cpp      src/backend/dictionary.h
        src/backend/CrowResourceAccess.cpp
        src/backend/CrowPing.cpp
        src/backend/CrowStream.cpp
        src/backend/CrowRegister.h
        src/client/main.cpp
        src/backend/file_access.cpp
        src/backend/file_access.h
        src/include/core/bin2hex.h
        src/include/core/crc64sum.h
)
target_include_directories(backend PRIVATE src/json/include)
target_link_libraries(backend PRIVATE core nlohmann_json::nlohmann_json)

add_executable(compress src/utils/compress.c)
target_link_libraries(compress PRIVATE core)

add_executable(decompress src/utils/decompress.c)
target_link_libraries(decompress PRIVATE core)
