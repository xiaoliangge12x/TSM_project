#!/bin/bash -e

function help() 
{
    echo "sh build.sh: normal build"
    echo "sh build.sh [open_test]: build with open_test"
}

function main() 
{
    if [[ $1 == '--help' || $1 == '-h' ]]; then
        help    
        exit 0
    fi
    find . ! \( -name '*.sh' -or -name '..'  -or -name '.' -or -name '.gitignore' \) | xargs rm -rf
    if [[ $# -eq 1 && $1 == 'open_test' ]]; then
        cmake .. -DOPEN_TEST=ON && make -j8
        exit 0
    fi
    cmake .. && make clean && make -j8
}

main $@