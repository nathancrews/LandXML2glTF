cmake_minimum_required(VERSION 3.2)

project(LANDXML2GLTF1)

set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/GLTFSDK")
add_subdirectory(LandXML2glTFDll)
add_subdirectory(LXML2glTF)

