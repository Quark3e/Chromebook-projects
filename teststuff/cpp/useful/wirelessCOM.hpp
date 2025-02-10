
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

    operator std::string() const {
        std::string _temp = "";
        std::string _retur= "{";
        if(this->_callFunc) {
            this->_callFunc(this->x, _temp);
            _retur += _temp+", ";
            this->_callFunc(this->y, _temp);
            _retur += _temp+", ";
            this->_callFunc(this->z, _temp);
            _retur += _temp;
        }
        else {
            _retur += std::to_string(this->x)+", ";
            _retur += std::to_string(this->y)+", ";
            _retur += std::to_string(this->z);
        }
        _retur+="}";
        return _retur;
    }

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
    bool newData(vec3<_store_type> &_ref) {
        this->x = _ref.x;
        this->y = _ref.y;
        this->z = _ref.z;
        return true;
    }
    bool newData(_store_type _x, _store_type _y, _store_type _z) {
        this->x = _x;
        this->y = _y;
        this->z = _z;
        return true;
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

    sockaddr_in _server_sockaddr_in;
    socklen_t   _sockAddrLen    = sizeof(sockaddr_in);
    bool        _socket_closed  = true;

    public:
    std::string _ipAddr     = DEFAULT__IPADDR;
    int         _port       = DEFAULT__PORT;
    bool _verbose = true;

    /// @brief Initialiser function for internal UDP socket setup
    /// @return Whether it successfully initialied [`0`] or an error has occurred [`!=0`]
    int init();
    int ms_timeout = 5000;
    
    /// @brief Constructor call with remote board's IP and PORT.
    /// @param _board_IP IP of the esp8266 board's server.
    /// @param _board_PORT PORT of the esp8266 board's server.
    /// @note Will call `init()` function by default.
    nodemcu_connect(std::string _board_IP, int _board_PORT);
    /// @brief Constructor call of class with default IP and PORT definitions.
    /// @param _initialise whether to call `init()` on constructor call or not.
    nodemcu_connect(bool _initialise);
    /// @brief Constructor call with remote board's IP and PORT.
    /// @param _initialise whether to call `init()` on constructor call or not.
    /// @param _board_IP IP of the esp8266 board's server.
    /// @param _board_PORT PORT of the esp8266 board's server.
    nodemcu_connect(bool _initialise, std::string _board_IP, int _board_PORT);
    ~nodemcu_connect();

    /**
     * @brief Request new data from the esp8266 board
     * 
     * @param _printResult whether to print the received data
     * @return int of number of bytes received
     */
    int request(bool _printResult = true);


    int         bytesRecv = 0;          // Number of bytes received from the esp8266 board
    int         bytesSent = 0;          // number of bytes sent to the esp8266 board from latest sendto call.
    std::string requestMsg = "Hello";   // the string sent to the esp8266 board
    char        buffer[MAXLINE];        // char buffer of the received string

    /**
     * @brief Get whether the class has been initialised
     * @return bool for whether it has been initialised or not.
     */
    bool isInit();
};

/**
 * Dedicated class for telemetry data from esp8266 board connected to an accelerometer board
 */
class nodemcu_orient {
    std::string _info_name = "nodemcu_orient";
    public:
    
    // float orient[3] = {0,0,0}; //degrees

    vec3<float> accel{0, 0, 0}; // Container for accelerometer data for the across XYZ-axes. [unit: G]
    vec3<float> gyro{0, 0, 0};  // Container for gyroscopic data for the XYZ-axes

    // float x_accel = 0, y_accel = 0, z_accel = 0;
    float pitch, roll, Pitch=0, Roll=0;
    bool useFilter = false;
    float accelFilter = 0.1;

    float* orientPtr = nullptr;

    nodemcu_connect connectObj;

    nodemcu_orient(bool initialise=true);
    nodemcu_orient(float ptrOrient[3], bool initialise=true);
    nodemcu_orient(std::string board_address, int board_port, bool initialise=true);
    nodemcu_orient(float ptrOrient[3], std::string board_address, int board_port, bool initialise=true);

    void update(bool printResult=true);
    bool parseData(std::string _str);

};


#endif