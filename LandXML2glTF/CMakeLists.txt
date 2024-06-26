cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_OSX_DEPLOYMENT_TARGET 11.0)

set (CMAKE_LIBRARY_OUTPUT_DIRECTORY  "${CMAKE_BINARY_DIR}/bin")
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

project(LANDXML2GLTF)

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/GLTFSDK")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/TinyGLTF")
add_subdirectory(LandXML2glTFDll)
add_subdirectory(LXML2glTF)
add_subdirectory(LXML2glTF_CGI)

