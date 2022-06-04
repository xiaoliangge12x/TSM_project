# 源文件
aux_source_directory(. SRC_FILE)
FILE(GLOB BASE_FILE "./base/*.c")
# list(REMOVE_ITEM SRC_FILE "./tsm_warning.c")
if (INJECT_TEST)
    list(REMOVE_ITEM SRC_FILE "./open_test.c;./tsm_main.c")
endif()

if (NOT OPEN_TEST)
    list(REMOVE_ITEM SRC_FILE "./open_test.c")
endif()

# message(${SRC_FILE})
set(SRC_FILE ${SRC_FILE}
             ${BASE_FILE})

# 指定头文件路径
set(INCLUDE_PATH ${CMAKE_SOURCE_DIR}/include/
                 ${CMAKE_SOURCE_DIR}/src/)
include_directories(${INCLUDE_PATH})

if (INJECT_TEST)
    add_library(tsm SHARED ${SRC_FILE})
else()
    add_executable(tsm ${SRC_FILE})
endif()

if (INJECT_TEST)
    target_link_libraries(tsm gcov)
endif()

if (OPEN_TEST)
    target_link_libraries(tsm yaml)
endif()
