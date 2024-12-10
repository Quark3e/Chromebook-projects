

// #define __TAKE_PERF

#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include <thread>
#include <chrono>

#include <jpeglib.h>

#include "../../../LIB_NETWORKCLASS/NETWORKCLASS.hpp"
#include "teststuff/cpp/useful/diy_performance.hpp"
#include "teststuff/cpp/useful/useful.hpp"
#include "teststuff/cpp/useful/createTable.hpp"

NETWORKCLASS tcpObj;

bool __VERBOSE = false;

int main(int argc, char** argv) {


    std::string serverIP   = "192.168.1.177";
    int         serverPORT = 1086;
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
                    serverPORT = std::stoi(argv[i+1]);
                }
                if(!strcmp(argv[i], "-a") || !strcmp(argv[i], "--address")) {
                    serverIP = argv[i+1];
                }
            }
            if(!strcmp(argv[i], "-v") || !strcmp(argv[i], "--verbose")) __VERBOSE = true;
        }
    }
    
    // if((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    //     std::cerr << "socket() failed!" << std::endl;
    //     exit(1);
    // }
    // sockaddr_in serverAddr;
    // serverAddr.sin_family = PF_INET;
    // serverAddr.sin_addr.s_addr = (!strcmp(serverIP.c_str(), "internal")? INADDR_ANY : inet_addr(serverIP.c_str()));
    // serverAddr.sin_port = htons(serverPORT);
    // if(connect(sock, (sockaddr*)&serverAddr, sizeof(serverAddr))<0) {
    //     std::cerr << "connect() failed!" << std::endl;
    //     exit(1);
    // }

    std::cout << " PROGRAM START:" << std::endl;
#if __TAKE_PERF
    PERF::perf_isolated perf_client;
    createTable perf_table(2, 7);
#endif

    if(__VERBOSE) {std::cout << " init NETWORKCLASS" << std::endl;}
    tcpObj = NETWORKCLASS(serverIP, serverPORT);
    if(__VERBOSE) {std::cout << " createSocket() test" << std::endl;}
    if(!tcpObj.func_createSocket(AF_INET, SOCK_STREAM)) exit(1);
    if(__VERBOSE) {std::cout << " connect()" << std::endl;}
    if(!tcpObj.func_connect()) exit(1);
    

    // cv::Mat img;
    // img = cv::Mat::zeros(480, 640, CV_8UC1);
    // int imgSize = img.total() * img.elemSize();
    // uchar *iptr = img.data;
    int bytes = 0;
    int key = 0;

    // if(!img.isContinuous()) {
    //     img = img.clone();
    // }
    // if(__VERBOSE) std::cout << "Image Size: " << imgSize << std::endl;


    // jpeglib setup
    int _rc, _i, _j;
    
    unsigned long jpg_size;
    // unsigned char *jpg_buffer;

    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;

    unsigned long bmp_size;
    unsigned char *bmp_buffer ;// = (unsigned char*)malloc(16000);
    int row_stride, width, height, pixel_size;



    cv::namedWindow("CV Video Client", 1);


    while(true) {
        if(__VERBOSE) std::cout << std::endl;
        
        std::vector<uchar> bitArr;
        uint16_t arrSize = 69;
        
#if __TAKE_PERF
        perf_client.set_T0("recv() arrSize");
#endif
        if(__VERBOSE) {std::cout << "-recv arrSize:"<<std::endl;}
        if((bytes = tcpObj.func_recv(0, &arrSize, 2, MSG_WAITALL))==-1) {
            std::cerr << "recv() for arrSize failed." << std::endl;
            if(__VERBOSE) {std::cout << "errno: " << errno << std::endl;}
            exit(1);
        }
        else if(bytes==0) {
            if(__VERBOSE) {std::cout << " peer closed the socket. Closing socket." <<std::endl;}
            exit(1);
        }
        else {
            if(__VERBOSE) {
                std::cout << "  received: "<<tcpObj._bytesRecv << " bytes" << std::endl;
                std::cout << "  Incoming arrSize: " << arrSize << " bytes" << std::endl;
            }
        }
#if __TAKE_PERF
        perf_client.set_T1("recv() arrSize");
        perf_table.insertText(perf_client.names()[0], 0, 0);
        perf_table.insertNum(perf_client.delays()[0], 1, 0);
        perf_client.set_T0("send() confirm");
#endif
        if(__VERBOSE) {std::cout << "-send confirmSize:" << std::endl;}
        // if(send(tcpObj.get_localSocket(), &arrSize, sizeof(arrSize), 0)==-1) {
        if(tcpObj.func_send(0, &arrSize, sizeof(arrSize), 0)==-1) {
            std::cerr << "send() for arrSize failed." << std::endl;
            if(__VERBOSE) std::cout << "errno: " << errno << std::endl;
            exit(1);
        }
        if(__VERBOSE) {std::cout << "  sent: " << tcpObj._bytesSent << " bytes" << std::endl;}
#if __TAKE_PERF
        perf_client.set_T1("send() confirm");
        perf_table.insertText(perf_client.names()[1], 0, 1);
        perf_table.insertNum(perf_client.delays()[1], 1, 1);
        perf_client.set_T0("recv() bitArr");
#endif
        if(__VERBOSE) {std::cout << "-recv bitArr:" << std::endl;}
        bitArr = std::vector<uchar>(arrSize);
        if((bytes = tcpObj.func_recv(0, bitArr.data(), arrSize*sizeof(bitArr[0]), MSG_WAITALL))==-1) {
            std::cerr << "recv() for bitArr data failed." << std::endl;
            if(__VERBOSE){ std::cout << "errno: " << errno << std::endl;}
            exit(1);
        }
        else {

        }
        if(__VERBOSE) {std::cout << "  received: "<<tcpObj._bytesRecv << " bytes" << std::endl;}
#if __TAKE_PERF
        perf_client.set_T1("recv() bitArr");
        perf_table.insertText(perf_client.names()[2], 0, 2);
        perf_table.insertNum(perf_client.delays()[2], 1, 2);
        perf_client.set_T0("dcmpr: C0");
#endif
        jpg_size = arrSize;
        // jpg_buffer = (unsigned char*)malloc(jpg_size+100);
        // for(int i=0; i<jpg_size; i++) {
        // }
        cinfo.err = jpeg_std_error(&jerr);
        jpeg_create_decompress(&cinfo);

        jpeg_mem_src(&cinfo, bitArr.data(), jpg_size);
        _rc = jpeg_read_header(&cinfo, TRUE);
        if(_rc!=1) {
            std::cerr << "jpeg_read_header() error"<<std::endl;
            exit(1);
        }
        cinfo.out_color_components = JCS_GRAYSCALE;
#if __TAKE_PERF
        perf_client.set_T1("dcmpr: C0");
        perf_table.insertText(perf_client.names()[3], 0, 3);
        perf_table.insertNum(perf_client.delays()[3], 1, 3);
        perf_client.set_T0("dcmpr: C1");
#endif
        jpeg_start_decompress(&cinfo);
        width       = cinfo.output_width;
        height      = cinfo.output_height;
        pixel_size  = cinfo.output_components;

        bmp_size    = width * height * pixel_size;
        bmp_buffer  = (unsigned char*)malloc(bmp_size);

        row_stride  = width * pixel_size;

        while(cinfo.output_scanline < cinfo.output_height) {
            unsigned char *buffer_array[1];
            buffer_array[0] = bmp_buffer + (cinfo.output_scanline)*row_stride;
            jpeg_read_scanlines(&cinfo, buffer_array, 1);
        }

        jpeg_finish_decompress(&cinfo);
        jpeg_destroy_decompress(&cinfo);

#if __TAKE_PERF
        perf_client.set_T1("dcmpr: C1");
        perf_table.insertText(perf_client.names()[4], 0, 4);
        perf_table.insertNum(perf_client.delays()[4], 1, 4);
        perf_client.set_T0("create cv::Mat");
#endif
        // cv::Mat imgInp = cv::imdecode(cv::Mat(1, arrSize, CV_8UC1, (void*)bitArr.data()), 0);
        // if((bytes = tcpObj.func_recv(iptr, imgSize, MSG_WAITALL))==-1) {
        //     std::cerr << "recv failed: " << bytes << " bytes" << std::endl;
        //     exit(1);
        // }
        cv::Mat imgInp(480, 640, CV_8UC1, (void*)bmp_buffer);

#if __TAKE_PERF
        perf_client.set_T1("create cv::Mat");
        perf_table.insertText(perf_client.names()[5], 0, 5);
        perf_table.insertNum(perf_client.delays()[5], 1, 5);
        perf_client.set_T0("display");
#endif

        cv::imshow("CV Video Client", imgInp);
        if(key = cv::waitKey(10)>=0) break;
        free(bmp_buffer);

#if __TAKE_PERF
        perf_client.set_T1("display");
        perf_table.insertText(perf_client.names()[6], 0, 6);
        perf_table.insertNum(perf_client.delays()[6], 1, 6);

        std::cout << perf_table.strExport() << std::endl;
        // ansiPrint(perf_table.strExport(), (float)0.0, (float)0.8, true, true);
#endif
    }
    // free(bmp_buffer);
    // close(sock);

    return 0;
}



