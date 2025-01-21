
#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include <thread>
#include <chrono>

#include "../../../LIB_NETWORKCLASS/NETWORKCLASS.hpp"

NETWORKCLASS tcpObj;

void *display(/*void**/);

int capDev = 0;
cv::VideoCapture cap0(capDev);
cv::VideoCapture cap1(2);

struct int2 {
    int x;
    int y;
};


int2 imgDim = {640, 480};

bool __VERBOSE = false;

int main(int argc, char** argv) {

    int port = 1086;

    if(argc>1) {
        if(!strcmp(argv[1], "-h") || !strcmp(argv[1], "--help")) {
            std::cout <<"Usage: cv_video_srv -p/--port [port] -c/--cap [capture device]\n" <<
                        " port:         : socket port (1086 default)\n" <<
                        // " capture device: (0 default)" << 
                        std::endl;
            return 0;
        }
        for(int i=1; i<argc; i++) {
            if(i<argc) {
                if(!strcmp(argv[i], "-p") || !strcmp(argv[i], "--port")) {
                    port = std::stoi(argv[i+1]);
                }
                // if(!strcmp(argv[i], "-c") || !strcmp(argv[i], "--cap")) {
                //     cap = cv::VideoCapture(std::stoi(argv[i+1]));
                // }
            }
            if(!strcmp(argv[i], "-v") || !strcmp(argv[i], "--verbose")) __VERBOSE = true;
        }
    }


    // pthread_t thread_id;
    tcpObj = NETWORKCLASS("ANY", port);
    if(tcpObj.func_createSocket(AF_INET, SOCK_STREAM)) exit(1);

    int _yes = 1;
    if(setsockopt(tcpObj.get_localSocket(), SOL_SOCKET, SO_REUSEADDR, &_yes, sizeof(_yes))==-1) {
        perror("setsockopt");
        exit(1);
    }
    if(tcpObj.func_bind()) exit(1);
    

    char        cli_IP[INET_ADDRSTRLEN];
    uint16_t    cli_PORT;
    while(true) {
        if(__VERBOSE) std::cout << "while loop iteration" << std::endl;
        /*if((remoteSocket = accept(localSocket, (struct sockaddr*)&remoteAddr, (socklen_t*)&addrLen)) < 0) {
            std::cerr << "accept failed!" << std::endl;
            exit(1);
        }*/
        if((tcpObj.func_listen() || tcpObj.func_accept())) exit(1);
        // Connected to a client

        inet_ntop(AF_INET, &tcpObj._remote_sockaddr_in.sin_addr, cli_IP, sizeof(cli_IP));
        cli_PORT = htons(tcpObj._remote_sockaddr_in.sin_port);
        std::cout << "Connection accepted. Client info [IP:"<<cli_IP<<", PORT:"<<cli_PORT<<"]"<< std::endl;
        //pthread_create(&thread_id, NULL, display, &remoteSocket);
        display();
        if(__VERBOSE) std::cout << std::endl << "--------------------" << std::endl;
    }
    return 0;
}


void *display(/*void *ptr*/) {
    // int socket = *(int*)ptr;

    cv::Mat imgFused = cv::Mat::zeros(2*imgDim.y, imgDim.x, CV_8UC1); //vstacked fused image holder
    std::vector<cv::Mat> img(2, cv::Mat::zeros(imgDim.y, imgDim.x, CV_8UC1)), imgGray(2, cv::Mat::zeros(imgDim.y, imgDim.x, CV_8UC1));
    std::vector<int> bitArr_param{cv::IMWRITE_JPEG_QUALITY, 80};
    uint16_t arrSize;
    // img = cv::Mat::zeros(480, 640, CV_8UC1);
    for(size_t i=0; i<2; i++) {
        if(!img[i].isContinuous()) {
            img[i] = img[i].clone();
            imgGray = img[i].clone();
        }
    }

    int imgSize = imgFused.total() * imgFused.elemSize();
    int bytes = 0;
    int key;

    if(__VERBOSE) std::cout << "Image size: " << imgSize << std::endl;

    char inpMsg[11];
    while(true) {
        if((bytes = tcpObj.func_recv(1, inpMsg, 10, MSG_WAITALL))==-1) {
            perror("recv() for initial signal failed: ");
            exit(1);
        }
        inpMsg[bytes] = '\0';
        if(!strcmp(inpMsg, "disconnect")) {
            std::cout << "disconnect msg received. Exiting client connection."<<std::endl;
            break;
        }
        cap0 >> img[0];
        cap1 >> img[1];

        cv::cvtColor(img[0], imgGray[0], cv::COLOR_BGR2GRAY);
        cv::cvtColor(img[1], imgGray[1], cv::COLOR_BGR2GRAY);

        cv::vconcat(img[0], img[1], imgFused);


        std::vector<uchar> bitArr;
        cv::imencode(".jpg", img[0], bitArr, bitArr_param);
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
        if(__VERBOSE) std::cout << std::endl;
    }
    if(shutdown(tcpObj.get_localSocket(), SHUT_RDWR)) {
        perror("shutdown of connection failed: ");
        exit(1);
    }
    while(tcpObj.func_recv(1)>0); //read until socket recv returns -1;
    if(close(tcpObj.get_remoteSocket())) {
        perror("failed closing remote socket: ");
        exit(1);
    }
    
    return nullptr;
}

