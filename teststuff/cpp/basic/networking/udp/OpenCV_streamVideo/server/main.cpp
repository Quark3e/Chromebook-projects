
#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>

#include "../../../LIB_NETWORKCLASS/NETWORKCLASS.hpp"

NETWORKCLASS udpObj;

void *display();

int capDev = 0;
cv::VideoCapture cap(capDev);

bool __VERBOSE = false;


int main(int argc, char** argv) {

    int port = 1086;
    
    if(argc>1) {
        if(!strcmp(argv[1], "-h") || !strcmp(argv[1], "--help")) {
            if(__VERBOSE) std::cout <<"Usage: cv_video_srv -p/--port [port] -c/--cap [capture device]\n" <<
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
            if(!strcmp(argv[i], "-v") || !strcmp(argv[i], "--verbose")) {
                __VERBOSE = true;
            }
        }
    }

    udpObj = NETWORKCLASS("ANY", port);

    if(udpObj.func_createSocket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) {
        std::cerr << "create socket failed. " << std::endl;
        exit(1);
    }
    if(udpObj.func_bind()) {
        std::cerr << "bind() failed." << std::endl;
        exit(1);
    }


    display();
    return 0;
}

void *display() {
    cv::Mat img, imgGray;
    std::vector<int> encodeParam{cv::IMWRITE_JPEG_QUALITY, 80};
    uint16_t arrSize;
    img = cv::Mat::zeros(480, 640, CV_8UC1);
    if(!img.isContinuous()) {
        img = img.clone();
        imgGray = img.clone();
    }

    int imgSize = img.total() * img.elemSize();
    int imgSize_compr = 0;

    while(true) {
        if((udpObj._bytesRecv = udpObj.func_recvfrom(
            udpObj.get_localSocket(),
            udpObj.recvBuffer,
            MAX_MESSAGE_SIZE,
            0,
            (struct sockaddr*)&udpObj._remote_sockaddr_in,
            &udpObj._sockAddrLen
        )) < 0) {
            perror("recvfrom request msg failed: ");
            break;
        }
        else {
            udpObj.recvBuffer[udpObj._bytesRecv] = '\0';
            if(__VERBOSE) std::cout << "-received request. Received: " << udpObj._bytesRecv << " bytes, msg: " << udpObj.recvBuffer << std::endl; 
        }

        cap >> img;

        cv::cvtColor(img, imgGray, cv::COLOR_BGR2GRAY);

        std::vector<uchar> bitArr;
        cv::imencode(".jpg", imgGray, bitArr, encodeParam);
        arrSize = bitArr.size();


        if(__VERBOSE) std::cout << "-send arrSize: " << sizeof(arrSize) << " bytes\n";
        if((udpObj._bytesSent = udpObj.func_sendto(
            udpObj.get_localSocket(),
            &arrSize,
            sizeof(arrSize),
            0,
            (struct sockaddr*)&udpObj._remote_sockaddr_in,
            udpObj._sockAddrLen
        )) < 0) {
            perror("failed to send arrSize data. ");
            if(__VERBOSE) std::cout << "errno: " << errno << std::endl;
            break;
        }
        if(__VERBOSE) std::cout << "  sent: " << udpObj._bytesSent << " bytes" << std::endl;
        if(__VERBOSE) std::cout << "  sent arrSize: " << arrSize << std::endl;


        if(__VERBOSE) std::cout << "-recv confirmSize\n";
        uint16_t confirmSize = 0;
        if((udpObj._bytesRecv = udpObj.func_recvfrom(
            udpObj.get_localSocket(),
            &confirmSize,
            sizeof(confirmSize),
            0,
            (struct sockaddr*)&udpObj._remote_sockaddr_in,
            &udpObj._sockAddrLen
        )) < 0) {
             perror("failed to receive confirmed arrSize response: ");
             if(__VERBOSE) std::cout << "errno: " << errno << std::endl;
             break;
        }
        else if(confirmSize!=arrSize) {
            if(__VERBOSE) std::cout << "  confirmed arrSize response: " << confirmSize << " doesnt match sent size: " << arrSize << std::endl;
            if(__VERBOSE) std::cout << " errno: " << errno << std::endl;
            break;
        }
        if(__VERBOSE) std::cout<< "  received: " << udpObj._bytesRecv << " bytes" << std::endl;

        if(__VERBOSE) std::cout << "-send bitArr:\n";
        if((udpObj._bytesSent = udpObj.func_sendto(
            udpObj.get_localSocket(),
            static_cast<void*>(bitArr.data()),
            arrSize*sizeof(bitArr[0]),
            0,
            (struct sockaddr*)&udpObj._remote_sockaddr_in,
            udpObj._sockAddrLen
        )) < 0) {
            std::cerr << "sendto() errno: "<< errno << " sent bytes = " << udpObj._bytesSent << std::endl;
            if(__VERBOSE) std::cout << "Exiting display" << std::endl;
            break;
        }
        else {
            if(__VERBOSE) std::cout << "  sent: " << udpObj._bytesSent << " bytes." << std::endl;
        }

        if(__VERBOSE) std::cout << std::endl;
    }

    return nullptr;
}