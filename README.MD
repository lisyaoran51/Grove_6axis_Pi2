

Requires WiringPi

Compile with;

git pull origin

gcc -o main -l rt main.c -lm -IwiringPi

rpi accelerometer
http://ozzmaker.com/guide-to-interfacing-a-gyro-and-accelerometer-with-a-raspberry-pi/

grove arduino accelerometer
https://github.com/Seeed-Studio/Accelerometer_And_Gyroscope_LSM6DS3/blob/master/SparkFunLSM6DS3.cpp

wire to wiringpi
https://www.raspberrypi.org/forums/viewtopic.php?f=44&t=53680



error: undefined reference to `wiringPiI2CSetup’
sudo apt-get install libi2c-dev

then re-make wiringPi:

cd wiringPi
./build clean ; ./build

If you don’t get any messages at all about I2C, then you need to upgrade:

git pull ; ./build