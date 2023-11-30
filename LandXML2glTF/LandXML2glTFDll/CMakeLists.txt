cmake_minimum_required(VERSION 3.8)
project(LandXML2GLTFDLL)

set(CMAKE_CXX_STANDARD 17)
add_compile_definitions(LANDXML2GLTFDLLAPI_EXPORT)

# Add project source directories
aux_source_directory("${CMAKE_CURRENT_SOURCE_DIR}/source" PROJECT_SRCS)
aux_source_directory("${CMAKE_CURRENT_SOURCE_DIR}/source/helpers" PROJECT_SRCS)
aux_source_directory("${CMAKE_CURRENT_SOURCE_DIR}/source/models" PROJECT_SRCS)
aux_source_directory("${CMAKE_CURRENT_SOURCE_DIR}/source/readers" PROJECT_SRCS)
aux_source_directory("${CMAKE_CURRENT_SOURCE_DIR}/source/writers" PROJECT_SRCS)
aux_source_directory("../3rdparty/tinyxml/source" PROJECT_SRCS)

# Add project include directories

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/inc")
include_directories("../3rdparty/tinyxml/inc")
include_directories("../3rdparty/GLTFSDK/GLTFSDK/Inc/")
include_directories("../3rdparty/GLTFSDK/GLTFSDK/Inc/GLTFSDK")

add_library(LandXML2GLTFDLL SHARED ${PROJECT_SRCS})

# Link against libraries
target_link_libraries(LandXML2GLTFDLL PRIVATE GLTFSDK)

