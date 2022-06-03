#!/bin/bash -e
cur_dir=$(cd `dirname $0`; pwd)
build_path=${cur_dir}/../../build

cd ${build_path}

# generate *.so
sh build.sh injection_test

cd ../tool/data_injection

# execute data injection test
python3 injection_test.py