cmake_minimum_required(VERSION 3.5.1)
project(main)

set(PROJEC_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)
set(PROJECT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)


set(MAIN_SRCS
		${PROJECT_SOURCE_DIR}/server.c
		)

include_directories("${PROJECT_BINARY_DIR}")
add_executable(main ${MAIN_SRCS})
include_directories("${PROJECT_INCLUDE_DIR}")

