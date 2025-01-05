
#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>

#include <NETWORKCLASS.hpp>
NETWORKCLASS udpObj;

bool __VERBOSE = false;

int main(int argc, char** argv) {

    std::string serverIP    = "192.168.1.177";
    int         serverPORT  = 1086;

    if(argc>1) {
        if(!strcmp(argv[1], "-h") || !strcmp(argv[1], "--help")) {
            if(__VERBOSE) std::cout <<"Usage: cv_video_cli -p/--port [port] -a/--address [address]\n" <<
                        " port      : socket port (8080 default)\n" <<
                        " address   : server IPaddress (\"192.168.1.177\" default (same device))\n" << std::endl;
            exit(0);
        }
        for(int i=1; i<argc; i++) {
            if(i<argc) {
                if(!strcmp(argv[i], "-p") || !strcmp(argv[i], "--port")) {
                    serverPORT = std::stoi(argv[i+1]);
                }
                if(!strcmp(argv[i], "-a") || !strcmp(argv[i], "--address")) {
                    serverIP = argv[i+1];
                }
            }
            if(!strcmp(argv[i], "-v") || !strcmp(argv[i], "--verbose")) {
                __VERBOSE = true;
            }
        }
    }
    

    udpObj = NETWORKCLASS(serverIP, serverPORT);
    if(!udpObj.func_createSocket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) exit(1);
    // if(!udpObj.func_bind()) exit(1);



    udpObj._remote_sockaddr_in.sin_family = AF_INET;
    udpObj._remote_sockaddr_in.sin_port = htons(serverPORT);
    udpObj._remote_sockaddr_in.sin_addr.s_addr = inet_addr(serverIP.c_str());

    int bytes = 0;
    int key = 0;

    cv::namedWindow("CV Video Client", 1);

    std::string msgSend = "Hello server.";
    
    int recvLen = 0;
    while(key!='q') {
        // send request msg
        if(__VERBOSE) std::cout << "-sendto() initial request:\n";
        int result = udpObj.func_sendto(
            udpObj.get_localSocket(),
            msgSend.c_str(),
            msgSend.size(),
            0,
            (sockaddr*)&udpObj._remote_sockaddr_in,
            sizeof(udpObj._remote_sockaddr_in)
        );
        if(result==-1) {
            perror("sendto() error: ");
            exit(1);
        }
        if(__VERBOSE) std::cout << "  sent: " << result << " bytes" << std::endl;

        std::vector<uchar> bitArr;
        uint16_t arrSize = 69;

        if(__VERBOSE) std::cout << "-recv arrSize:\n";

        sockaddr_in from;
        if((bytes = recvfrom(
            udpObj.get_localSocket(),
            reinterpret_cast<char*>(&arrSize),
            sizeof(arrSize),
            0,
            reinterpret_cast<SOCKADDR*>(&from),
            &udpObj._bytesRecv
            // udpObj.func_recvfrom(
            // udpObj.get_localSocket(),
            // &arrSize,
            // sizeof(arrSize),
            // 0,
            // (struct sockaddr*)&udpObj._remote_sockaddr_in,
            // &recvLen
        ))==-1) {
#if _WIN32
            std::cout << "recvfrom() for arrSize failed: " << WSAGetLastError() << std::endl;
#else
            perror("recvfrom() for arrSize failed: ");
#endif
            exit(1);
        }

        
        
        if(__VERBOSE) std::cout << "  received: " << bytes << " bytes" << std::endl;
        if(__VERBOSE) std::cout << "  Incoming arrSize: " << arrSize << " bytes" << std::endl;

        if(__VERBOSE) std::cout << "-send confirmation of arrSize:\n";
        if((bytes = udpObj.func_sendto(
            udpObj.get_localSocket(),
            &arrSize,
            sizeof(arrSize),
            0,
            (sockaddr*)&udpObj._remote_sockaddr_in,
            sizeof(udpObj._remote_sockaddr_in)
        )) < 0) {
            perror("sendto() for arrSize confirmation failed: ");
            exit(1);
        }
        if(__VERBOSE) std::cout << "  sent: " << bytes << " bytes" << std::endl;


        
        bitArr = std::vector<uchar>(arrSize);
        if(__VERBOSE) std::cout << "-recv bitArr:\n";
        if((bytes = udpObj.func_recvfrom(
            udpObj.get_localSocket(),
            bitArr.data(),
            arrSize*sizeof(bitArr[0]),
            0,
            (struct sockaddr*)&udpObj._remote_sockaddr_in,
            &recvLen
        ))==-1) {
            if(__VERBOSE) std::cout << "Error at recvfrom(): errno: " << errno << std::endl;
            exit(1);
        }
        if(__VERBOSE) std::cout << "  received: " << bytes << " bytes" << std::endl;
        

        cv::Mat imgInp = cv::imdecode(cv::Mat(1, arrSize, CV_8UC1, (void*)bitArr.data()), 0);

        cv::imshow("CV Video Client", imgInp);
        if(key = cv::waitKey(10)>=0) break;
    }

    return 0;
}
