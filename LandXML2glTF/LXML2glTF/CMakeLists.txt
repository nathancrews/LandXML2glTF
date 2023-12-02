cmake_minimum_required(VERSION 3.5)

project(LXML2GLTF)

add_compile_definitions(LANDXML2GLTFDLLAPI_IMPORT)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/Inc")
include_directories("../LandXML2glTFDll/Inc")

add_executable(LXML2GLTF "${CMAKE_CURRENT_SOURCE_DIR}/Source/LXML2glTF.cpp")

if (MSVC)
    set_target_properties(LXML2GLTF PROPERTIES COMPILE_PDB_NAME "LXML2GLTF" COMPILE_PDB_OUTPUT_DIRECTORY "${LIBRARY_OUTPUT_DIRECTORY}")
elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(LXML2GLTF
        PRIVATE "-fpic"
        PRIVATE "-Wunguarded-availability"
        PRIVATE "-Wall"
        PRIVATE "-Werror"
        PRIVATE "-Wunused-but-set-variable"
        PUBLIC "-Wno-unknown-pragmas")
elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    target_compile_options(LXML2GLTF
        PRIVATE "-fPIC")
endif()

add_custom_command(TARGET LXML2GLTF POST_BUILD
COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CMAKE_CURRENT_LIST_DIR}/data" "${CMAKE_BINARY_DIR}/bin/$<CONFIG>/data"
)

target_link_libraries(LXML2GLTF PRIVATE LandXML2glTFDLL)
