
#include "wirelessCOM.hpp"



int nodemcu_connect::init() {
	if(this->_init) return 0;

	if(this->_verbose) std::cout << this->_info_name+"::init() called: Starting with socket for server: "<<this->_ipAddr << " " << this->_port << std::endl;
#if _WIN32
	WSADATA wsaData;
	int wsaerr;
	WORD wVersionRequested = MAKEWORD(2, 2);
	wsaerr = WSAStartup(wVersionRequested, &wsaData);

	if(wsaerr!=0) {
		std::cout << this->_info_name<<"::init() "<< "ERROR: winsock dll not found." << std::endl;
		return -1;
	}
	else {
		if(this->_verbose) std::cout << this->_info_name<<"::init() "<< "winsock dll found. status: "<<wsaData.szSystemStatus << std::endl;
	}

	this->_sock = INVALID_SOCKET;
#else
#endif
	memset(&_server_sockaddr_in, 0, sizeof(_server_sockaddr_in));
	
	this->_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
#if _WIN32
	if(this->_sock == INVALID_SOCKET) {
		std::cout << this->_info_name<<"::init() "<< "ERROR: socket creation failed: "<< WSAGetLastError() << std::endl;
		WSACleanup();
		return -1;
	}
#else
	if(this->_sock < 0) {
		std::cout << this->_info_name<<"::init() "<< "ERROR: socket creation failed." << std::endl;
		return -1;
	}
#endif
	this->_socket_closed = false;

	if(this->_verbose) std::cout << this->_info_name+"::init() socket successfully created." << std::endl;
	
	this->_server_sockaddr_in.sin_family 	= AF_INET;
	this->_server_sockaddr_in.sin_port		= htons(this->_port);
	this->_server_sockaddr_in.sin_addr.s_addr	= inet_addr(this->_ipAddr.c_str());

#if _WIN32
	if(this->timeout>0)
#else
	if(this->timeout.tv_sec>0 || this->timeout.tv_usec>0)
#endif
	{
		if(setsockopt(this->_sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout))!=0) {
			std::cout << this->_info_name<<"::init() "<< "ERROR: recv timeout setsockopt failed." << std::endl;
		}
	}
	
	this->_sockAddrLen = sizeof(sockaddr_in);

	this->_init = true;
	return 0;
}
nodemcu_connect::nodemcu_connect(std::string _board_IP, int _board_PORT): _ipAddr(_board_IP), _port(_board_PORT) {
	this->init();
}
nodemcu_connect::nodemcu_connect(bool _initialise) {
	if(_initialise) this->init();
}
nodemcu_connect::nodemcu_connect(bool _initialise, std::string _board_IP, int _board_PORT): _ipAddr(_board_IP), _port(_board_PORT) {
	if(_initialise) this->init();
}

nodemcu_connect::~nodemcu_connect() {
	if(!this->_init) {
		// std::cout << this->_info_name<<"::~nodemcu_connect() NOTE: this class wasn't initialised when its destructor is now called." << std::endl;
		return;
	}
	if(!this->_socket_closed) {
#if _WIN32
		closesocket(this->_sock);
		WSACleanup();
#else
		close(this->_sock);
#endif
		this->_socket_closed = true;
	}
	std::cout << this->_info_name<<"::~nodemcu_connect() socket closed." << std::endl;
}

int nodemcu_connect::request(bool _printResult) {
	if(!this->_init) {
		std::cout << this->_info_name<<"::request(bool) ERROR: class has not been initialised." << std::endl;
		return -1;
	}
	this->bytesSent = sendto(
		this->_sock,
		this->requestMsg.c_str(),
		this->requestMsg.size(),
		0,
		(sockaddr*)&this->_server_sockaddr_in,
		sizeof(sockaddr_in)
	);
	if(this->bytesSent < 0) {
		std::cout << this->_info_name<<"::request(bool) "<< "ERROR: failed at sendto()";
#if _WIN32
		std::cout << ": " << WSAGetLastError();
#else
		std::cout << ".";
#endif
		std::cout << std::endl;
		return -1;
	}
	// if(_printResult) std::cout << this->_info_name+"::request(bool) sent "<<bytesSent<<" bytes" << std::endl;

	this->bytesRecv = recvfrom(
		this->_sock,
		this->buffer,
		MAXLINE,
		0,
		(sockaddr*)&this->_server_sockaddr_in,
		&this->_sockAddrLen
	);
	if(this->bytesRecv < 0) {
		std::cout << this->_info_name<<"::request(bool) "<< "ERROR: failed at recvfrom()";
#if _WIN32
		std::cout << ": " << WSAGetLastError();
#else
		std::cout << ".";
#endif
		std::cout << std::endl;
		return -1;
	}
	// if(_printResult) std::cout << this->_info_name+"::request(bool) received "<<bytesRecv<<" bytes" << std::endl;
	this->buffer[this->bytesRecv] = '\0';
	if(_printResult) {
		std::cout << "\""<< buffer << "\" ";
	}
	return this->bytesRecv;
}
bool nodemcu_connect::isInit() {
	return this->_init;
}


nodemcu_orient::nodemcu_orient(bool initialise): connectObj(initialise, DEFAULT__IPADDR, DEFAULT__PORT) {}
nodemcu_orient::nodemcu_orient(float ptrOrient[3], bool initialise): connectObj(initialise, DEFAULT__IPADDR, DEFAULT__PORT) {
	assert(ptrOrient);
	orientPtr = ptrOrient;
}
nodemcu_orient::nodemcu_orient(std::string board_address, int board_port, bool initialise): connectObj(initialise, board_address, board_port) {

}
nodemcu_orient::nodemcu_orient(float ptrOrient[3], std::string board_address, int board_port, bool initialise): connectObj(initialise, board_address, board_port) {
	assert(ptrOrient);
	orientPtr = ptrOrient;
}

bool nodemcu_orient::parseData(std::string _str) {

	/// Example: "{0.11:-0.11:1.02;-0.03:-0.01:-0.01}11;"

	accel.x = stof(_str.substr(1, _str.find(':'))) - accel_offs.x;
	_str.erase(0, _str.find(':')+1);
	accel.y = stof(_str.substr(0, _str.find(':'))) - accel_offs.y;
	_str.erase(0, _str.find(':')+1);
	accel.z = stof(_str.substr(0, _str.find(';'))) - accel_offs.z;
	_str.erase(0, _str.find(';')+1);
	
	gyro.x = stof(_str.substr(1, _str.find(':')));
	_str.erase(0, _str.find(':')+1);
	gyro.y = stof(_str.substr(0, _str.find(':')));
	_str.erase(0, _str.find(':')+1);
	gyro.z = stof(_str.substr(0, _str.find('}')));
	_str.erase(0, _str.find('}')+1);

	return true;
}
void nodemcu_orient::update(bool printResult) {
	if(!connectObj.isInit()) {
		std::cout << this->_info_name << "::update(bool) ERROR: connectObj has not been initialised." << std::endl;
		return;
	}
    int n = connectObj.request(printResult);
	if(n<0) throw std::runtime_error("nodemcu_orient connectObj.request() failed. timeout.");
	std::string temp = std::string(connectObj.buffer);
	if(connectObj.buffer[0]=='{' && connectObj.buffer[n-1]==';') { //{x:y:z}
		if(!this->parseData(connectObj.buffer)) {
			return;
		}

		size_t _endCurlPos = temp.find('}') + 1;
		if(_endCurlPos==std::string::npos) {
			std::cout << this->_info_name<<"::update(bool) "<< "ERROR: received string doesn't contain '}'." << std::endl;
			return;
		}
		std::string filtBoolStr = temp.substr(_endCurlPos, temp.find(';', _endCurlPos));

		if(filtBoolStr == "off") useFilter = false;
		else if(filtBoolStr == "on ") useFilter = true;

		// if(accel.x>1 || accel.y>1 || accel.z>1 || accel.x<-1 || accel.y<-1 || accel.z<-1) return;
		// if(accel.x>1) accel.x = 0.9999;
		// if(accel.y>1) accel.y = 0.9999;
		// if(accel.z>1) accel.z = 0.9999;
		// if(printResult) printf("x_acc:%d\ty_acc:%d\tz_acc:%d\t",int(accel.x),int(accel.y),int(accel.z));

		pitch = atan(accel.y / sqrt(pow(accel.x,2)+pow(accel.z,2))) * 180 / M_PI; //degrees
		roll = atan(-1 * accel.x / sqrt(pow(accel.y,2)+pow(accel.z,2))) * 180 / M_PI; //degrees
		//pitch = -pitch;
		//roll = -roll;

		if(useFilter) {
			Pitch = (1-accelFilter) * Pitch + accelFilter * pitch;
			Roll = (1-accelFilter) * Roll + accelFilter * roll;
		}
		else {
			Pitch = pitch;
			Roll = roll;
			if(orientPtr) {
				orientPtr[0] = Roll;
				orientPtr[1] = Pitch;
			}
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
