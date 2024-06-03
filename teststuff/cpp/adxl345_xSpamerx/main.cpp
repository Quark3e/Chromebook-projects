#include <stdio.h>
#include <stdlib.h>
#include <linux/i2c-dev.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <iostream>
#include <cmath>
#include "func.h"
using namespace std;

float G_GAIN = 0.07;
float DT = 0.02; // [s/loop] loop period. 20ms
//#define M_PI = 3.14159265358979323846;
float RAD_TO_DEG = 57.29578; 
float AA = 0.98; // complementary filter constant


int main(int argc, char **argv)
{
	
	int fd;// File descrition
	short ax,ay,az;
	short gx,gy,gz;

	float rate_gyr_x(0.0),rate_gyr_y(0.0),rate_gyr_z(0.0);
	float gyroXangle(0.0),gyroYangle(0.0),gyroZangle(0.0);

	float AccYangle(0.0),AccXangle(0.0);
	
	float CFangleX = 0.0;
    float CFangleY = 0.0;
	if(!init(fd))
			exit(1);
	usleep(100000);
	while(readADXL345(fd,ax,ay,az) && readL3G4200D(fd,gx,gy,gz))
	{
		//Convert Gyro raw to degrees per second
		rate_gyr_x = (float)gx * G_GAIN;
		rate_gyr_y = (float)gy * G_GAIN;
		rate_gyr_z = (float)gz * G_GAIN;
		//Calculate the angles from the gyro
		gyroXangle+=rate_gyr_x*DT;
        gyroYangle+=rate_gyr_y*DT;
        gyroZangle+=rate_gyr_z*DT;
        //Convert Accelerometer values to degrees
		AccXangle = (float) (atan2(ay,az)+M_PI)*RAD_TO_DEG;
        AccYangle = (float) (atan2(ax,az)+M_PI)*RAD_TO_DEG;

         //Change the rotation value of the accelerometer to -/+ 180
        if (AccXangle >180)
        {
                AccXangle -= (float)360.0;
        }
        if (AccYangle >180)
                AccYangle -= (float)360.0;

        // Complementary filter used to combine the accelerometer and gyro values.
        CFangleX=AA*(CFangleX+rate_gyr_x*DT) +(1 - AA) * AccXangle;
        CFangleY=AA*(CFangleY+rate_gyr_y*DT) +(1 - AA) * AccYangle;
		//std::cout<<"ax:"<<ax<<"\tay:"<<ay<<"\taz:"<<az<<"\tgx:"<<gx<<"\tgy:"<<gy<<"\tgz:"<<gz<<std::endl;
		printf ("   GyroX  %7.3f \t AccXangle \e[m %7.3f \t \033[22;31mCFangleX %7.3f\033[0m\t GyroY  %7.3f \t AccYangle %7.3f \t \033[22;36mCFangleY %7.3f\t\033[0m\n",gyroXangle,AccXangle,CFangleX,gyroYangle,AccYangle,CFangleY);
		//std::cout<<" GyroX "<<gyroXangle<<"\t AccXangle "<<AccXangle<<"\tCFangleX "<<CFangleX<<"\tGyroY "<<gyroYangle<<"\t AccYangle "<<AccYangle<<"\tCFangleY "<<CFangleY<<"\n";
		usleep(20000);	
	}
			
	return 0;
}