#pragma once
#ifndef HPP_RECORD_FRAMES
#define HPP_RECORD_FRAMES

#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>


class opencv_recorder {

    /**
     * frame dimensions:
     * [0] - width
     * [1] - height
    */
    int frameDim[2] = {640, 480};
    int initFPS = 25;
    std::string filename = "defaultName.mp4";

    cv::Size frameSize;
    cv::VideoWriter videoObj;

    public:
    opencv_recorder();
    opencv_recorder(
        std::string filename,
        int frame_width     = 640,
        int frame_height    = 480,
        int startFPS        = 25
    );
    int setup();
    void addFrame(cv::Mat frame);
    void releaseVideo();
};



/** Source file section below:
 * NOTE: isn't in a different file because i can't be arsed with cmake*/

/// add frame to videoObject
void opencv_recorder::addFrame(
    cv::Mat frame
) {
    videoObj.write(frame);
}

/// finish the video storing and create the file
void opencv_recorder::releaseVideo() {
    videoObj.release();
}


/**
 * Setup all objects and stuff and e.t.c e.t.c
 * @return returns whether it was successful or not: `0` - successful; `-1` - failed
*/
int opencv_recorder::setup() {
    frameSize = cv::Size(frameDim[0], frameDim[1]);
    videoObj = cv::VideoWriter(
        filename,
        cv::VideoWriter::fourcc('a', 'v', 'c', '1'),
        initFPS,
        frameSize,
        true
    );
    if(!videoObj.isOpened()) {
        std::cout << "ERROR: cannot open/initialise VideoWriter object. Paused: enter to continue" << std::endl;
        std::cin.get();
        std::cin.clear();
        std::cin.ignore();
        return -1;
    }
    return 0;
}

/// Default constructor
opencv_recorder::opencv_recorder(
) {

}
/**
 * Initialises `cv::VideoWriter` with given parameters
 * @param fileName name of video file
 * @param frame_width width of frame/video in pixels
 * @param frame_height height of frame/video in pixels
 * @param startFPS fps to initialise `cv::VideoWriter` object
*/
opencv_recorder::opencv_recorder(
    std::string fileName,
    int frame_width,
    int frame_height,
    int startFPS
) {
    filename = fileName;
    frameDim[0] = frame_width;
    frameDim[1] = frame_height;
    initFPS = startFPS;

    setup();
}



#endif