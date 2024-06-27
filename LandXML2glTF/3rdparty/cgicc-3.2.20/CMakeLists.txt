cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(cgicc)

set(CMAKE_OSX_DEPLOYMENT_TARGET 14.0)

if (WIN32)
    add_compile_definitions(
        _CRT_SECURE_NO_WARNINGS
    )
endif()

# Add project source directories
aux_source_directory("${CMAKE_CURRENT_SOURCE_DIR}/cgicc" PROJECT_SRCS)

# Add project include directories
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/cgicc")

add_library(cgicc STATIC ${PROJECT_SRCS})

if (MSVC)
    set_target_properties(cgicc PROPERTIES COMPILE_PDB_NAME "cgicc" COMPILE_PDB_OUTPUT_DIRECTORY "${LIBRARY_OUTPUT_DIRECTORY}")
elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(cgicc
        PRIVATE "-fpic"
        PRIVATE "-Wunguarded-availability"
        PRIVATE "-Wall"
        PRIVATE "-Werror"
        PRIVATE "-Wunused-but-set-variable"
        PUBLIC "-Wno-unknown-pragmas")
elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    target_compile_options(cgicc PRIVATE "-fPIC")
endif()


