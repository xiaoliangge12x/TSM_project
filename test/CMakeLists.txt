# 指定CMake最低版本
cmake_minimum_required(VERSION 3.4.1)

# 配置编译器(一定要放在工程之前，否则失效)
set(CMAKE_C_COMPILER gcc)
set(CMAKE_CXX_COMPILER g++)

project(test_tsm)

# 显示编译命令, for debug
set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
# 增加编译选项
add_compile_options(-D_NEED_LOG)
# generate coverage report
add_compile_options(-fprofile-arcs -ftest-coverage)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/../output/test/)

# 程序源文件
aux_source_directory(../src/ SRC_FILE)
FILE(GLOB BASE_FILE "../src/base/*.c")
list(REMOVE_ITEM SRC_FILE "../src//open_test.c"
                          "../src//tsm_main.c")

set(SRC_FILE ${SRC_FILE} ${BASE_FILE})
# 测试文件
aux_source_directory(. TEST_FILE)

set(TOTOAL_FILE ${SRC_FILE} ${TEST_FILE})

set(INCLUDE_PATH ${CMAKE_SOURCE_DIR}/../include/
                 ${CMAKE_SOURCE_DIR}/../src/)

include_directories(${INCLUDE_PATH})

add_executable(test_tsm ${TOTOAL_FILE})

target_link_libraries(test_tsm gtest 
                               pthread 
                               gcov)
