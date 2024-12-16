
#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include <thread>
#include <chrono>

#include "../../../LIB_NETWORKCLASS/NETWORKCLASS.hpp"

NETWORKCLASS tcpObj;

void *display(/*void**/);

int capDev = 0;
cv::VideoCapture cap(capDev);


bool __VERBOSE = false;

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
            if(!strcmp(argv[i], "-v") || !strcmp(argv[i], "--verbose")) __VERBOSE = true;
        }
    }


    // pthread_t thread_id;
    tcpObj = NETWORKCLASS("ANY", port);
    if(!tcpObj.func_createSocket(AF_INET, SOCK_STREAM)) exit(1);
    int _yes = 1;
    // if(setsockopt(tcpObj.get_localSocket(), SOL_SOCKET, SO_REUSEADDR, &_yes, sizeof(_yes))==-1) {
    //     perror("setsockopt");
    //     exit(1);
    // }
    if(!tcpObj.func_bind()) exit(1);
    
    while(true) {
        if(__VERBOSE) std::cout << "while loop iteration" << std::endl;
        /*if((remoteSocket = accept(localSocket, (struct sockaddr*)&remoteAddr, (socklen_t*)&addrLen)) < 0) {
            std::cerr << "accept failed!" << std::endl;
            exit(1);
        }*/
        if(!(
            tcpObj.func_listen() &&
            tcpObj.func_accept()
        )) exit(1);
        if(__VERBOSE) std::cout << "Connection accepted" << std::endl;
        //pthread_create(&thread_id, NULL, display, &remoteSocket);
        display();
        if(__VERBOSE) std::cout << std::endl << "--------------------" << std::endl;
    }
    return 0;
}


void *display(/*void *ptr*/) {
    // int socket = *(int*)ptr;

    cv::Mat img, imgGray;
    std::vector<int> bitArr_param{cv::IMWRITE_JPEG_QUALITY, 80};
    uint16_t arrSize;
    img = cv::Mat::zeros(480, 640, CV_8UC1);
    if(!img.isContinuous()) {
        img = img.clone();
        imgGray = img.clone();
    }

    int imgSize = img.total() * img.elemSize();
    int bytes = 0;
    int key;

    if(__VERBOSE) std::cout << "Image size: " << imgSize << std::endl;

    char inpMsg[10];
    while(true) {
        if((bytes = tcpObj.func_recv(1, inpMsg, 10, MSG_WAITALL))==-1) {
            perror("recv() for initial signal failed: ");
            exit(1);
        }
        std::cout << "initial received: \""<<inpMsg<<"\"" << std::endl;
        if(!strcmp(inpMsg, "disconnect")) {
            std::cout << "disconnect msg received. Exiting client connection."<<std::endl;
            break;
        }
        cap >> img;

        cv::cvtColor(img, imgGray, cv::COLOR_BGR2GRAY);
    
        std::vector<uchar>  bitArr;
        cv::imencode(".jpg", imgGray, bitArr, bitArr_param);
        arrSize = bitArr.size();


        // bitArr.insert( )
        if(__VERBOSE) std::cout << "-send arrSize:\n";
        if((bytes = tcpObj.func_send(1, &arrSize, sizeof(arrSize), 0)) < 0) {
            //send compressed img size
            std::cout << "failed to send arrSize data." << std::endl;
            std::cout << "errno: " << errno << std::endl;
            break;
        }
        if(__VERBOSE) {
            std::cout << "  sent: " << tcpObj._bytesSent << " bytes" << std::endl;
            std::cout << "  sent arrSize: " << arrSize << std::endl;
        }

        // std::cout << "-recv confirmSize\n";
        // uint16_t confirmSize = 0;
        // if(tcpObj.func_recv(1, &confirmSize, sizeof(confirmSize), MSG_WAITALL)==-1) {
        //     std::cout << "failed to receive confirmed arrSize response." << std::endl;
        //     std::cout << "errno: " << errno << std::endl;
        //     break;
        // }
        // else if(confirmSize!=arrSize) {
        //     std::cout << "  confirmed arrSize response: "<<confirmSize << " doesnt match sent size: " << arrSize << std::endl;
        //     std::cout << "errno: " << errno << std::endl;
        //     break;
        // }
        // std::cout << "  received: " << tcpObj._bytesRecv << " bytes" << std::endl;

        if(__VERBOSE) std::cout << "-send bitArr\n";
        if((bytes=tcpObj.func_send(1, bitArr.data(), arrSize*sizeof(bitArr[0]), 0)) < 0) {
            std::cout << "failed to send data." << std::endl;
            std::cout << "errno: " << errno << std::endl;
            break;
        }
        if(__VERBOSE) std::cout << "  sent: " << tcpObj._bytesSent << " bytes" << std::endl;
        
        // imgGray = cv::imdecode(cv::Mat(1, arrSize, CV_8UC1, (void*)bitArr.data()), 0);
        if(__VERBOSE) std::cout << "  compressed img size: " << arrSize <<" + " << sizeof(arrSize) << " bytes" <<  std::endl;

        // if((bytes=tcpObj.func_send(imgGray.data, imgSize, 0)) < 0) {
        //     std::cerr << "bytes = " << bytes << std::endl;
        //     std::cout << "exiting display" << std::endl;
        //     break;
        // }
        // // if((bytes=send(socket, reinterpret_cast<const char*>(imgGray.data), imgSize, 0)) < 0) {
        // //     std::cerr << "bytes = " << bytes << std::endl;
        // //     break;
        // // }

        if(__VERBOSE) std::cout << std::endl;
    }
    
    return nullptr;
}

