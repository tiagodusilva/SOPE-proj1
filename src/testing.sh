#!/bin/sh

(./simpledu -a -L || echo $?) | sort -k2 > /tmp/testeT_simpledu.txt
(du -l -a -L || echo $?) | sort -k2 > /tmp/testeT_du.txt
diff -q /tmp/testeT_simpledu.txt /tmp/testeT_du.txt > /dev/null 2>&1 && echo OK || echo FAILED

diff /tmp/testeT_simpledu.txt /tmp/testeT_du.txt 
