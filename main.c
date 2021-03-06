/*
* The code is released under the GNU General Public License.
* Developed by Mark Williams
* A guide to this code can be found here; http://ozzmaker.com/2013/04/22/845/
* Created 28th April 2013
*/


//#include <unistd.h>
#include <sys/stat.h> 
#include <math.h>
#include <signal.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include "L3G.h"
#include "LSM303.h"
#include "sensor.c"
#include "i2c-dev.h"


#define X   0
#define Y   1
#define Z   2

#define DT 0.02         // [s/loop] loop period. 20ms
#define AA 0.98         // complementary filter constant

#define A_GAIN 0.0573      // [deg/LSB]
#define G_GAIN 0.070     // [deg/s/LSB]
#define RAD_TO_DEG 57.29578
#define M_PI 3.14159265358979323846




void  INThandler(int sig)
{
        signal(sig, SIG_IGN);
        exit(0);
}

int mymillis()
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (tv.tv_sec) * 1000 + (tv.tv_usec)/1000;
}

int timeval_subtract(struct timeval *result, struct timeval *t2, struct timeval *t1)
{
    long int diff = (t2->tv_usec + 1000000 * t2->tv_sec) - (t1->tv_usec + 1000000 * t1->tv_sec);
    result->tv_sec = diff / 1000000;
    result->tv_usec = diff % 1000000;
    return (diff<0);
}

int main(int argc, char *argv[])
{


	int startInt  = mymillis();
	int startInt2  = mymillis();

	enableIMU();

	FILE *f = fopen("file.txt", "w+");
	if (f == NULL)
	{
		//int fd = open("file.txt", O_RDWR | O_CREAT, S_IRUSR | S_IRGRP | S_IROTH);
		//close(fd);
		f = fopen("file.txt", "w+");
		if (f == NULL)
		{
			printf("Error opening file!\n");
			return 0;
		}
		//printf("Error opening file!\n");
	}


	while(1)
	{
	startInt = mymillis();

	
	int fd;
	// set offset
	unsigned char writeData[1] = {0};
	writeData[0]=LSM6DS3_ACC_GYRO_OUTX_L_XL;
	if((fd=wiringPiI2CSetup(I2CAddress))<0){
		printf("error opening i2c channel\n\r");
	}
	write (fd, writeData, 1) ;
	usleep(10000);
	// read data
	unsigned char readData [2];
	read (fd, readData, 2) ;
	int16_t output01 = (int16_t)readData[0] | (int16_t)(readData[1] << 8);
	float output = (float)output01 * 0.061 * (16 >> 1) / 1000;
	printf (" X:  %f", output) ;
	fprintf(f, "%f, ", output);
	
	
	writeData[0]=LSM6DS3_ACC_GYRO_OUTY_L_XL;
	write (fd, writeData, 1) ;
	usleep(10000);
	// read data
	read (fd, readData, 2) ;
	 output01 = (int16_t)readData[0] | (int16_t)(readData[1] << 8);
	 output = (float)output01 * 0.061 * (16 >> 1) / 1000;
	printf (" Y:  %f", output) ;
	fprintf(f, "%f, ", output);
	
	writeData[0]=LSM6DS3_ACC_GYRO_OUTZ_L_XL;
	write (fd, writeData, 1) ;
	usleep(10000);
	// read data
	read (fd, readData, 2) ;
	 output01 = (int16_t)readData[0] | (int16_t)(readData[1] << 8);
	 output = (float)output01 * 0.061 * (16 >> 1) / 1000;
	printf (" Z:  %f\n", output) ;
	fprintf(f, "%f, ", output);
	
	fprintf(f, "%f, ", mymillis() - startInt2);
	//Each loop should be at least 20ms.
        while(mymillis() - startInt < 20)
        {
            usleep(100000);
        }
	usleep(1000000);
	//printf(" Loop Time %d\n", mymillis()- startInt);
    }
}

