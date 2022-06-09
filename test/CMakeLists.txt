# 指定CMAKE最低版本
cmake_minimum_required(VERSION 3.4.1)

# 配置编译器(注，一定要放在工程之前)， 否则不生效
set(CMAKE_C_COMPILER gcc)
set(CMAKE_CXX_COMPILER g++)

project(test_tsm)

# 显示编译命令
set(CMAKE_VERBOSE_MAKEFILE ON)

add_compile_options(-D_NEED_LOG)
add_compile_options(-std=c++11)
# generate coverage report
add_compile_options(-fprofile-arcs -ftest-coverage)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/../output/test/)

# 测试文件
FILE(GLOB TEST_FILE "./test_tsm_monitor.cc")
FILE(GLOB SRC_FILE "../src/tsm_monitor.c"
                   "../src/tsm_parameter.c"
                   "../src/tsm_signalhandling.c")

set(TEST_FILE ${TEST_FILE} ${SRC_FILE})

# 指定头文件路径
set(INCLUDE_PATH ${CMAKE_SOURCE_DIR}/../include/
                 ${CMAKE_SOURCE_DIR}/../src/)

include_directories(${INCLUDE_PATH})

add_executable(test_tsm ${TEST_FILE})

target_link_libraries(test_tsm gtest pthread gcov)