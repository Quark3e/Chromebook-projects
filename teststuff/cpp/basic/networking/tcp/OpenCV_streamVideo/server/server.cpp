
#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>

#include "../tcp_lib/NETWORKCLASS_TCP.hpp"

NETWORKCLASS_TCP tcpObj;

void *display(/*void**/);

int capDev = 0;
cv::VideoCapture cap(capDev);

int main(int argc, char** argv) {

#if _WIN32
    
#endif

    if(!tcpObj.func_init()) {
        exit(1);
    }

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

    /*
    // Socket creation
    int localSocket;
    if((localSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cerr << "socket() failed!" << std::endl;
        exit(1);
    }

    struct sockaddr_in localAddr;
    localAddr.sin_family = AF_INET;
    localAddr.sin_addr.s_addr = INADDR_ANY;
    localAddr.sin_port = htons(port);

    if(bind(localSocket, (struct sockaddr*)&localAddr, sizeof(localAddr)) < 0) {
        std::cerr << "bind() failed!" << std::endl;
        exit(1);
    }

    listen(localSocket, 3);

    std::cout <<"Waiting for connections...\n" << "server port: " << port << std::endl;

    int remoteSocket;
    struct sockaddr_in remoteAddr;
    int addrLen = sizeof(struct sockaddr_in);
    */
    // pthread_t thread_id;
    tcpObj = NETWORKCLASS_TCP("ANY", port);
    if(!(
        tcpObj.func_sockCreate() &&
        tcpObj.func_bind()
    )) {
        exit(1);
    }
    while(true) {
        std::cout << "while loop iteration" << std::endl;
        /*if((remoteSocket = accept(localSocket, (struct sockaddr*)&remoteAddr, (socklen_t*)&addrLen)) < 0) {
            std::cerr << "accept failed!" << std::endl;
            exit(1);
        }*/
        if(!(
            tcpObj.func_listen() &&
            tcpObj.func_accept()
        )) exit(1);
        std::cout << "Connection accepted" << std::endl;
        //pthread_create(&thread_id, NULL, display, &remoteSocket);
        display();
        std::cout << std::endl;
    }
    return 0;
}


void *display(/*void *ptr*/) {
    // int socket = *(int*)ptr;

    cv::Mat img, imgGray;
    img = cv::Mat::zeros(480, 640, CV_8UC1);
    if(!img.isContinuous()) {
        img = img.clone();
        imgGray = img.clone();
    }

    int imgSize = img.total() * img.elemSize();
    int bytes = 0;
    int key;

    std::cout << "Image size: " << imgSize << std::endl;

    while(true) {
        cap >> img;

        cv::cvtColor(img, imgGray, cv::COLOR_BGR2GRAY);
        
        if((bytes=tcpObj.func_send(imgGray.data, imgSize, 0)) < 0) {
            std::cerr << "bytes = " << bytes << std::endl;
            std::cout << "exiting display" << std::endl;
            break;
        }
        // if((bytes=send(socket, reinterpret_cast<const char*>(imgGray.data), imgSize, 0)) < 0) {
        //     std::cerr << "bytes = " << bytes << std::endl;
        //     break;
        // }
    }
    
    return nullptr;
}

