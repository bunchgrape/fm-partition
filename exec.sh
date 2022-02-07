#!/bin/bash  

# Define your function here
FileSuffix () {
    local filename="$1"
    if [ -n "$filename" ]; then
        echo "${filename##*.}"
    fi
}

# Define your function here
IsSuffix () {
    local filename="$1"
    local suffix="$2"
    if [ "$(FileSuffix ${filename})" = "$suffix" ]; then
        return 0
    else
        return 1
    fi
}

for file in `ls ./testcases`;  
do   
    IsSuffix ${file} "cells"
    ret=$?
    if [  $ret -eq 0 ]; then
        exec ./par ./testcases/$file
    fi
done 