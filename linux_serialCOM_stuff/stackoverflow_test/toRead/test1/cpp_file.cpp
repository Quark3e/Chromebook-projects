//Nathax - arduino forum

#include <iostream>
#include <stdio.h>
#include <string>
#include <string.h>
#include <chrono>
#include <thread>

using namespace std;
using namespace chrono;

char serialPortFilename[] = "/dev/ttyUSB0";

int main() {
    
    char readBuffer[1024];
    int numBytesRead;

    FILE *serPort = fopen(serialPortFilename, "r");

	if (serPort == NULL) {
		printf("ERROR");	
		return 0;
	}

	printf(serialPortFilename);
	printf(":\n");
	while(true) {
		memset(readBuffer, 0, 1024);
		fread(readBuffer, sizeof(char),1024,serPort);
		if(sizeof(readBuffer) != 0)
		{
			printf(readBuffer);
		}
	}
}