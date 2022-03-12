# 指定CMAKE最低版本
cmake_minimum_required(VERSION 3.4.1)

# 配置编译器(注，一定要放在工程之前)， 否则不生效
set(CMAKE_C_COMPILER gcc)
set(CMAKE_CXX_COMPILER g++)

# 工程名
project(tsm)

# 指定生成路径
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/output/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/output/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/output/bin)

# 添加编译选项
add_compile_options(-pthread)

# 进入下一层目录
add_subdirectory(src)
