#########################################################################
# File Name: build_graph.sh
# Author: Biying Xu 
# mail: biying@utexas.edu
# Created Time: Mon 29 Oct 2018 05:49:44 PM CDT
#########################################################################
#!/bin/bash

FILES=benchmark/*
for f in $FILES
do
	echo "Processing $f file..."
	IFS='/' read -ra f_delim <<< "$f"
    word=${f_delim[1]}
    length=${#word}
    cname=${word:0:($length-3)}
	python build_graph.py -f $f > logs/$cname.log
done
