#!/bin/bash
for f in `ls *.cpp`
do
	echo $f; /usr/bin/time -f "%e %M" g++ -I ../../include $f
done
