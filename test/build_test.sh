#!/bin/bash -e
export ECHO_STYLE_DEFAULT="\033[0m" 
export ECHO_STYLE_GREEN="\033[32m" 
export ECHO_STYLE_RED="\033[31m"
# cd ../build/

# # build src shared library
# sh build.sh injection_test
# if [ $? -ne 0  ]; then
#     echo -e "${ECHO_STYLE_RED}build tsm library failed.${ECHO_STYLE_DEFAULT}"
#     exit 1
# fi

cd build

# build test 
cmake .. && make -j8
if [ $? -ne 0 ]; then
    echo -e "${ECHO_STYLE_RED}build test excutable failed.${ECHO_STYLE_DEFAULT}"
    exit 1
fi
cd ../../output/test

# run unittest
./test_tsm

cd ../../test

# generate converage report
cd build

lcov -c -d ./ -o cover.info

# --remove 删除统计信息中如下的代码或文件， 支持正则
lcov --remove cover.info '*/usr/include/*' '*/usr/lib/*' '*/test/*' -o cover.info

genhtml -o cover_report cover.info