cmake_minimum_required(VERSION 3.16)
project(CharPaper LANGUAGES C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
        set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_library(project_warnings INTERFACE)
target_compile_options(project_warnings INTERFACE
                        -Wall
                        -Wextra
                        -Wpedantic
                        -Werror
                        )

add_subdirectory(daemon)
add_subdirectory(renderer)
add_subdirectory(apps/charpaper)
add_subdirectory(lib)
