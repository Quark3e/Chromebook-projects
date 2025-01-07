
#pragma once
#ifndef H_WIRELESS_COM
#define H_WIRELESS_COM


#if _WIN32
#pragma comment(lib, "Ws2_32.lib")
#pragma warning(disable:4996)

#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <bits/stdc++.h> 
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>
#endif

#include <string>
#include <iostream>
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <math.h>
#include <cassert>

#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170679 //82148086513282306647093844609550582231725359408128481117450284102701938521105559644622948954930381964428810975665933446128475648233786783165271201909145648566923460348610454326648213393607260249141273724587006 
#endif

#define MAXLINE 2048

/**
 * Default ipaddress for the remote esp8266 board.
 *  - "nodemcu board"   - "192.168.1.177"
 *  - "oled hw board"   - "192.168.1.231"
 *  - "unknown"         - "192.168.1.118"
 */
#define DEFAULT__IPADDR "192.168.1.117"
/**
 * Default port that the esp8266 board is bound to.
 *  - "nodemcu board"   - 1089
 *  - "oled hw board"   - 1089 (might be 1087 according to chromeos linux env portforward saved settings)
 *  - "unknown"ABC      - 53
 */
#define DEFAULT__PORT   1089


template <class _store_type>
struct vec3 {
    _store_type x;
    _store_type y;
    _store_type z;
    
    _store_type& operator[](size_t _axis) {
        switch (_axis) {
        case 0: return this->x; break;
        case 1: return this->y; break;
        case 3: return this->z; break;
        default:
            throw std::invalid_argument("_axis argument is invalid.");
            break;
        }
    }
    
    /// @brief Function to call for printing purposes with number formatting
    void (*_callFunc)(float, std::string&) = nullptr;

    friend auto operator<<(std::ostream &os, vec3 const& m) -> std::ostream& {
        std::string _temp = "";
        os << "{";
        if(m._callFunc) {
            m._callFunc(m.x, _temp);
            os << _temp << ", ";
            m._callFunc(m.y, _temp);
            os << _temp << ", ";
            m._callFunc(m.z, _temp);
            os << _temp;
        }
        else {
            os << m.x << ", ";
            os << m.y << ", ";
            os << m.z;
        }
        os << "}";
        return os;
    }
    size_t size() {
        return 3;
    }
};



class nodemcu_connect {
    private:
    std::string _info_name = "nodemcu_connect";

    /// Whether the class has been initialised.
    bool _init = false;

#if _WIN32
    SOCKET  _sock;
#else
    int  _sock;
#endif
    std::string _ipAddr     = DEFAULT__IPADDR;
    int         _port       = DEFAULT__PORT;

    sockaddr_in _server_sockaddr_in;
    bool        _socket_closed  = true;
    int         _sockAddrLen    = sizeof(sockaddr_in);

    int _func_init();
    public:
    int ms_timeout = 5000;

    nodemcu_connect(std::string _board_IP, int _board_PORT);
    ~nodemcu_connect();

    /**
     * @brief Request new data from the esp8266 board
     * 
     * @param _printResult whether to print the received data
     * @return int of number of bytes received
     */
    int request(bool _printResult = true);

    /// @brief Number of bytes received from the esp8266 board
    int bytesRecv = 0;
    /// @brief number of bytes sent to the esp8266 board from latest sendto call.
    int bytesSent = 0;
    /// @brief the string sent to the esp8266 board
    std::string requestMsg = "Hello";
    /// @brief char buffer of the received string
    char buffer[MAXLINE];

};

/**
 * Dedicated class for telemetry data from esp8266 board connected to an accelerometer board
 */
class nodemcu_orient {
    std::string _info_name = "nodemcu_orient";
    public:
    
    // float orient[3] = {0,0,0}; //degrees

    /// Container for accelerometer data for the across XYZ-axes. [unit: G]
    vec3<float> accel{0, 0, 0};
    /// Container for gyroscopic data for the XYZ-axes
    vec3<float> gyro{0, 0, 0};

    // float x_accel = 0, y_accel = 0, z_accel = 0;
    float pitch, roll, Pitch=0, Roll=0;
    bool useFilter = false;
    float accelFilter = 0.1;

    float* orientPtr = nullptr;

    nodemcu_connect connectObj;

    nodemcu_orient(
        float ptrOrient[3],
        std::string board_address   = DEFAULT__IPADDR,
        int board_port              = DEFAULT__PORT
    ): connectObj(board_address, board_port) {
        assert(ptrOrient);
        orientPtr = ptrOrient;
    }
    nodemcu_orient(): connectObj(DEFAULT__IPADDR, DEFAULT__PORT) {

    }
    void update(bool printResult=true);
    bool parseData(std::string _str);
};


#endif