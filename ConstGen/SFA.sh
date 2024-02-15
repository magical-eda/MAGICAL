#!/bin/bash

FILES=benchmarks_spectre/*
for f in $FILES
do
	echo "Processing $f file..."
	IFS='/' read -ra f_delim <<< "$f"
    word=${f_delim[1]}
    length=${#word}
    cname=${word:0:($length-8)}
    ./bin/ConstGen $f logs/$cname
#	./bin/ConstGen $f > logs/$cname.log
done
