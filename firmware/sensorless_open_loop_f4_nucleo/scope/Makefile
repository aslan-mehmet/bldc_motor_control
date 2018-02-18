all:
	gcc main.c -g
	./a.out NewFile1.csv File1.csv
	gnuplot -e "load 'plt_original'"
	gnuplot -e "load 'plt_compared'"
	echo done
