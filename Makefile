all: rtl_t1
rtl_t1: rtl_t1.c
	gcc rtl_t1.c convenience/convenience.c -lrtlsdr -lfftw3 -o rtl_t1
leakcheck: rtl_t1
	valgrind --leak-check=full ./rtl_t1
run: rtl_t1
	./rtl_t1
plotdft: run
	gnuplot -persist -e "set autoscale fix;plot \"testdft.dat\" using 1:2 with lines" 
