cmake_minimum_required(VERSION 3.5)

project(LandXML2glTFDLL)

#find_package(PROJ REQUIRED CONFIG)

add_compile_definitions(LANDXML2GLTFDLLAPI_EXPORT)

if (WIN32)
    add_compile_definitions(
        _CRT_SECURE_NO_WARNINGS
    )
endif()

# Add project source directories
aux_source_directory("${CMAKE_CURRENT_SOURCE_DIR}/Source" PROJECT_SRCS)
aux_source_directory("${CMAKE_CURRENT_SOURCE_DIR}/Source/Helpers" PROJECT_SRCS)
aux_source_directory("${CMAKE_CURRENT_SOURCE_DIR}/Source/Models" PROJECT_SRCS)
aux_source_directory("${CMAKE_CURRENT_SOURCE_DIR}/Source/Readers" PROJECT_SRCS)
aux_source_directory("${CMAKE_CURRENT_SOURCE_DIR}/Source/Writers" PROJECT_SRCS)
aux_source_directory("../3rdparty/TinyXML/Source" PROJECT_SRCS)

# Add project include directories
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/Inc")
include_directories("../3rdparty/TinyXML/Inc")
include_directories("../3rdparty/GLTFSDK/GLTFSDK/Inc/")

add_library(LandXML2glTFDLL SHARED ${PROJECT_SRCS})

if (MSVC)
    set_target_properties(LandXML2glTFDLL PROPERTIES COMPILE_PDB_NAME "LandXML2glTFDLL" COMPILE_PDB_OUTPUT_DIRECTORY "${LIBRARY_OUTPUT_DIRECTORY}")
elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(LandXML2glTFDLL
        PRIVATE "-fpic"
        PRIVATE "-Wunguarded-availability"
        PRIVATE "-Wall"
        PRIVATE "-Werror"
        PRIVATE "-Wunused-but-set-variable"
        PUBLIC "-Wno-unknown-pragmas")
elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    target_compile_options(LandXML2glTFDLL
        PRIVATE "-fPIC")
endif()

target_link_libraries(LandXML2glTFDLL PRIVATE GLTFSDK)

