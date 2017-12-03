/*
* The code is released under the GNU General Public License.
* Developed by Mark Williams
* A guide to this code can be found here; http://ozzmaker.com/2013/04/22/845/
* Created 28th April 2013
*/


//#include <unistd.h>
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

	float rate_gyr_y = 0.0;   // [deg/s]
	float rate_gyr_x = 0.0;    // [deg/s]
	float rate_gyr_z = 0.0;     // [deg/s]


	int  *Pacc_raw;
	int  *Pmag_raw;
	int  *Pgyr_raw;
	int  acc_raw[3];
	int  mag_raw[3];
	int  gyr_raw[3];

	Pacc_raw = acc_raw;
	Pmag_raw = mag_raw;
	Pgyr_raw = gyr_raw;


	float gyroXangle = 0.0;
	float gyroYangle = 0.0;
	float gyroZangle = 0.0;
	float AccYangle = 0.0;
	float AccXangle = 0.0;
	float CFangleX = 0.0;
	float CFangleY = 0.0;

	int startInt  = mymillis();
	struct  timeval tvBegin, tvEnd,tvDiff;

	signed int acc_y = 0;
	signed int acc_x = 0;
	signed int acc_z = 0;
	signed int gyr_x = 0;
	signed int gyr_y = 0;
	signed int gyr_z = 0;


        signal(SIGINT, INThandler);

	enableIMU();

	gettimeofday(&tvBegin, NULL);



	while(1)
	{
	startInt = mymillis();


	//read ACC and GYR data
	//readMAG(Pmag_raw);
	//readACC(Pacc_raw);
	//readGYR(Pgyr_raw);
	
	//Acelerometer axis X
	//int16_t temp;
	//if( myIMU.readRegisterInt16(&temp, LSM6DS3_ACC_GYRO_OUTX_L_XL) != 0 )
	//{
	//	errorsAndWarnings++;
	//}
	int fd;
	// set offset
	unsigned char writeData[1] = {0};
	writeData[0]=LSM6DS3_ACC_GYRO_OUTX_L_XL;
	if((fd=wiringPiI2CSetup(I2CAddress))<0){
		printf("error opening i2c channel\n\r");
	}
	write (fd, (unsigned int)writeData, 1) ;
	// read data
	// Wire.requestFrom(I2CAddress, length);
	// c = Wire.read()
	unsigned char readData [2] ;
	if ((fd = wiringPiI2CSetup (I2CAddress)) < 0)
	{
		printf("error opening i2c channel\n\r");
	}
	read (fd, readData, 2) ;
	int16_t output0 = (int16_t)readData[0];
	int16_t output1 = (int16_t)(readData[1] << 8);
	//int16_t output = (int16_t)readData[0] | int16_t(readData[1] << 8);
	
	printf ("  %02X", readData [0]) ;
	printf ("  %02X", readData [1]) ;
	//for (;;)
	//{
	//	read (fd, readData, 2) ;
	//	for (int i = 0 ; i < 2 ; ++i)
	//	  printf ("  %02X", readData [i]) ;
	//	printf ("\n") ;
	//	sleep (1) ;
	//}
	
	//Serial.print(" X = ");
	//Serial.println(temp); 
	
	/* for reference
	status_t LSM6DS3Core::readRegisterInt16( int16_t* outputPointer, uint8_t offset )
	{
		uint8_t myBuffer[2];
		status_t returnError = readRegisterRegion(myBuffer, offset, 2);  //Does memory transfer
		int16_t output = (int16_t)myBuffer[0] | int16_t(myBuffer[1] << 8);
		
		*outputPointer = output;
		return returnError;
	}*/
	
	/*
	//Convert Gyro raw to degrees per second
	rate_gyr_x = (float) *gyr_raw * G_GAIN;
	rate_gyr_y = (float) *(gyr_raw+1) * G_GAIN;
	rate_gyr_z = (float) *(gyr_raw+2) * G_GAIN;



	//Calculate the angles from the gyro
	gyroXangle+=rate_gyr_x*DT;
	gyroYangle+=rate_gyr_y*DT;
	gyroZangle+=rate_gyr_z*DT;




	//Convert Accelerometer values to degrees
	AccXangle = (float) (atan2(*(acc_raw+1),*(acc_raw+2))+M_PI)*RAD_TO_DEG;
	AccYangle = (float) (atan2(*(acc_raw+2),*acc_raw)+M_PI)*RAD_TO_DEG;


	//Change the rotation value of the accelerometer to -/+ 180
	if (AccXangle >180)
	{
		AccXangle -= (float)360.0;
	}
	if (AccYangle >180)
		AccYangle -= (float)360.0;

//      Complementary filter used to combine the accelerometer and gyro values.
	CFangleX=AA*(CFangleX+rate_gyr_x*DT) +(1 - AA) * AccXangle;
	CFangleY=AA*(CFangleY+rate_gyr_y*DT) +(1 - AA) * AccYangle;
	*/

	printf ("   GyroX  %7.3f \t AccXangle \e[m %7.3f \t \033[22;31mCFangleX %7.3f\033[0m\t GyroY  %7.3f \t AccYangle %7.3f \t \033[22;36mCFangleY %7.3f\t\033[0m\n",gyroXangle,AccXangle,CFangleX,gyroYangle,AccYangle,CFangleY);

	//Each loop should be at least 20ms.
        while(mymillis() - startInt < 20)
        {
            usleep(100);
        }

	printf("Loop Time %d\t", mymillis()- startInt);
    }
}

