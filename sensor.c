#include "i2c-dev.h"
#include "L3G.h"
#include "LSM303.h"
//#include "gyro.h"
#include "LSM6DS3.h"

#include <wiringPi.h>
#include <wiringPiI2C.h>

int file;

void  readBlock(uint8_t command, uint8_t size, uint8_t *data)
{
    int result = i2c_smbus_read_i2c_block_data(file, command, size, data);
    if (result != size)
    {
        printf("Failed to read block from I2C.");
        exit(1);
    }
}

void selectDevice(int file, int addr)
{
        char device[3];
        if (addr == 1)
                device == "L3G";
        else
                device == "LSM";


        if (ioctl(file, I2C_SLAVE, addr) < 0) {
                fprintf(stderr,
                        "Error: Could not select device  0x%02x: %s\n",
                        device, strerror(errno));
        }
}


void readACC(int  *a)
{
 	uint8_t block[6];
        selectDevice(file,ACC_ADDRESS);
		readBlock(0x80 | LSM303_OUT_X_L_A, sizeof(block), block);

        *a = (int16_t)(block[0] | block[1] << 8) >> 4;
        *(a+1) = (int16_t)(block[2] | block[3] << 8) >> 4;
        *(a+2) = (int16_t)(block[4] | block[5] << 8) >> 4;
}

void readMAG(int  *m)
{
	uint8_t block[6];
        selectDevice(file,MAG_ADDRESS);
	// DLHC: register address order is X,Z,Y with high bytes first
	readBlock(0x80 | LSM303_OUT_X_H_M, sizeof(block), block);

       	*m = (int16_t)(block[1] | block[0] << 8);
        *(m+1) = (int16_t)(block[5] | block[4] << 8) ;
        *(m+2) = (int16_t)(block[3] | block[2] << 8) ;
}
void readGYR(int *g)
{
	uint8_t block[6];

        selectDevice(file,GYR_ADDRESS);

	readBlock(0x80 | L3G_OUT_X_L, sizeof(block), block);

        *g = (int16_t)(block[1] << 8 | block[0]);
        *(g+1) = (int16_t)(block[3] << 8 | block[2]);
        *(g+2) = (int16_t)(block[5] << 8 | block[4]);

}


void writeAccReg(uint8_t reg, uint8_t value)
{
    selectDevice(file,ACC_ADDRESS);
  int result = i2c_smbus_write_byte_data(file, reg, value);
    if (result == -1)
    {
        printf ("Failed to write byte to I2C Acc.");
        exit(1);
    }
}

void writeMagReg(uint8_t reg, uint8_t value)
{
    selectDevice(file,MAG_ADDRESS);
  int result = i2c_smbus_write_byte_data(file, reg, value);
    if (result == -1)
    {
        printf("Failed to write byte to I2C Mag.");
        exit(1);
    }
}


void writeGyrReg(uint8_t reg, uint8_t value)
{
    selectDevice(file,GYR_ADDRESS);
  int result = i2c_smbus_write_byte_data(file, reg, value);
    if (result == -1)
    {
        printf("Failed to write byte to I2C Gyr.");
        exit(1);
    }
}


void enableIMU()
{

	__u16 block[I2C_SMBUS_BLOCK_MAX];

        int res, bus,  size;


        char filename[20];
        sprintf(filename, "/dev/i2c-%d", 1);
        file = open(filename, O_RDWR);
        if (file<0) {
		printf("Unable to open I2C bus!");
                exit(1);
        }

 // Enable accelerometer.
	//writeAccReg(LSM303_CTRL_REG1_A, 0b01010111); //  z,y,x axis enabled , 100Hz data rate
	//writeAccReg(LSM303_CTRL_REG4_A, 0b00101000); // +/- 8G full scale: FS = 10 on DLHC, high resolution output mode

	//!
	// uint8_t readCheck;
	// readRegister(&readCheck, LSM6DS3_ACC_GYRO_WHO_AM_I_REG);
	// if( readCheck != 0x69 )
	// {		
    //     returnError = IMU_HW_ERROR;
	// }
	
	uint8_t dataToWrite = 0;  //Temporary variable

    //Setup the accelerometer******************************
    dataToWrite = 0; //Start Fresh!
    dataToWrite |= LSM6DS3_ACC_GYRO_BW_XL_100Hz;
    dataToWrite |= LSM6DS3_ACC_GYRO_FS_XL_8g;
    dataToWrite |= LSM6DS3_ACC_GYRO_ODR_XL_416Hz;
	
	//Now, write the patched together data
	int fd;
	unsigned char dataArray[2] = {0};
	dataArray[0]=LSM6DS3_ACC_GYRO_CTRL1_XL;
	dataArray[1]=dataToWrite;
	if((fd=wiringPiI2CSetup(I2CAddress))<0){
		printf("error opening i2c channel\n\r");
	}
	write (fd, dataArray, 2) ;
	
	// read data
	unsigned char writeData[1] = {0};
	writeData[0]=LSM6DS3_ACC_GYRO_CTRL4_C;
	write (fd, writeData, 1) ;
	read (fd, dataToWrite, 1) ;
	dataToWrite &= ~((uint8_t)LSM6DS3_ACC_GYRO_BW_SCAL_ODR_ENABLED);
	dataToWrite |= LSM6DS3_ACC_GYRO_BW_SCAL_ODR_ENABLED;
	dataArray[0]=LSM6DS3_ACC_GYRO_CTRL4_C;
	dataArray[1]=dataToWrite;
	write (fd, dataArray, 2) ;

	
	
	
}



