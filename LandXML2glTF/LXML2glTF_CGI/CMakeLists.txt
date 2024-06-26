cmake_minimum_required(VERSION 3.5)

project(LXML2GLTF_CGI)

add_compile_definitions(LANDXML2GLTFDLLAPI_IMPORT)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/Inc")
include_directories("../LandXML2glTFDll/Inc")
include_directories("../3rdparty/cgicc-3.2.20/cgicc")
include_directories("../3rdparty/cgicc-3.2.20")

aux_source_directory("${CMAKE_CURRENT_SOURCE_DIR}/Source" PROJECT_SRCS)
aux_source_directory("../3rdparty/cgicc-3.2.20/cgicc" PROJECT_SRCS)

add_executable(LXML2GLTF_CGI ${PROJECT_SRCS})

if (MSVC)
    set_target_properties(LXML2GLTF_CGI PROPERTIES COMPILE_PDB_NAME "LXML2GLTF_CGI" COMPILE_PDB_OUTPUT_DIRECTORY "${LIBRARY_OUTPUT_DIRECTORY}")
elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(LXML2GLTF_CGI
        PRIVATE "-fpic"
        PRIVATE "-Wunguarded-availability"
        PRIVATE "-Wall"
        PRIVATE "-Werror"
        PRIVATE "-Wunused-but-set-variable"
        PUBLIC "-Wno-unknown-pragmas")
elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    target_compile_options(LXML2GLTF_CGI
        PRIVATE "-fPIC")
endif()

add_custom_command(TARGET LXML2GLTF_CGI POST_BUILD
COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CMAKE_CURRENT_LIST_DIR}/data" "${CMAKE_BINARY_DIR}/bin/$<CONFIG>/data"
)

target_link_libraries(LXML2GLTF_CGI PRIVATE LandXML2glTFDLL)
