//this is not affiliated with the rtl-sdr project, but uses their library and helper code
#include "rtl-sdr.h"
#include "convenience/convenience.h"
#include <stdio.h>
#include <stdlib.h>
#include "fftw3.h"


int main(){
	rtlsdr_dev_t *dev = NULL;
	int devindex = verbose_device_search("0");

	int r = rtlsdr_open(&dev, devindex);
	if(r){printf("err %d opening device\n", r);}
	verbose_set_sample_rate(dev, 2048000);
	verbose_set_frequency(dev, 90000000);//357000);
	verbose_gain_set(dev, 207);
	//verbose_auto_gain(dev);

	verbose_reset_buffer(dev);
	char buf[(1<<16)*2] = {0};
	//char *buf = malloc((1<<16)*8);//[(1<<16)*8] = {0};
	int howmany = 0;
	rtlsdr_read_sync(dev, buf, sizeof(buf), &howmany);
	printf("read %d of %d\n", howmany, sizeof(buf));
	rtlsdr_close(dev);

	FILE *testfile = fopen("test.dat", "w");
	//fprintf(testfile, "[\n");
	int i;
	for(i=0;i<howmany/2;i++){
		fprintf(testfile, "%d %d\n", i, (int)((unsigned short*)buf)[i] - 0xffff/2);
	}
	fwrite(buf, 1, howmany,testfile);
	fclose(testfile);



	double inbuf[sizeof(buf)/2] = {0};
	fftw_complex outbuf[sizeof(buf)/2] = {0};
	fftw_plan plan = fftw_plan_dft_r2c_1d(sizeof(buf)/2, inbuf, outbuf, FFTW_ESTIMATE);




	for(i=0;i<howmany/2;i++){
		inbuf[i] = (double)((int)((unsigned short*)buf)[i] - 0xffff/2);
	}
	fftw_execute(plan);
	testfile = fopen("testdft.dat", "w");
	for(i=1/*ignore first component (dc?)*/;i<howmany/2/2;i++){
		fprintf(testfile, "%d %f\n", i, outbuf[i][0]);
	}
	fclose(testfile);
	//free(buf);
	return 0;
}
