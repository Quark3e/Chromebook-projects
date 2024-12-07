
#include <opencv2/opencv.hpp>
// #include <unistd.h
#include <cstring>
#include <string>
#include <iostream>

#include "../../../LIB_NETWORKCLASS/NETWORKCLASS.hpp"

NETWORKCLASS tcpObj;

int main(int argc, char** argv) {

    std::string serverIP   = "192.168.1.177";
    int         serverPort = 1086;
    // int sock;

    if(argc>1) {
        if(!strcmp(argv[1], "-h") || !strcmp(argv[1], "--help")) {
            std::cout <<"Usage: cv_video_cli -p/--port [port] -a/--address [address]\n" <<
                        " port      : socket port (8080 default)\n" <<
                        " address   : server IPaddress (\"192.168.1.177\" default (same device))\n" << std::endl;
            exit(0);
        }
        for(int i=1; i<argc; i++) {
            if(i<argc) {
                if(!strcmp(argv[i], "-p") || !strcmp(argv[i], "--port")) {
                    serverPort = std::stoi(argv[i+1]);
                }
                if(!strcmp(argv[i], "-a") || !strcmp(argv[i], "--address")) {
                    serverIP = argv[i+1];
                }
            }
        }
    }
    
    // if((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    //     std::cerr << "socket() failed!" << std::endl;
    //     exit(1);
    // }
    // sockaddr_in serverAddr;
    // serverAddr.sin_family = PF_INET;
    // serverAddr.sin_addr.s_addr = (!strcmp(serverIP.c_str(), "internal")? INADDR_ANY : inet_addr(serverIP.c_str()));
    // serverAddr.sin_port = htons(serverPort);
    // if(connect(sock, (sockaddr*)&serverAddr, sizeof(serverAddr))<0) {
    //     std::cerr << "connect() failed!" << std::endl;
    //     exit(1);
    // }

    
    tcpObj = NETWORKCLASS(serverIP, serverPort);
    if(!(
        tcpObj.func_createSocket(AF_INET, SOCK_STREAM) &&
        tcpObj.func_connect()
    )) exit(1);
    
    cv::Mat img;
    img = cv::Mat::zeros(480, 640, CV_8UC1);
    int imgSize = img.total() * img.elemSize();
    uchar *iptr = img.data;
    int bytes = 0;
    int key = 0;

    if(!img.isContinuous()) {
        img = img.clone();
    }

    std::cout << "Image Size: " << imgSize << std::endl;

    cv::namedWindow("CV Video Client", 1);

    while(key!='q') {
        if((bytes = tcpObj.func_receive(iptr, imgSize, MSG_WAITALL))==-1) {
            std::cerr << "recv failed: " << bytes << " bytes" << std::endl;
            exit(1);
        }
        cv::imshow("CV Video Client", img);
        if(key = cv::waitKey(10)>=0) break;
    }
    // close(sock);

    return 0;
}



