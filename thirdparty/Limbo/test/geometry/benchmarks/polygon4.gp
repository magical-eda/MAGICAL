clear
reset
set obj 1000 polygon from \
		0, 0 to \
		20, 0 to \
		20, 20 to \
		5, 20 to \
		5, 15 to \
		15, 15 to \
		15, 10 to \
		5, 10 to \
		5, 20 to \
		0, 20 to \
		0, 0
#set obj 1000 fc rgb "cyan" fillstyle solid lw 0
set obj 1000 fc rgb "cyan" fillstyle transparent pattern 5 bo
set xrange [-10:30] 
set yrange [-10:30]
plot NaN notitle
