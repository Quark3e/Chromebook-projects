
#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>
#include <fstream>
#include <cmath>

std::string ascii_table ="@B%8&WM#*oahkbdpqwmZO0QLCJUYXzcvunxrjft/|()1{}[]?-_+~<>i!lI;:,^`'.";

int main(int argc, char** argv) {
    std::cout << "-------------------"<<std::endl;
    if(argc<2) {
        std::cout << "No image parameter given. exiting.."<<std::endl;
        return 1;
    }

    std::string outFolder = "output/";
    std::string imgPath = argv[1];
    std::string fileName = "";
    size_t lastPos = 0;
    while(true) {
        size_t currPos = imgPath.find('/', lastPos+1);
        if(currPos==std::string::npos || currPos>=imgPath.length()) {
            break;
        }
        lastPos = currPos;
        // std::cout << imgPath.find('/')<<std::endl;
    }
    std::ofstream outFile;
    fileName = "ASCII_"+imgPath.substr(lastPos+1, imgPath.length()-lastPos)+".txt";
    std::cout << fileName << std::endl;
    outFile.open(outFolder+fileName);
    if(!outFile.is_open()) {
        std::cout << "error: could not open file with given path(s). exiting."<<std::endl;
        return 1;
    }
    cv::Mat img = cv::imread(argv[1]);
    std::cout << "image dimensions: "<<img.rows << ":"<<img.cols<<std::endl;
    // cv::Mat img = cv::imread("/home/pi/Chromebook-projects/teststuff/test_media/mask.png");
    if(img.empty()) {
        std::cout << "File not successfully opened. Exiting"<<std::endl;
        return 1;
    }
    //Before changing
    // cv::imshow("Before",img);
    
    cv::cvtColor(img, img, cv::COLOR_BGR2GRAY);
    cv::Mat newImg = img.clone();
    //change some pixel value
    for(int j=0;j<newImg.rows;j++)  {
        for (int i=0;i<newImg.cols;i++) {
            // std::cout << " -"<<j<<":"<<i;
            // std::string outStr = ;
            outFile<<ascii_table[static_cast<int>(round(((float)newImg.at<uchar>(j,i)/static_cast<float>(255))*ascii_table.length()))];
            // if(j==i) std::cout << (int)newImg.at<uchar>(j,i)<<std::endl; //white
        }
        // std::cout<<"\n";
        outFile<<"\n";
    }
    outFile.close();
    //After changing
    cv::namedWindow("Before - after");
    cv::moveWindow("Before - after", 0, 0);
    std::cout << "opening window.."<<std::endl;
    cv::hconcat(img, newImg, img);
    // while(true) {
        cv::imshow("Before - after", img);
        int cv_winKey = cv::waitKey(-1);
        if(cv_winKey==27) {
            return 0;
        }
    // }
    return 0;
}