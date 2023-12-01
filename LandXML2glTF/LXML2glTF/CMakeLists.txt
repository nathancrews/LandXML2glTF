cmake_minimum_required(VERSION 3.2)
project(LXML2GLTF)

set(CMAKE_CXX_STANDARD 17)

add_compile_definitions(LANDXML2GLTFDLLAPI_IMPORT)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/Inc")
include_directories("../LandXML2glTFDll/Inc")

add_executable(LXML2GLTF "${CMAKE_CURRENT_SOURCE_DIR}/Source/LXML2glTF.cpp")

target_link_libraries(LXML2GLTF PRIVATE LandXML2glTFDLL)



