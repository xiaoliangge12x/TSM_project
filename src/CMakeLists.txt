# 源文件
aux_source_directory(. SRC_FILE)

# 指定头文件路径
set(INCLUDE_PATH ${CMAKE_SOURCE_DIR}/include/)
message(${INCLUDE_PATH})
include_directories(${INCLUDE_PATH})

message(${SRC_FILE})

add_executable(tsm ${SRC_FILE})
