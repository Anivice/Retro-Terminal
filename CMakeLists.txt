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

    if ("${COMPILE_WITH_MEMORY_SANITIZERS}" STREQUAL "True")
        message(STATUS "Sanitizers for memory enabled")
        list(APPEND compiler_options
                -fsanitize=address                  # Detect illegal memory access such as buffer overflows and use-after-free
                -fsanitize=undefined                # Detect undefined behavior like integer overflows and null dereferencing
        )

        list(APPEND linker_options
                -fsanitize=address                  # Link the AddressSanitizer runtime for memory integrity
                -fsanitize=undefined                # Link the UndefinedBehaviorSanitizer for detecting undefined behavior
                -lasan -lubsan
        )
    endif ()
    add_compile_options(${compiler_options})
    add_link_options(${linker_options})
else ()
    add_compile_definitions(DEBUG=0)
endif ()

add_compile_definitions(VERSION="0.0.1")
add_executable(lfs
        main.cpp
        log.cpp         include/log.h
        include/cpp_assert.h
        backtrace.cpp   include/backtrace.h
        execute.cpp     include/execute.h
        include/err_type.h
        color.cpp       include/color.h
        get_env.cpp     include/get_env.h
        arg_parser.cpp  include/arg_parser.h
        configuration.cpp include/configuration.h
)
target_include_directories(lfs PRIVATE ${CMAKE_SOURCE_DIR}/include)

add_custom_target(remove_objmap_c
        COMMAND rm -f ${CMAKE_BINARY_DIR}/objmap.cpp
        COMMENT "Removing previously generated map"
)
add_custom_command(
        OUTPUT  ${CMAKE_BINARY_DIR}/objmap.cpp
        DEPENDS lfs remove_objmap_c
        COMMAND ${CMAKE_SOURCE_DIR}/get_map.sh "${CMAKE_BINARY_DIR}/lfs" > ${CMAKE_BINARY_DIR}/objmap.cpp
        COMMENT "Generating backtrace map..."
)
add_library(lfs_map SHARED ${CMAKE_BINARY_DIR}/objmap.cpp)
