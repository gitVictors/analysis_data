cmake_minimum_required(VERSION 3.29.0)
project(analis_sha VERSION 0.1.0 LANGUAGES C)

include(CTest)
enable_testing()

set(SRC_LIST main.c log_step.c)

add_executable(analis_sha ${SRC_LIST})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mbig-endian")

include(CPack)
