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
    opencv_recorder() {}
    /**
     * Initialises `cv::VideoWriter` with given parameters
     * @param fileName name of video file
     * @param frame_width width of frame/video in pixels
     * @param frame_height height of frame/video in pixels
     * @param startFPS fps to initialise `cv::VideoWriter` object
    */
    opencv_recorder(
        std::string fileName,
        int frame_width     = 640,
        int frame_height    = 480,
        int startFPS        = 25
    ) {
        filename = fileName;
        frameDim[0] = frame_width;
        frameDim[1] = frame_height;
        initFPS = startFPS;

        setup();
    }

    int setup() {
        frameSize = cv::Size(frameDim[0], frameDim[1]);
        videoObj = cv::VideoWriter(
            filename,
            cv::VideoWriter::fourcc('a', 'v', 'c', '1'),
            initFPS,
            frameSize,
            true
        );
        if(!videoObj.isOpened()) {
            throw std::runtime_error("ERROR: cannot open/initialise VideoWriter object.");

        }
        return 0;
    }
    void addFrame(cv::Mat frame) {
        videoObj.write(frame);
    }
    void releaseVideo() {
        videoObj.release();
    }
};




#endif