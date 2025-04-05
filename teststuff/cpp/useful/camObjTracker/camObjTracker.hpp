#pragma once
#ifndef CAM_OBJ_TRACKER_HPP
#define CAM_OBJ_TRACKER_HPP


#include <iostream>

#include <fstream>

#include <vector>
#include <string>

#include <chrono>

#include <thread>
#include <atomic>
#include <mutex>

#include <pos2d.hpp>

#if _WIN32
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#else
#include <opencv4/opencv2/opencv.hpp>
#include <opencv4/opencv2/highgui/highgui.hpp>
#include <opencv4/opencv2/imgproc/imgproc.hpp>
#endif


namespace CVTRACK {

    struct camObjTrackerData {
        /// @brief final/total/avg contour position
        pos2d<float> cnt_pos{0, 0};
        /// @brief all contour positions
        std::vector<pos2d<float>> allCnt_pos;
        /// @brief total sum of contour areas
        float cnt_area = 0;
        /// @brief all contours
        std::vector<std::vector<cv::Point>> contours;
        /// @brief hierarchy of contours
        std::vector<cv::Vec4i> hierarchy;
        /// @brief original image/frame from camera
        cv::Mat imgRaw;
        /// @brief original image/frame from camera (not flipped)
        cv::Mat imgOriginal;
        /// @brief flipped image/frame from camera (flipped vertically)
        cv::Mat imgFlipped;
        /// @brief image/frame from camera converted to HSV color space
        cv::Mat imgHSV;
        /// @brief image/frame from camera converted to HSV color space and thresholded
        cv::Mat imgThreshold;
    };
    
    class camObjTracker {
        private:

        /**
         * @brief VideoCapture object used to read from camera.
         * 
         */
        cv::VideoCapture _cap;
        /**
         * @brief Mutex object used for thread safety when switching between read/write data pointers.
         * 
         */
        std::mutex _mtx_dataSwitch;
        /**
         * @brief Mutex object used for thread safety when accessing data from non-thread allocated members.
         * 
         */
        std::mutex _mtx_dataAccess;

        std::thread _thread_processCam;

        /// @brief option: Whether to use automatic brightness adjustment. Default is true.
        bool _options_autoBrightness = true;
        /// @brief option: Whether to allocate processing to multiple threads. Default is true.
        bool _options_useThreads = true;

        bool _init = false;
        
        ///--------------------------------------
        /// Processing data: Write data (data that is solved by the processing thread)
        
        camObjTrackerData _data_write;
        camObjTrackerData* ptr_data_write = &_data_write;
        
        /// Read/Share data: Read data (data that is shared to outside-scope/users-of-this-class)
        
        /// @brief Whether the processing thread is running. Default is false.
        std::atomic<bool> _isRunning{false};
        camObjTrackerData _data_read;
        camObjTrackerData* ptr_data_read = &_data_read;
        
        ///--------------------------------------
        pos2d<float> _frameSize{640, 480};
        int _areaLim = 1000;

        std::vector<int> _l_HSV{0, 0, 0};
        std::vector<int> _u_HSV{180, 255, 255};
        int _camIdx = -1;
        
        /**
         * A __schedule of morhological operations to be performed on the image/frame.
         * if the value is <0 then it's an erode operation, if >0 then it's a dilate operation.
         * 
         */
        std::vector<int> _morphological_schedule__size{0};
        
        void _process__1_readCam();
        void _process__2_resizeImg();
        void _process__3_cvt();
        void _process__4_colourFilter();
        void _process__5_morphologicalOps();
        void _process__6_findContours();

        /**
         * @brief Friend private function to process the camera data in a separate thread.
         * 
         * @param _this Pointer to the current instance of the camObjTracker class.
         * @note This function is called by the thread created in the constructor of camObjTracker.
         */
        friend void camObjTracker_processThreadFunc(camObjTracker* _this) {
            _this->_isRunning = true;
            std::unique_lock<std::mutex> u_lck_dataSwitch(_this->_mtx_dataSwitch, std::defer_lock);
            std::unique_lock<std::mutex> u_lck_dataAccess(_this->_mtx_dataAccess, std::defer_lock);
            while(_this->_isRunning) {
                u_lck_dataAccess.lock();
                _this->_process__1_readCam();
                _this->_process__2_resizeImg();
                _this->_process__3_cvt();
                _this->_process__4_colourFilter();
                _this->_process__5_morphologicalOps();
                _this->_process__6_findContours();
                u_lck_dataAccess.unlock();

                u_lck_dataSwitch.lock();
                auto temp = _this->ptr_data_write;
                _this->ptr_data_write = _this->ptr_data_read;
                _this->ptr_data_read = temp;
                u_lck_dataSwitch.unlock();
            }
        }
        public:
        
        camObjTracker() = default;
        /**
         * @brief Constructor for the camObjTracker class.
         * 
         * @param __callInit A boolean flag to determine whether to initialize the tracker upon creation.
         * @param __camIndex The index of the camera to be used for tracking.
         * @param __frameWidth The width of the video frame (default is 640).
         * @param __frameHeight The height of the video frame (default is 480).
         * @param __setAutoBrightness A boolean flag to enable or disable automatic brightness adjustment (default is true).
         * @param __useThreads A boolean flag to enable or disable multithreading for processing (default is true).
         * @param __morphological_schedule__size A vector specifying the size of the morphological operations schedule (default is {-1, 6}).
         * @param __u_HSV A vector specifying the upper HSV threshold values (default is {180, 255, 255}).
         * @param __l_HSV A vector specifying the lower HSV threshold values (default is {0, 0, 0}).
         * @param areaLim The minimum area limit for object detection (default is 1000).
         */
        camObjTracker(
            bool __callInit,
            int __camIndex,
            int __frameWidth = 640,
            int __frameHeight = 480,
            bool __setAutoBrightness = true,
            bool __useThreads = true,
            std::vector<int> __morphological_schedule__size = {-1, 6},
            std::vector<int> __u_HSV = {180, 255, 255},
            std::vector<int> __l_HSV = {0, 0, 0},
            int areaLim = 1000
        );
        camObjTracker(camObjTracker&& _other);
        camObjTracker& operator=(camObjTracker&& _other);

        camObjTracker(const camObjTracker& _other) = delete;
        camObjTracker& operator=(const camObjTracker& _other) = delete;
        /**
         * @brief Destructor for the camObjTracker class.
         * 
         * Cleans up resources and performs any necessary cleanup 
         * when an instance of camObjTracker is destroyed.
         */
        ~camObjTracker();

        bool init();
        /**
         * @brief Initializes the camera object tracker with the specified parameters.
         * 
         * @param __camIndex The index of the camera to be used.
         * @param __frameWidth The width of the video frame. Default is 640.
         * @param __frameHeight The height of the video frame. Default is 480.
         * @param __setAutoBrightness Flag to enable or disable automatic brightness adjustment. Default is true.
         * @param __useThreads Flag to enable or disable multithreading. Default is true.
         * @param __morphological_schedule__size A vector specifying the size of the morphological operations schedule. Default is {-1, 6}.
         * @param __u_HSV A vector specifying the upper HSV threshold values. Default is {180, 255, 255}.
         * @param __l_HSV A vector specifying the lower HSV threshold values. Default is {0, 0, 0}.
         * @param areaLim The minimum area limit for object detection. Default is 1000.
         * @return true if initialization is successful, false otherwise.
         */
        bool init(
            int __camIndex,
            int __frameWidth = 640,
            int __frameHeight = 480,
            bool __setAutoBrightness = true,
            bool __useThreads = true,
            std::vector<int> __morphological_schedule__size = {-1, 6},
            std::vector<int> __u_HSV = {180, 255, 255},
            std::vector<int> __l_HSV = {0, 0, 0},
            int areaLim = 1000
        );

        void setAutoBrightness(bool __setAutoBrightness);
        void setFrameSize(int width, int height);
        void setAreaLim(int areaLim);
        void setMorphologicalSchedule(std::vector<int> __schedule);
        void setHSV(std::vector<int> __u_HSV, std::vector<int> __l_HSV);

        bool isInit();
        bool isRunning();
        camObjTrackerData data();

        camObjTrackerData updateData();
    };
};


#endif // CAM_OBJ_TRACKER_HPP