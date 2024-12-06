
#include <opencv2/opencv.hpp>
#include <unistd.h>
#include <cstring>
#include <iostream>

#include "../lib/NETWORKCLASS_TCP.hpp"

NETWORKCLASS_TCP tcpObj;

int main(int argc, char** argv) {

    int     sock;
    char*   serverIP   = "internal";
    int     serverPort = 1086;


    if(argc>1) {
        if(!strcmp(argv[1], "-h") || !strcmp(argv[1], "--help")) {
            std::cout <<"Usage: cv_video_cli -p/--port [port] -a/--address [address]\n" <<
                        " port      : socket port (8080 default)\n" <<
                        " address   : server IPaddress (\"internal\" default (same device))\n" << std::endl;
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
    
    /*
    if((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cerr << "socket() failed!" << std::endl;
        exit(1);
    }

    struct sockaddr_in serverAddr;
    socklen_t addrLen = sizeof(struct sockaddr_in);

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = (!strcmp(serverIP, "internal")? INADDR_ANY : inet_addr(serverIP));
    serverAddr.sin_port = htons(serverPort);


    if(connect(sock, (sockaddr*)&serverAddr, addrLen)<0) {
        std::cerr << "connect() failed!" << std::endl;
        exit(1);
    }
    */
    if(!(
        tcpObj.func_sockCreate() &&
        tcpObj.func_connect()
    )) exit(1);
    
    cv::Mat img;
    img = cv::Mat::zeros(480, 640, CV_8UC1);
    int imgSize = img.total() * img.elemSize();
    uchar *iptr = img.data;
    int bytes = 0;
    int key;

    if(!img.isContinuous()) {
        img = img.clone();
    }

    std::cout << "Image Size: " << imgSize << std::endl;

    cv::namedWindow("CV Video Client", 1);

    while(key!='q') {
        if((bytes = tcpObj.func_receive(iptr, imgSize, MSG_WAITALL))==-1) {
            std::cerr << "recv failed." << std::endl;
            exit(1);
        }
        // if((bytes = recv(sock, iptr, imgSize, MSG_WAITALL))==-1) {
        //     std::cerr << "recv failed, received = " << bytes << std::endl;
        //     exit(1);
        // }
        cv::imshow("CV Video Client", img);
        if(key = cv::waitKey(10)>=0) break;
    }
    close(sock);

    return 0;
}
