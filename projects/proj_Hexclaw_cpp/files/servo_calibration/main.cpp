
#include <iostream>
#include <math.h>
#include <stdio.h>
#include <string>
#include <string.h>
#include <unistd.h>
#include <fstream>
#include <cstdlib>
#include <chrono>
#include <ctime>
#include <algorithm>

//nodemcu/esp8266 module udp communication
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <memory.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <errno.h>
#include <stdlib.h>
#include <fcntl.h>
#include <poll.h>
#include <csignal>

#include "PiPCA9685/PCA9685.h"

using namespace std;
using namespace PiPCA9685;


#define MAXLINE 2048

int bind_result;
int sock;
char buffer[MAXLINE];
const char* PORT;
char szIP[100];
sockaddr_in addrListen;
sockaddr_storage addrDest;
const char* toESP_msg;
float orient[3] = {0,0,0}; /*[degrees]; 
pitch/orient[1] for rotational joints (s[1,2,4])
roll/orient[0] for twisting joints (s[3,5])
*/
float x_accel, y_accel, z_accel, pitch, roll, Pitch=0, Roll=0;

bool useFilter = false;
float accelFilter = 0.2;

bool usebPosCorrection = false;

bool motorTested[6] = {false, false, false, false, false, false};

int resolvehelper(const char* hostname, int family, const char* service, sockaddr_storage* pAddr)
{
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
    cout << "\nsetting up nodemcu_udp COM settings... ";
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
    cout << "finished\n";
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
		x_accel = stof(temp.substr(1, temp.find(':')));
		temp.erase(0, temp.find(':')+1);
		y_accel = stof(temp.substr(0, temp.find(':')));
		temp.erase(0, temp.find(':')+1);
		z_accel = stof(temp.substr(0, temp.find('}')));
		temp.erase(0, temp.find(':')+1);
		string filtBoolStr = temp.substr(0, temp.find(';'));
		if(filtBoolStr == "off") useFilter = false;
		else if(filtBoolStr == "on ") useFilter = true;

		// if(x_accel>1 || y_accel>1 || z_accel>1 || x_accel<-1 || y_accel<-1 || z_accel<-1) return;
		if(x_accel>1) x_accel = 0.99;
		if(y_accel>1) y_accel = 0.99;
		if(z_accel>1) z_accel = 0.99;
		if(printResult) printf("x_acc:%f\ty_acc:%f\tz_acc:%f\t",x_accel,y_accel,z_accel);

		pitch = atan(y_accel / sqrt(pow(x_accel,2)+pow(z_accel,2))) * 180 / M_PI; //degrees
		roll = atan(-1 * x_accel / sqrt(pow(y_accel,2)+pow(z_accel,2))) * 180 / M_PI; //degrees

		if(useFilter) {
			Pitch = (1-accelFilter) * Pitch + accelFilter * pitch;
			Roll = (1-accelFilter) * Roll + accelFilter * roll;
		}
		else {
			Pitch = pitch;
			Roll = roll;
		}
		int bPos = -1;
    	if(Pitch <= 90 and Pitch >= -90) {
			if(useFilter) orient[1] = Pitch * 0.9 + orient[1] * 0.1;
			else orient[1] = Pitch;
			if(orient[1] < 0) bPos = -1;
			if(orient[1] > 0) bPos = 1;
		}
    	if(Roll <= 90 and Roll >= -90) {
			if(useFilter) orient[0] = Roll * accelFilter + orient[0] * (1-accelFilter);
			else orient[0] = Roll;
			if(usebPosCorrection) orient[0] = orient[0] * bPos;
		}
		if(printResult) printf("a:%d \tb:%d  \tRoll:%d  \tPitch:%d", 
		int(orient[0]), int(orient[1]), int(Roll), int(Pitch));
	}
}

float q_corrections(int q, float angle) {
    float error_Consts[6] = {
        0.802139037433155,
        0.7538,
        0.8772,
        1.0345,
        1,
        1
    };
	if(q==1) angle+=10;
	return angle * error_Consts[q];
}


void to_sendToServo(
    PCA9685* pcaBoard,
    int servoToMove,
    int angle
) {
    pcaBoard->set_pwm(servoToMove, 0, round(400*(float(angle)/180))+100);
}

float readAngles[6][18]; //stores read values which is at function end appended to readDelays.dat

int main(int argc, char** argv) {
    nodemcu_udp_setup();
    
    ofstream delayFile;
    delayFile.open("readDelays.dat", ios_base::app);

    for(int i=0; i<6; i++) {for(int j=0; j<18; j++) {readAngles[i][j]=0;}}

    PCA9685 pca{};
    pca.set_pwm_freq(50.0);

				//measured: //	90	90
    int angleDefaults[6] = {90, 90, 135, 90, 90, 90};
    float readValues[20];
    int servoToMove=0;
    string input;
    while(true) {
        for(int i=0; i<6; i++) to_sendToServo(&pca, i, q_corrections(i, angleDefaults[i]));
        cout << "--- Servo motor calibration ---\n";
        cout << "enter servo to test [0-5]: ";
        cin >> input;
        if(input=="exit") break;
        servoToMove = stoi(input);
        cin.clear();
        cin.ignore();

        //display:  testing motor by sending commands preliminary
        printf("\nprelim. angles sent:\t");
        for(int angle=0; angle<=180; angle+=10) {
            printf(" %d", angle);
            sendToServo(&pca, servoToMove, q_corrections(servoToMove, angle));
            usleep(300'000);
        }
        usleep(3'000'000);

        //tracking: actual sending and reading motor error readings
        printf("\ntrack. angles:\n");
        for(int i=0; i<=18; i+=1) {
			updateOrients(false);
			
			sendToServo(&pca, servoToMove, q_corrections(servoToMove, i*10));
			
			if(i==0) usleep(2'000'000); 
            usleep(4'000'000);
            updateOrients(false);
            printf("sent: %d read: ", i*10);
            // cin >> input;
            // if(input=="exit") return 0;
			updateOrients(false);
            if(servoToMove==0) {}
            else if(servoToMove==1 || servoToMove==2 || servoToMove==4) { readValues[i] = 90+orient[0]; }
            else if(servoToMove==3 || servoToMove==5) { readValues[i] = 90-orient[0]; }
			printf("%d\n", int(round(readValues[i])));
            // readValues[i] = stof(input);
            // cin.clear();
            // cin.ignore();
			if(i>=18) {
				for(int n=1; n<=19; n++) {
					readValues[n-1]=readValues[n];
            		readAngles[servoToMove][n-1] = readValues[n-1];
				}
			}
			motorTested[servoToMove] = true;
        }

        printf("\n\nresults:\nsent: ");
        for(int i=0; i<=18; i++) { printf(" %d", i*10); }

        printf("\nread: ");
        for(int i=0; i<=18; i++) { printf(" %d", int(round(readValues[i]))); }
        printf("\npaused: ");
        cin.get();
        cin.clear();
        cin.ignore();
        system("clear");
        sendToServo(&pca, servoToMove, q_corrections(servoToMove, angleDefaults[servoToMove]));
    }

    auto currentTime = chrono::system_clock::now();
    time_t currentDate = chrono::system_clock::to_time_t(currentTime);
    // struct tm* p = localtime(&currentDate);
    // char temp_s[1000];
    // strftime(temp_s, 1000, "%A, %B %d %Y", p);
    char* t = ctime(&currentDate);
    if(t[strlen(t)-1] == '\n') t[strlen(t)-1] = '\0';

    //delayFile << "\n";
    delayFile << "\ndate:\"" << t << "\";\n";
    for(int q=0; q<6; q++) {
		if(!motorTested[q]) {}
		else {
			delayFile << to_string(q) + ":" + to_string(readAngles[q][0]);
			for(int i=1; i<=18; i++) delayFile << "," + to_string(readAngles[q][i]);
			delayFile << ";\n";
		}
	}

    delayFile.close();

    return 0;
}
