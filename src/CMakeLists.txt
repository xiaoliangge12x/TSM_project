# 源文件
aux_source_directory(. SRC_FILE)

if (NOT OPEN_TEST)
    list(REMOVE_ITEM SRC_FILE "./open_test.c")
endif()

# message(${SRC_FILE})
# set(SRC_FILE ${SRC_FILE}
#              ${TOOL_FILE})

# 指定头文件路径
set(INCLUDE_PATH ${CMAKE_SOURCE_DIR}/include/)
include_directories(${INCLUDE_PATH})

add_executable(tsm ${SRC_FILE})

if (OPEN_TEST)
    target_link_libraries(tsm yaml)
endif()
