clear
reset
set obj 1000 polygon from \
0, 0  to \
3, 0 to \
3, 1 to \
5, 1 to \
5, 5 to \
3, 5 to \
3, 4 to \
4, 4 to \
4, 3 to \
2, 3 to \
2, 1 to \
1, 1 to \
1, 2 to \
2, 2 to \
2, 3 to \
0, 3 to \
0, 0
#set obj 1000 fc rgb "cyan" fillstyle solid lw 0
set obj 1000 fc rgb "cyan" fillstyle transparent pattern 5 bo
set xrange [-5:10] 
set yrange [-5:10] 
plot NaN notitle
