cmake_minimum_required(VERSION 3.2)
project(LandXML2glTFDLL)

set(CMAKE_CXX_STANDARD 17)
add_compile_definitions(LANDXML2GLTFDLLAPI_EXPORT)

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

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
target_compile_options(LandXML2glTFDLL PRIVATE "-fPIC")
endif()

# Link against libraries
target_link_libraries(LandXML2glTFDLL PRIVATE GLTFSDK)

