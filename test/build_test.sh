#!/bin/bash -e

cd build/

# build
cmake .. && make -j8

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