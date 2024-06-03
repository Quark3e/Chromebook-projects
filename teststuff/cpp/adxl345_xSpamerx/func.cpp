#include "func.h"

bool selectDevice(int i2c, int addr, char * name)
{
   if (ioctl(i2c, I2C_SLAVE, addr) < 0)
   {
      fprintf(stderr, "%s not present\n", name);
      return false;
   }

   return true;
}
bool writeToDevice(int i2c, char * buf, int len)
{
   if (write(i2c, buf, len) != len)
   {
      fprintf(stderr, "Can't write to device\n");
      return false;
   }
  return true;
}
bool readADXL345(int &fd,short &x , short &y, short &z)
{
	if(!selectDevice(fd,ADXL345,"ADXL345"))
		return false;
	char buf[7];
	buf[0] = 0x32;													// This is the register we wish to read from
	if(!writeToDevice(fd,buf,2))
	{
 		return false;
	}
	
	if (read(fd, buf, 6) != 6) {								// Read back data into buf[]
		printf("Unable to read from slave\n");
		return false;
	}
	else {

		x = (buf[1]<<8) |  buf[0]; 
 		y = (buf[3]<<8) |  buf[2];
 		z = (buf[5]<<8) |  buf[4];

	}
	return true;
}
bool readL3G4200D(int &fd,short &x , short &y, short &z)
{
	if(!selectDevice(fd,L3G4200D,"L3G4200D"))
		return false;

	char buf[7];
	buf[0] = 0x28;													// This is the register we wish to read from
	if(!writeToDevice(fd,buf,2))
	{
 		return false;
	}
	
	if (read(fd, buf, 6) != 6) {								// Read back data into buf[]
		printf("Unable to read from slave\n");
		return false;
	}
	else {

		x = (buf[1]<<8) |  buf[0]; 
 		y = (buf[3]<<8) |  buf[2];
 		z = (buf[5]<<8) |  buf[4];
 		

	}
	return true;
}
bool init(int & fd)
{

														
	char *fileName = "/dev/i2c-1";								// Name of the port we will be using
											
	char buf[6];										// Buffer for data being read/ written on the i2c bus
	

	if ((fd = open(fileName, O_RDWR)) < 0) {					// Open port for reading and writing
		printf("Failed to open i2c port\n");
		return false;
	}
	
	if(!selectDevice(fd,ADXL345,"ADXL345"))
		return false;
	

	buf[0] = 0x2d;													// Commands for performing a ranging
	buf[1] = 0x18;
	
	if(!writeToDevice(fd,buf,2))
	{
 		return false;
	}

	

	buf[0] = 0x31;													// Commands for performing a ranging
	buf[1] = 0x0A; //09 4g , A 8g
	
	if(!writeToDevice(fd,buf,2))
	{
 		return false;
	}

	if(!selectDevice(fd,L3G4200D,"L3G4200D"))
		return false;
	 // Enable x, y, z and turn off power down:
	buf[0] = CTRL_REG1;													// Commands for performing a ranging
	buf[1] = 0x0F;
	
	if(!writeToDevice(fd,buf,2))
	{
 		return false;
	}
	// If you'd like to adjust/use the HPF, you can edit the line below to configure CTRL_REG2:
	buf[0] = CTRL_REG2;													// Commands for performing a ranging
	buf[1] = 0x00;
	
	if(!writeToDevice(fd,buf,2))
	{
 		return false;
	}
	// Configure CTRL_REG3 to generate data ready interrupt on INT2
    // No interrupts used on INT1, if you'd like to configure INT1
    // or INT2 otherwise, consult the datasheet:
	buf[0] = CTRL_REG3;													// Commands for performing a ranging
	buf[1] = 0x08;
	
	if(!writeToDevice(fd,buf,2))
	{
 		return false;
	}

	buf[0] = CTRL_REG4;													// Commands for performing a ranging
	buf[1] = 0x30;//0x30 - 2000
	
	if(!writeToDevice(fd,buf,2))
	{
 		return false;
	}

	buf[0] = CTRL_REG5;													// Commands for performing a ranging
	buf[1] = 0x00;
	
	if(!writeToDevice(fd,buf,2))
	{
 		return false;
	}

	return true;
}