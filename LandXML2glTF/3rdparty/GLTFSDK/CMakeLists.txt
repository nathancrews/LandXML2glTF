cmake_minimum_required(VERSION 3.2)
project (GLTFSDK)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG -D_DEBUG -DFEATURE_ASSERTS_ENABLED")
set(CMAKE_OSX_DEPLOYMENT_TARGET 11.0)

if (WIN32)
    # Define _SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING to disable the warnings in the current version of Google Test (1.8.0)
    # TODO: Newer versions shouldn't have this problem.  Re-evaluate this when upgrading.
    add_compile_definitions(
        _SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
    )
    # Multithreaded Build
    add_compile_options(
        /MP
    )
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/Build/CMake/Modules")

if(NOT IOS)
    # This variable is set by ios.toolchain.cmake.
    # Notice that External/RapidJSON uses `find_package` for external project is ready.
    # We have to prevent modification of RapidJSON_XXX variables in cache at this moment
    find_package(RapidJSON CONFIG)
endif()
if(NOT RapidJSON_FOUND)
    add_subdirectory(External/RapidJSON)
endif()

add_subdirectory(GLTFSDK)

if(RapidJSON_FOUND)
    target_include_directories(GLTFSDK
        PUBLIC $<BUILD_INTERFACE:${RapidJSON_INCLUDE_DIRS}>
    )
elseif(TARGET RapidJSON)
    get_target_property(RapidJSON_INCLUDE_DIRS RapidJSON INTERFACE_INCLUDE_DIRECTORIES)
    target_include_directories(GLTFSDK
        PUBLIC $<BUILD_INTERFACE:${RapidJSON_INCLUDE_DIRS}>
    )
endif()

INSTALL(DIRECTORY ${RapidJSON_INCLUDE_DIRS} DESTINATION ${CMAKE_SOURCE_DIR}/Built/Out/RapidJSON/ FILES_MATCHING PATTERN "*.h"  PATTERN "*.hpp")