#include "./include/serial.h"

Serial::Serial() {

	// Set up initial values
	dev = "/dev/ttyUSB0";
	baud = 9600;
	dataBits = 8;
	bufferSize = 1000;
	parity = PARITY_8N1;
	bufferIndex = 0;
	blocking = 0;
}

Serial::~Serial() {

	// Kills the port
	closePort();
}

int Serial::initPort() {
	
	// Try to open serial port with r/w access
	printf("SERIAL: Opening %s at %i baud...", dev, baud);
	fd = open(dev, O_RDWR | O_NOCTTY | O_NDELAY);

        // As long as port is actually open...
	if(fd != -1) {

		// Share the good news
		printf("OK\n");

		// Display blocking info
		if (blocking == 1) printf("SERIAL: Blocking enabled\n");
		else printf("SERIAL: Blocking disabled\n");

		// Configure port settings
		fcntl(fd, F_SETFL, FNDELAY);

		// Save current port settings so we don't corrupt anything on exit
		struct termios options;
		tcgetattr(fd, &options);

		// Convert integer baud to Baud type
		// Default to 9600 baud if none specified
		switch (baud) {
			case 4800:
				cfsetispeed(&options, B4800);
				cfsetospeed(&options, B4800);
				break;
			case 9600:	
				cfsetispeed(&options, B9600); 
				cfsetospeed(&options, B9600); 		
				break;
			case 38400:	
				cfsetispeed(&options, B38400); 
				cfsetospeed(&options, B38400); 				
				break;
			case 57600:	
				cfsetispeed(&options, B57600); 
				cfsetospeed(&options, B57600); 				
				break;
			case 115200:	
				cfsetispeed(&options, B115200); 
				cfsetospeed(&options, B115200); 				
				break;
			default:	
				cfsetispeed(&options, B9600); 
				cfsetospeed(&options, B9600); 				
				break;
		}

		// Set options for proper port settings 
		//	ie. 8 Data bits, No parity, 1 stop bit
		options.c_cflag |= (CLOCAL | CREAD);
		
		switch (parity) {
			case PARITY_7E1:
				// Parity, odd, 1 stop bit (7E1)
				options.c_cflag |= PARENB;
				options.c_cflag &= ~PARODD;
				options.c_cflag &= ~CSTOPB;
				printf("SERIAL: Parity set to 7E1\n");
				break;
			case PARITY_8N1:
				// No parity, 1 stop bit (8N1)
				options.c_cflag |= CS8;
				printf("SERIAL: Parity set to 8N1\n");
				break;
			default:
				// No parity, 1 stop bit (8N1)
				options.c_cflag |= CS8;
				printf("SERIAL: No parity set\n");
				break;
		}			

		// Set number of data bits.  Default is 8.
		switch (dataBits) {
			case 7:		
				options.c_cflag |= CS7; 
				printf("SERIAL: Databits set to 7\n");
				break;
			case 8:		
				options.c_cflag |= CS8; 
				printf("SERIAL: Databits set to 8\n");
				break;
			default:	
				options.c_cflag |= CS8; 
				printf("SERIAL: Databits not set!\n");
				break;
		}

		// Turn off hardware flow control
		options.c_cflag &= ~CRTSCTS;
		
		options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);


		// Write our changes to the port configuration
		tcsetattr(fd, TCSANOW, &options);
	}

	// There was a problem, let's tell the user what it was
	else {
		printf("FAIL\n");
		perror("Error opening port:");
		closePort();
		exit(1);
	}

	// Send back the public port file descriptor
	return fd;
}

void Serial::flushPort() {

	// If the port is actually open, flush it
	if (fd != -1) ioctl(fd, TCFLSH, 2);
}

int Serial::getData(char* data) {

	// If the port is actually open, read the data
	if (fd != -1) {
		// Grab the data and return the nubmer of bytes actually read
		return read(fd, data, sizeof(data));
	}
	// Port is closed!
	else return -1;
}

int Serial::sendData(char* data) {

	// If the port is actually open, send the data
	if (fd != -1) {
		// Send the data and return the nubmer of bytes actually written
		//printf("Writing %s...\n", data);
		return write(fd, data, sizeof(data));
	}
	// Port is closed!
	else return -1;
}

char Serial::getChar() {
	int delay = 0;
	switch (blocking) {
		// Non-blocking mode enable, so use the buffer
		case 0:
			if (bufferIndex > 0) {
				// Yes, so grab the byte we need and shift the buffer left
				char c = buffer[1];
				for (int j = 1; j < (bufferIndex + 1); j++) {
					buffer[j] = buffer[j + 1];
				}
				bufferIndex--;
				// Send back the firt byte in the buffer
				return c;
			}
			// If the port is actually open, grab a byte
			if (fd != -1) {
				// Return the byte received if it's there
				int n = read(fd, &temp[0], 1);
				if (n > -1) {
					// Just 1 byte, so return it right away
					if (n == 1) return temp[0];
					// More than 1 byte there, so buffer them and return the first
					else {
						for (int i = 0; i < (n - 1); i++) {
							bufferIndex++;
							buffer[bufferIndex] = temp[i + 1];
						}
						return temp[0];
					}
				}
				else return -1;
			}
			break;
		// Blocking mode enabled, so wait until we get a byte
		case 1:
			if (bufferIndex > 0) {
				// Yes, so grab the byte we need and shift the buffer left
				char c = buffer[1];
				for (int j = 1; j < (bufferIndex + 1); j++) {
					buffer[j] = buffer[j + 1];
				}
				bufferIndex--;
				// Send back the firt byte in the buffer
				return c;
			}
			// If the port is actually open, grab a byte
			if (fd != -1) {
				// Return the byte received if it's there
				int n = -1;
				while (n < 1) {
					n = read(fd, &temp[0], 1);
					if (n == -1) usleep(10000);	// wait 10ms
					delay++;
					if (delay == 5) return -1;
				}
				if (n > -1) {
					// Just 1 byte, so return it right away
					if (n == 1) return temp[0];
					// More than 1 byte there, so buffer them and return the first
					else {
						for (int i = 0; i < (n - 1); i++) {
							bufferIndex++;
							buffer[bufferIndex] = temp[i + 1];
						}
						return temp[0];
					}
				}
				else return -1;
			}
			// Port is closed!
			else return -1;
			break;
		// Blocking variable is messed up
		default:
			printf("SERIAL: Error with blocking setting!\n");
			return -1;
			break;
	}
	// Should never get here
	return -1;
}

int Serial::sendChar(char data) {
	switch (blocking) {
		// Non-blocking mode
		case 0:
			// If the port is actually open, send a byte
			if (fd != -1) {
				// Send the data and return number of bytes actually written
				write(fd, &data, 1);
				return 1;
			}
			else return -1;
			break;
		// Blocking mode
		case 1:
			// If the port is actually open, send a byte
			if (fd != -1) {
				// Send the data and return number of bytes actually written
				write(fd, &data, 1);
				return 1;
			}
			else return -1;
			break;
		// Blocking variable is messed up
		default:
			printf("SERIAL: Error with blocking setting!\n");
			return -1;
			break;
	}
}

void Serial::closePort() {

	// If the port is actually open, close it
	if (fd != -1) {
		close(fd);
		printf("SERIAL: Device %s is now closed.\n", dev);
	}
}