#!/bin/bash

command_args=(
    "-l -a"
    "-l -a -L"
    "-l -a -L --max-depth=2"
    "-l -a -L --max-depth=2"
    "-l -a -L -B 25"
    "-l -a -B 25"
    "-l -a -b"
)

directories=(
    ${HOME}/Documents/SOPE/
    ${HOME}/Documents/SOPE/SOPE-proj1/
)

for dir in ${directories[*]}
do
    echo "Dir: ${dir}"
    for i in ${!command_args[*]}
    do
        echo -n "Test no. $i: "
        (./simpledu ${command_args[$i]} ${dir} || echo $?) | sort -k2 > /tmp/testeT_simpledu.txt
        (du ${command_args[$i]} ${dir} || echo $?) | sort -k2 > /tmp/testeT_du.txt
        diff -q /tmp/testeT_simpledu.txt /tmp/testeT_du.txt > /dev/null 2>&1 && echo OK || echo FAILED
        
        # Uncomment line below to print what's wrong (in case of test failed)
        # diff -q -y /tmp/testeT_simpledu.txt /tmp/testeT_du.txt 
    done
done
