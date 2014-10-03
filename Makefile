all: rtl_t1
rtl_t1: rtl_t1.c
	gcc $(CFLAGS) rtl_t1.c convenience/convenience.c -lrtlsdr -lfftw3 -lm -g -o rtl_t1
clean:
	rm rtl_t1
leakcheck: rtl_t1
	valgrind --leak-check=full ./rtl_t1
run: rtl_t1
	./rtl_t1
plotdft: run
	gnuplot -persist -e "set autoscale fix;plot \"testdft.dat\" using 1:2 with lines" 
plotfulldft: rtl_t1.c
	#I don't know how to use variables
	gcc -DBIGDFT rtl_t1.c convenience/convenience.c -lrtlsdr -lfftw3 -lm -g -o rtl_t1
	./rtl_t1
	gnuplot -persist -e "set autoscale fix;plot \"bigdft.dat\" using 1:2 with lines" 
	
