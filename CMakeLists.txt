cmake_minimum_required(VERSION 3.5.1)
#enable_testing()
project(Controle)

#configuring versioning

set(Controle_VERSION_MAJOR 1)
set(Controle_VERSION_MINOR 0)
##############################################




set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})

set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

set(PROJECT_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)

include_directories("${PROJECT_BINARY_DIR}")
include_directories("${PROJECT_INCLUDE_DIR}")

add_subdirectory(server)
add_subdirectory(Test)
