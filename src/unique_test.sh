#!/bin/bash

simpledu_location="${HOME}/Documents/SOPE/SOPE-proj1/src/simpledu"

command_args="-l -a -L -S -B 25"
dir="/home/tiago/"

echo "Dir: $dir"
echo ${simpledu_location} ${command_args} ${dir}
(${simpledu_location} ${command_args} ${dir} 2>"/dev/null" || echo $?) | sort -k2 > /tmp/testeT_simpledu.txt
(du ${command_args} ${dir} 2>"/dev/null" || echo $?) | sort -k2 > /tmp/testeT_du.txt
diff -y --suppress-common-lines /tmp/testeT_simpledu.txt /tmp/testeT_du.txt
diff -q /tmp/testeT_simpledu.txt /tmp/testeT_du.txt > /dev/null 2>&1 && echo OK || echo FAILED

