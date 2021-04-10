#!/bin/bash

unit_tests=("ut1 ut2 ut3 ut4")

for ut in $unit_tests;
do
    ../test_table $ut > _test.txt
    diff ${ut}_correct.txt _test.txt
    _res=`diff ${ut}_correct.txt _test.txt | wc -l`
    if [ $_res -eq 0 ]
    then
	echo "${ut} PASSED"
    else
	echo "${ut} ** FAILED **"
	diff ${ut}_correct.txt _test.txt
    fi    
done
rm _test.txt
