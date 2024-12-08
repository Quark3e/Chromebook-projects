
#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>

#include "../../../LIB_NETWORKCLASS/NETWORKCLASS.hpp"

NETWORKCLASS udpObj;

void *display();

int capDev = 0;
cv::VideoCapture cap(capDev);

int main(int argc, char** argv) {

    int port = 1086;
    
    if(argc>1) {
        if(!strcmp(argv[1], "-h") || !strcmp(argv[1], "--help")) {
            std::cout <<"Usage: cv_video_srv -p/--port [port] -c/--cap [capture device]\n" <<
                        " port:         : socket port (1086 default)\n" <<
                        " capture device: (0 default)" << std::endl;
            return 0;
        }
        for(int i=1; i<argc; i++) {
            if(i<argc) {
                if(!strcmp(argv[i], "-p") || !strcmp(argv[i], "--port")) {
                    port = std::stoi(argv[i+1]);
                }
                if(!strcmp(argv[i], "-c") || !strcmp(argv[i], "--cap")) {
                    cap = cv::VideoCapture(std::stoi(argv[i+1]));
                }
            }
        }
    }

    udpObj = NETWORKCLASS("ANY", port);

    if(!udpObj.func_createSocket(AF_INET, SOCK_DGRAM)) exit(1);
    
    memset(&udpObj._remote_sockaddr_in, 0, sizeof(udpObj._remote_sockaddr_in));
    
    if(!udpObj.func_bind()) exit(1);

    int bytes = 0;
    int recvLen = 0;
    while(true) {
        std::cout << "Waiting for connection" << std::endl;
        if((bytes = udpObj.func_recvfrom(
            udpObj.get_localSocket(),
            udpObj.recvBuffer,
            MAX_MESSAGE_SIZE,
            MSG_WAITALL,
            (struct sockaddr*)&udpObj._remote_sockaddr_in,
            &recvLen
        )) < 0) exit(1);
        std::cout << "Connection accepted" << std::endl;

        display();
        std::cout << std::endl;
    }
    return 0;
}

void *display() {
    cv::Mat img, imgGray;
    std::vector<uchar> buff;
    std::vector<int> param{cv::IMWRITE_JPEG_QUALITY, 80};
    img = cv::Mat::zeros(480, 640, CV_8UC1);
    // img = cv::Mat::zeros(200, 320, CV_8UC1);
    if(!img.isContinuous()) {
        img = img.clone();
        imgGray = img.clone();
    }

    int imgSize = img.total() * img.elemSize();
    int imgSize_compr = 0;
    int bytes = 0;
    int recvLen = 0;
    int key = 0;

    std::cout << "Image size: " << imgSize << std::endl;

    while(true) {
        if((bytes = udpObj.func_recvfrom(
            udpObj.get_localSocket(),
            udpObj.recvBuffer,
            MAX_MESSAGE_SIZE,
            MSG_WAITALL,
            (struct sockaddr*)&udpObj._remote_sockaddr_in,
            &recvLen
        )) < 0) break;

        cap >> img;

        cv::cvtColor(img, imgGray, cv::COLOR_BGR2GRAY);
        cv::imencode(".jpg", imgGray, buff, param);
        imgSize_compr = buff.size();
        std::cout << "compressed size: " << imgSize_compr << " bytes" << std::endl;
        
        // if((bytes = udpObj.func_sendto(
        //     udpObj.get_localSocket(),
        //     imgGray.data,
        //     imgSize,
        //     MSG_CONFIRM,
        //     (const struct sockaddr*)&udpObj._remote_sockaddr_in,
        //     sizeof(udpObj._remote_sockaddr_in)
        // )) < 0) {
        //     std::cerr << "sendto() errno: "<< errno << " sent bytes = " << bytes << std::endl;
        //     std::cout << "exiting display" << std::endl;
        //     break;
        // }

        if((bytes = udpObj.func_sendto(
            udpObj.get_localSocket(),
            static_cast<void*>(buff.data()),
            buff.size(),
            MSG_CONFIRM,
            (const struct sockaddr*)&udpObj._remote_sockaddr_in,
            sizeof(udpObj._remote_sockaddr_in)
        )) < 0) {
            std::cerr << "sendto() errno: "<< errno << " sent bytes = " << bytes << std::endl;
            std::cout << "Exiting display" << std::endl;
            break;
        }
        else {
            std::cout << "Sent: " << bytes << " bytes." << std::endl;
        }
    }

    return nullptr;
}