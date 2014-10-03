//this is not affiliated with the rtl-sdr project, but uses their library and helper code
#include "rtl-sdr.h"
#include "convenience/convenience.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include "fftw3.h"

#define SAMPLERATE ((2048000))
#define BUFLEN (((1<<16)/64))


int main(){
	printf("samplerate: %d\nbuflen:%d\n", SAMPLERATE, BUFLEN);
	rtlsdr_dev_t *dev = NULL;
	int devindex = verbose_device_search("0");

	int r = rtlsdr_open(&dev, devindex);
	if(r){printf("err %d opening device\n", r);}
	verbose_set_sample_rate(dev, SAMPLERATE);
	verbose_set_frequency(dev, 90000000);//357000);
	//verbose_gain_set(dev, 496);
	verbose_gain_set(dev, 200);
	//verbose_auto_gain(dev);

	verbose_reset_buffer(dev);
	char *buf = calloc(BUFLEN,1);
	//char *buf = malloc((1<<16)*8);//[(1<<16)*8] = {0};
	int howmany = 0;
	rtlsdr_read_sync(dev, buf, BUFLEN, &howmany);
	printf("read %d of %d\n", howmany, BUFLEN);

	FILE *testfile = fopen("test.dat", "w");
	//fprintf(testfile, "[\n");
	int i;
	for(i=0;i<howmany/2;i++){
		fprintf(testfile, "%d %d\n", i, (int)((unsigned short*)buf)[i] - 0xffff/2);
	}
	fwrite(buf, 1, howmany,testfile);
	fclose(testfile);



	double *inbuf = calloc(BUFLEN/2, sizeof(double));
	fftw_complex *outbuf = calloc(BUFLEN/2,  sizeof(fftw_complex));
	fftw_plan plan = fftw_plan_dft_r2c_1d(BUFLEN/2, inbuf, outbuf, FFTW_ESTIMATE);




	for(i=0;i<howmany/2;i++){
		inbuf[i] = (double)((int)((unsigned short*)buf)[i] - 0xffff/2);
	}
	fftw_execute(plan);
	testfile = fopen("testdft.dat", "w");
	for(i=1/*ignore first component (dc?)*/;i<howmany/2/2;i++){
		fprintf(testfile, "%d %f\n", i, log10(abs(outbuf[i][0]))-log10(DBL_MAX));//this math is probably/definitely all wrong
	}
	fclose(testfile);
	//free(buf);










#ifdef BIGDFT
	testfile = fopen("bigdft.dat", "w");
	int freq;
	//for(freq=2000000;freq<1000000000;freq += 2048000){
	//for(freq=80000000;freq<100000000;freq += SAMPLERATE/*BUFLEN/2*/){
	for(freq=40000000;freq<1000000000;freq += SAMPLERATE/*BUFLEN/2*/){
		verbose_set_frequency(dev, freq);//357000);
		verbose_reset_buffer(dev);
		rtlsdr_read_sync(dev, buf, BUFLEN, &howmany);
		for(i=0;i<howmany/2;i++){
			inbuf[i] = (double)((int)((unsigned short*)buf)[i] - 0xffff/2);
		}
		fftw_execute(plan);
		for(i=1;i<howmany/2/2;i+= 1){//50){
			//fprintf(testfile, "%d %f\n", i*(SAMPLERATE/(howmany/2/2))+freq, (outbuf[i][0]));//this math is probably/definitely all wrong
			fprintf(testfile, "%d %f\n", i*(SAMPLERATE/(howmany/2/2))+freq, log10(abs(outbuf[i][0]))-log10(DBL_MAX));//this math is probably/definitely all wrong
		}
		printf("%d\n", freq);
	}
	fclose(testfile);
#endif











	rtlsdr_close(dev);




	return 0;
}
