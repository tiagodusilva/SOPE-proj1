#!/bin/bash

command_args=(
    "-l -a -L"
    "-l -a -L --max-depth=2"
    "-l -a -L --max-depth=2"
    "-l -a -L -B 25"
    "-l -a -B 25"
    "-a -b"
)

folder=${HOME}/Documents/SOPE/SOPE-proj1/src

for i in ${!command_args[*]}
do
    echo -n "Test no. $i: "
    (./simpledu ${command_args[$i]} ${folder} || echo $?) | sort -k2 > /tmp/testeT_simpledu.txt
    (du ${command_args[$i]} ${folder} || echo $?) | sort -k2 > /tmp/testeT_du.txt
    diff -q /tmp/testeT_simpledu.txt /tmp/testeT_du.txt > /dev/null 2>&1 && echo OK || echo FAILED
    
    # diff /tmp/testeT_simpledu.txt /tmp/testeT_du.txt 
done
