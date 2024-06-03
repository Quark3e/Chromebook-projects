
#pragma once
#ifndef H_WIRELESS_COM
#define H_WIRELESS_COM


#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <memory.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <csignal>
#include <stdio.h>
#include <iostream>
#include <math.h>

#define MAXLINE 2048


class nodemcu_connect {
    private:
    int bind_result;
    int sock;
    const char* PORT = "53";
    const char* ADDRESS = "192.168.1.118";
    char szIP[100];
    sockaddr_in addrListen;
    sockaddr_storage addrDest;
    const char* toESP_msg;

    public:
    char buffer[MAXLINE];

    int resolvehelper(const char* hostname, int family, const char* service, sockaddr_storage* pAddr);
    void nodemcu_udp_setup();
    nodemcu_connect(const char* board_ip="192.168.1.118", const char* board_port="53") {
        ADDRESS = board_ip;
        PORT = board_port;

        nodemcu_udp_setup();
    }
    int receive(bool printResult);
};


class nodemcu_orient {
    public:
    
    // float orient[3] = {0,0,0}; //degrees
    float x_accel, y_accel, z_accel, pitch, roll, Pitch=0, Roll=0;
    bool useFilter = false;
    float accelFilter = 0.1;

    float* orientPtr;

    nodemcu_connect connectObj;
    const char* PORT = "53";
    const char* ADDRESS = "192.168.1.118";

    nodemcu_orient(
        float ptrOrient[3],
        const char* board_address="192.168.1.118",
        const char* board_port="53"
        ): connectObj(board_address, board_port) {
            orientPtr = ptrOrient;
            PORT = board_port;
            ADDRESS = board_address;
    }
    void update(bool printResult);
};



int nodemcu_connect::resolvehelper(
    const char* hostname,
    int family,
    const char* service,
    sockaddr_storage* pAddr
) {
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

int nodemcu_connect::receive(bool printResult=true) {
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
		// printf("\tSent %d bytes\t",bind_result);
		printf(" Read from server: \"%s\"  ",buffer);
	}
    return n;
}

void nodemcu_connect::nodemcu_udp_setup() {
	bind_result = 0;
	sock = socket(AF_INET, SOCK_DGRAM, 0);
	fcntl(sock, F_SETFL, O_NONBLOCK);
	addrListen = {};
	addrListen.sin_family = AF_INET;
	bind_result = bind(sock, (sockaddr*)&addrListen, sizeof(addrListen));
	if(bind_result==-1) {
		int lasterror = errno;
		std::cout << "bind() error:" << lasterror;
		exit(1);
	}
	addrDest = {};
	bind_result = resolvehelper(ADDRESS, AF_INET, PORT, &addrDest);
	if(bind_result!=0) {
		int lasterror = errno;
		std::cout << "resolvehelper error:" << lasterror;
		exit(1);
	}
	toESP_msg = "1";
}



/// @brief update/refresh orient values
/// @param printResult whether to print the received buffer and orient
void nodemcu_orient::update(bool printResult=true) {
    int n = connectObj.receive(printResult);
	// cout << "[n=" << n << " 0:\"" <<buffer[0] << "\" n-1:\"" << buffer[n-1] << "\" ]";
	std::string temp = "";
	if(connectObj.buffer[0]=='{' && connectObj.buffer[n-1]==';') { //{x:y:z}
		// if(printResult) cout << buffer << "\t";
		for(int i=0; i<n-1; i++) temp+=connectObj.buffer[i];
		x_accel = stof(temp.substr(1, temp.find(':')));
		temp.erase(0, temp.find(':')+1);
		y_accel = stof(temp.substr(0, temp.find(':')));
		temp.erase(0, temp.find(':')+1);
		z_accel = stof(temp.substr(0, temp.find('}')));
		temp.erase(0, temp.find(':')+1);
		std::string filtBoolStr = temp.substr(0, temp.find(';'));
		if(filtBoolStr == "off") useFilter = false;
		else if(filtBoolStr == "on ") useFilter = true;

		// if(x_accel>1 || y_accel>1 || z_accel>1 || x_accel<-1 || y_accel<-1 || z_accel<-1) return;
		if(x_accel>1) x_accel = 0.99;
		if(y_accel>1) y_accel = 0.99;
		if(z_accel>1) z_accel = 0.99;
		// if(printResult) printf("x_acc:%d\ty_acc:%d\tz_acc:%d\t",int(x_accel),int(y_accel),int(z_accel));

		pitch = atan(y_accel / sqrt(pow(x_accel,2)+pow(z_accel,2))) * 180 / M_PI; //degrees
		roll = atan(-1 * x_accel / sqrt(pow(y_accel,2)+pow(z_accel,2))) * 180 / M_PI; //degrees
		//pitch = -pitch;
		//roll = -roll;

		if(useFilter) {
			Pitch = (1-accelFilter) * Pitch + accelFilter * pitch;
			Roll = (1-accelFilter) * Roll + accelFilter * roll;
		}
		else {
			Pitch = pitch;
			Roll = roll;
			orientPtr[0] = Roll;
			orientPtr[1] = Pitch;
		}
		// int bPos = -1;
    	// if(Pitch <= 90 and Pitch >= -90) {
		// 	if(useFilter) orient[1] = Pitch * accelFilter + orient[1] * (1-accelFilter);
		// 	else orient[1] = Pitch;
		// 	if(orient[1] < 0) bPos = -1;
		// 	if(orient[1] > 0) bPos = 1;
		// }
    	// if(Roll <= 90 and Roll >= -90) {
		// 	if(useFilter) orient[0] = Roll * accelFilter + orient[0] * (1-accelFilter);
		// 	else orient[0] = Roll;
		// 	orient[0] = orient[0] * bPos;
		// }
		if(printResult) printf(" Roll:%3d Pitch:%3d ", 
		int(Roll), int(Pitch));
	}
}

#endif