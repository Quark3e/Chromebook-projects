
//nodemcu/esp8266 module udp communication
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <memory.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <err.h>
#include <fcntl.h>
#include <poll.h>
#include <csignal>

#define MAXLINE 2048

//default headers/incudes
#include <iostream>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <fstream>
#include <algorithm>
#include <unistd.h>
#include <chrono>

//pca9685 communication
#include <PiPCA9685/PCA9685.h>

#include "/home/pi/Chromebook-projects/projects/proj_Hexclaw_cpp/HexclawMain/IK_header.hpp"

using namespace std;

//udp com related: udp communication variable declarations
int bind_result;
int sock;
char buffer[MAXLINE];
const char* PORT;
char szIP[100];
sockaddr_in addrListen;
sockaddr_storage addrDest;
const char* toESP_msg;

//IK related: ik calc variable declaration
float current_q[6] = {0,0,0,0,0,0}; //old_rotation
float new_q[6] = {0,0,0,0,0,0};
float orient[3] = {0,0,0}; //degrees
float PP[3] = {0,150,250};


bool moveServos = false;

//Acceleroemter related: storing raw and manipulated values from accelerometer (received via wifi/udp from nodemcu board)
float accelRaw[3] = {0, 0, 0}; //store raw values received from nodemcu_board/accelerometer
float accelFin[3] = {0, 0, 0}; //"finished" and presented values that is used in the movement calculated e.t.c.
float accelOffset[3] = {0, 0, 1}; //NOTE: it is under the assumption that the controller doesnt tilt in any way.
// float accelThresh[3] = {0.5, 0.5, 0.5}; //threshold where if abs(readAccel[i])>accelThresh[i] then PP[i] is moved
float accelScalar[3] = {1, 1, 1}; //read-sent adjustment variable

float v_0[3] = {0, 0, 0};
float elapsedTime = 0; //microseconds

bool calibrateOffset = false;
bool useFilter = false;
float accelFilter = 0.01;

int resolvehelper(const char* hostname, int family, const char* service, sockaddr_storage* pAddr) {
    int result;
    addrinfo* result_list = NULL;
    addrinfo hints = {};
    hints.ai_family = family;
    hints.ai_socktype = SOCK_DGRAM; // without this flag, getaddrinfo will return 3x the number of addresses (one for each socket type).
    result = getaddrinfo(hostname, service, &hints, &result_list);
    if (result == 0)
    {
        //ASSERT(result_list->ai_addrlen <= sizeof(sockaddr_in));
        memcpy(pAddr, result_list->ai_addr, result_list->ai_addrlen);
        freeaddrinfo(result_list);
    }

    return result;
}

void nodemcu_udp_setup() {
	bind_result = 0;
	sock = socket(AF_INET, SOCK_DGRAM, 0);
	fcntl(sock, F_SETFL, O_NONBLOCK);
	PORT = "53";
	addrListen = {};
	addrListen.sin_family = AF_INET;
	bind_result = bind(sock, (sockaddr*)&addrListen, sizeof(addrListen));
	if(bind_result==-1) {
		int lasterror = errno;
		cout << "bind() error:" << lasterror;
		exit(1);
	}
	addrDest = {};
	bind_result = resolvehelper("192.168.1.117", AF_INET, PORT, &addrDest);
	if(bind_result!=0) {
		int lasterror = errno;
		cout << "resolvehelper error:" << lasterror;
		exit(1);
	}
	toESP_msg = "1";
}

void updateOrients(bool printResult) {
	size_t msg_length = strlen(toESP_msg);
	bind_result = sendto(
		sock, toESP_msg, msg_length, 0,
		(sockaddr*)&addrDest, sizeof(addrDest)
		);
	socklen_t len;
	int n = recvfrom(
		sock, (char*)buffer, MAXLINE, MSG_WAITALL,
		(struct sockaddr*)&addrDest, &len);
	buffer[n] = '\0';
	if(printResult) {
		printf("\tSent %d bytes\t",bind_result);
		printf("Read from server: \"%s\"\t",buffer);
	}
	string temp = "";
	
	if(buffer[0]=='{' && buffer[n-1]==';') { //{x:y:z}
		if(printResult) cout << buffer << "\t";
		for(int i=0; i<n-1; i++) temp+=buffer[i];
		accelRaw[0] = stof(temp.substr(1, temp.find(':')));
		temp.erase(0, temp.find(':')+1);
		accelRaw[1] = stof(temp.substr(0, temp.find(':')));
		temp.erase(0, temp.find(':')+1);
		accelRaw[2] = stof(temp.substr(0, temp.find('}')));
		temp.erase(0, temp.find(':')+1);
		string filtBoolStr = temp.substr(0, temp.find(';'));
		if(filtBoolStr == "off") calibrateOffset = false; //"off" is default
		else if(filtBoolStr == "on ") calibrateOffset = true;

		// if(accelRaw[0]>1 || accelRaw[1]>1 || accelRaw[2]>1 || accelRaw[0]<-1 || accelRaw[1]<-1 || accelRaw[2]<-1) return;
		if(accelRaw[0]>1) accelRaw[0] = 0.99;
		if(accelRaw[1]>1) accelRaw[1] = 0.99;
		if(accelRaw[2]>1) accelRaw[2] = 0.99;
		if(printResult) printf("x_acc:%f\ty_acc:%f\tz_acc:%f\t",accelRaw[0],accelRaw[1],accelRaw[2]);
        if(useFilter) {
            accelFin[0] = (1-accelFilter)*accelFin[0] + accelFilter*accelRaw[0];
            accelFin[1] = (1-accelFilter)*accelFin[1] + accelFilter*accelRaw[1];
            accelFin[2] = (1-accelFilter)*accelFin[2] + accelFilter*accelRaw[2];
        }
        else {
            for(int i=0; i<3; i++) accelFin[i] = accelRaw[i];
        }
        for(int i=0; i<3; i++) accelFin[i]=accelFin[i]-accelOffset[i];
    }
}

void movePosition(int axis, float elapsedTime) {
    //note: elapsedTime is in microseconds
    v_0[axis] = v_0[axis] + float(((accelFin[axis]))*accelScalar[axis])*(elapsedTime/1'000'000);
    PP[axis] += v_0[axis]*(elapsedTime/1'000'000)/2;
}

int main(int argc, char** argv) {
    nodemcu_udp_setup();

    PiPCA9685::PCA9685 pca{};
    pca.set_pwm_freq(50.0);
    if(moveServos) sendToServo(&pca, new_q, current_q, true, 0);

    auto t1 = chrono::high_resolution_clock::now();
    auto t2 = chrono::high_resolution_clock::now();

    while(true) {
        usleep(100'000);
        updateOrients(false);
        if(calibrateOffset) {
            cout << "MODE: Calibrating accelerometer offset\n";
            cout << "\tkeep it still!\n";
            usleep(1'000'000);
            cout << "\tstarting measurements.";
            for(int i=0; i<3; i++) {
                usleep(1'000'000);
                cout << " .";
                updateOrients(false);
            }
            accelOffset[0] = accelRaw[0];
            accelOffset[1] = accelRaw[1];
            accelOffset[2] = accelRaw[2];
            cout << "\n\toffset set. Measurement taking finished\n\n";
            calibrateOffset = false;
        }

        t2 = chrono::high_resolution_clock::now();
        float timeDur = (chrono::duration_cast<chrono::microseconds>(t2-t1)).count();
        for(int i=0; i<3; i++) {
            // if(accelFin[i]>=accelThresh[i]) {
            movePosition(i, timeDur);
            // }
        }
        printf("%d %d %d \t %f %f %f \t %f %f %f \t", 
        int(round(PP[0])), int(round(PP[1])), int(round(PP[2])),
        accelFin[0], accelFin[1], accelFin[2],
        v_0[0], v_0[1], v_0[2]
        );
        cout << timeDur/1000000;
        t1 = chrono::high_resolution_clock::now();

        if(moveServos) {
            if(getAngles(new_q,PP,toRadians(orient[0]),toRadians(orient[1]),toRadians(orient[2]),1)) {
				// printf("a:%d\tb:%d", int(orient[0]), int(orient[1]));
				// printf("\tangles:\t%d\t%d\t%d\t%d\t%d\t%d",
				// int(new_q[0]),int(new_q[1]),int(new_q[2]),int(new_q[3]),int(new_q[4]),int(new_q[5]));
				sendToServo(&pca,new_q,current_q,false);
			}
			else {printf("\tPP-orient not reachable");}
			printf("\n");
        }
        printf("\n");
    }

    return 0;
}


