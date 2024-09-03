cmake_minimum_required(VERSION 2.10)
project(myreadelf)

set(CMAKE_CXX_STANDARD 14)

file(GLOB SRC_LIST *.cpp *.c *.h)

add_executable(${PROJECT_NAME} ${SRC_LIST})
