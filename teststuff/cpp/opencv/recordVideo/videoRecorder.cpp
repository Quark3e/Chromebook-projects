
#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include <cstring>


int main(int argc, char** argv) {

    cv::VideoCapture cap(0);
    if(!cap.isOpened()) {
        std::cout << "Cannot open the video camera with index[" << 0 << "]" << std::endl;
        std::cin.get();
        return -1;
    }

    int frame_width = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_WIDTH));
    int frame_height = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_HEIGHT));

    cv::Size frame_size(frame_width, frame_height);
    int frames_per_second = 25;

    std::string filename = "test.mp4";
    cv::VideoWriter oVideoWriter(
        filename,
        // cv::VideoWriter::fourcc('M', 'J', 'P', 'G'),
        cv::VideoWriter::fourcc('a','v','c','1'),
        frames_per_second,
        frame_size,
        true
    );

    if(!oVideoWriter.isOpened()) {
        std::cout << "Cannot save the video to a file" << std::endl;
        std::cin.get();
        return -1;
    }
    std::string window_name = "My Camera Feed";
    cv::namedWindow(window_name);

    while(true) {
        cv::Mat frame;
        bool isSuccess = cap.read(frame);

        if(!isSuccess) {
            std::cout << "Video camera is disconnected (can't read from it)" << std::endl;
            std::cin.get();
            break;
        }

        oVideoWriter.write(frame);

        imshow(window_name, frame);

        int cv_winKey = cv::waitKey(10);
        if(cv_winKey == 27) {
            std::cout << "Esc key is pressed by the user. Stopping the video" << std::endl;
            break;
        }
    }

    oVideoWriter.release();
    return 0;
}