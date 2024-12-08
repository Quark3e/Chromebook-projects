
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

    if(!udpObj.func_createSocket(AF_INET, SOCK_DGRAM)) exit(1);
    
    // if((udpObj.get_localSocket() = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    //     perror("socket creation failed: ");
    //     exit(1);
    // }

    // memset(&udpObj._remote_sockaddr_in, 0, sizeof(udpObj._remote_sockaddr_in));
    // memset(&udpObj._local_sockaddr_in, 0, sizeof(udpObj._local_sockaddr_in));
    
    if(!udpObj.func_bind()) exit(1);

    // udpObj._local_sockaddr_in.sin_family = AF_INET;
    // udpObj._local_sockaddr_in.sin_addr.s_addr = INADDR_ANY;
    // udpObj._local_sockaddr_in.sin_port = htons(port);
    // if(bind(udpObj.get_localSocket(), (const struct sockaddr*)&udpObj._local_sockaddr_in, sizeof(udpObj._local_sockaddr_in)) < 0) {
    //     perror("bind failed: ");
    //     exit(1);
    // }

    // while(true) {
        // if(__VERBOSE) std::cout << "Waiting for connection" << std::endl;
        // if((bytes = udpObj.func_recvfrom(
        //     udpObj.get_localSocket(),
        //     udpObj.recvBuffer,
        //     MAX_MESSAGE_SIZE,
        //     MSG_WAITALL,
        //     (struct sockaddr*)&udpObj._remote_sockaddr_in,
        //     &recvLen
        // )) < 0) exit(1);
        // else {
        //     if(__VERBOSE) std::cout << "recv request. Received: " << bytes << " bytes. Message: " << udpObj.recvBuffer << std::endl;
        // }
        // if(__VERBOSE) std::cout << "Connection accepted" << std::endl;

        display();
    //     if(__VERBOSE) std::cout << std::endl;
    // }
    return 0;
}

void *display() {
    cv::Mat img, imgGray;
    std::vector<int> encodeParam{cv::IMWRITE_JPEG_QUALITY, 80};
    uint16_t arrSize;
    img = cv::Mat::zeros(480, 640, CV_8UC1);
    // img = cv::Mat::zeros(200, 320, CV_8UC1);
    if(!img.isContinuous()) {
        img = img.clone();
        imgGray = img.clone();
    }

    int imgSize = img.total() * img.elemSize();
    int imgSize_compr = 0;
    int bytes = 0;
    socklen_t recvLen = 0;
    int key = 0;

    if(__VERBOSE) std::cout << "Image size: " << imgSize << std::endl;

    while(true) {
        // receive request msg
        if((bytes = udpObj.func_recvfrom(
            udpObj.get_localSocket(),
            udpObj.recvBuffer,
            MAX_MESSAGE_SIZE,
            0,
            (struct sockaddr*)&udpObj._remote_sockaddr_in,
            &recvLen
        )) < 0) {
            perror("recvfrom request msg failed: ");
            break;
        }
        else {
            udpObj.recvBuffer[bytes] = '\0';
            if(__VERBOSE) std::cout << "-received request. Received: " << bytes << " bytes, msg: " << udpObj.recvBuffer << std::endl; 
        }

        if(__VERBOSE) std::cout << "----second\n";
        if((bytes = udpObj.func_recvfrom(
            udpObj.get_localSocket(),
            udpObj.recvBuffer,
            MAX_MESSAGE_SIZE,
            0,
            (struct sockaddr*)&udpObj._remote_sockaddr_in,
            &recvLen
        )) < 0) {
            perror("recvfrom request msg failed: ");
            break;
        }
        else {
            udpObj.recvBuffer[bytes] = '\0';
            if(__VERBOSE) std::cout << "-received request. Received: " << bytes << " bytes, msg: " << udpObj.recvBuffer << std::endl; 
        }

        cap >> img;

        cv::cvtColor(img, imgGray, cv::COLOR_BGR2GRAY);

        std::vector<uchar> bitArr;
        cv::imencode(".jpg", imgGray, bitArr, encodeParam);
        arrSize = bitArr.size();
        
        // if((bytes = udpObj.func_sendto(
        //     udpObj.get_localSocket(),
        //     imgGray.data,
        //     imgSize,
        //     MSG_CONFIRM,
        //     (const struct sockaddr*)&udpObj._remote_sockaddr_in,
        //     sizeof(udpObj._remote_sockaddr_in)
        // )) < 0) {
        //     std::cerr << "sendto() errno: "<< errno << " sent bytes = " << bytes << std::endl;
        //     if(__VERBOSE) std::cout << "exiting display" << std::endl;
        //     break;
        // }

        recvLen = sizeof(udpObj._remote_sockaddr_in);

        if(__VERBOSE) std::cout << "-send arrSize: " << sizeof(arrSize) << " bytes\n";
        // std::string testMsg = "test message:";
        if((bytes = udpObj.func_sendto(
            udpObj.get_localSocket(),
            &arrSize,
            // testMsg.c_str(),
            sizeof(arrSize),
            // testMsg.length(),
            0,
            (const struct sockaddr*)&udpObj._remote_sockaddr_in,
            recvLen
        )) < 0) {
            perror("failed to send arrSize data. ");
            if(__VERBOSE) std::cout << "errno: " << errno << std::endl;
            break;
        }
        if(__VERBOSE) std::cout << "  sent: " << bytes << " bytes" << std::endl;
        if(__VERBOSE) std::cout << "  sent arrSize: " << arrSize << std::endl;


        if(__VERBOSE) std::cout << "-recv confirmSize\n";
        uint16_t confirmSize = 0;
        if((bytes = udpObj.func_recvfrom(
            udpObj.get_localSocket(),
            &confirmSize,
            sizeof(confirmSize),
            MSG_WAITALL,
            (struct sockaddr*)&udpObj._remote_sockaddr_in,
            &recvLen
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
        if(__VERBOSE) std::cout<< "  received: " << bytes << " bytes" << std::endl;

        if(__VERBOSE) std::cout << "-send bitArr:\n";
        if((bytes = udpObj.func_sendto(
            udpObj.get_localSocket(),
            static_cast<void*>(bitArr.data()),
            arrSize*sizeof(bitArr[0]),
            MSG_CONFIRM,
            (const struct sockaddr*)&udpObj._remote_sockaddr_in,
            sizeof(udpObj._remote_sockaddr_in)
        )) < 0) {
            std::cerr << "sendto() errno: "<< errno << " sent bytes = " << bytes << std::endl;
            if(__VERBOSE) std::cout << "Exiting display" << std::endl;
            break;
        }
        else {
            if(__VERBOSE) std::cout << "  sent: " << bytes << " bytes." << std::endl;
        }

        if(__VERBOSE) std::cout << std::endl;
    }

    return nullptr;
}