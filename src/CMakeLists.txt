cmake_minimum_required(VERSION 3.16)

# Project name and language
project(gdb C)

# Use gcc explicitly (optional but explicit)
set(CMAKE_C_COMPILER gcc)

# C standard
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# Warnings
add_compile_options(-Wall -Wextra -Wpedantic)

# debug symbols
# add_compile_options(-g)

# Executable
add_executable(gdb
    main.c
    logger.c
    loggerconf.c
    breakpoint.c
    cli.c
    operation.c
    registers.c
    breakpoint.c
    elf/symbols.c
    arch/x86_64.c
    utils/path_utils.c
    utils/parser.c
    utils/data_utils.c
)

# Include headers from this directory
target_include_directories(gdb PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Link against your logger shared library
target_link_libraries(gdb
    PRIVATE
    elf
    Zydis
    readline
)
