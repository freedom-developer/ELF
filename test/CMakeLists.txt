cmake_minimum_required(VERSION 2.10)

project(test_demo)

add_library(add SHARED add.c)

add_executable(${PROJECT_NAME} test.c)

target_link_libraries(${PROJECT_NAME}  add)

