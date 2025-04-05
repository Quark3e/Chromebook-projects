#include "camObjTracker.hpp"

namespace CVTRACK {

    camObjTracker::camObjTracker(
        bool callInit,
        int camIndex,
        int frameWidth,
        int frameHeight,
        bool setAutoBrightness,
        bool useThreads,
        std::vector<int> morphological_schedule__size,
        std::vector<int> u_HSV,
        std::vector<int> l_HSV,
        int areaLim
    ) : _camIdx(camIndex), _frameSize(frameWidth, frameHeight), 
        _options_autoBrightness(setAutoBrightness), _options_useThreads(useThreads),
        _morphological_schedule__size(morphological_schedule__size), _u_HSV(u_HSV), _l_HSV(l_HSV), _areaLim(areaLim)
    {
        if (callInit) {
            this->init(camIndex, frameWidth, frameHeight, setAutoBrightness, useThreads, morphological_schedule__size, u_HSV, l_HSV, areaLim);
        }
    }

    camObjTracker::~camObjTracker() {
        if (_isRunning) {
            _isRunning = false;
            if (_thread_processCam.joinable()) {
                _thread_processCam.join();
            }
        }
        if (_cap.isOpened()) {
            _cap.release();
        }
    }

    bool camObjTracker::init() {
        return init(_camIdx, static_cast<int>(_frameSize.x), static_cast<int>(_frameSize.y), _options_autoBrightness, _options_useThreads);
    }
    bool camObjTracker::init(
        int camIndex,
        int frameWidth = 640,
        int frameHeight = 480,
        bool setAutoBrightness = true,
        bool useThreads = true,
        std::vector<int> morphological_schedule__size = {-1, 6},
        std::vector<int> u_HSV = {180, 255, 255},
        std::vector<int> l_HSV = {0, 0, 0},
        int areaLim = 1000
    ) {
        if (_isRunning) {
            std::cerr << "Camera is already running!" << std::endl;
            return false;
        }
        if (_init) {
            std::cerr << "Class object is already initialized!" << std::endl;
            return false;
        }

        _camIdx = camIndex;
        _frameSize = pos2d<float>(frameWidth, frameHeight);
        _options_useThreads = useThreads;
        _options_autoBrightness = setAutoBrightness;
        _morphological_schedule__size = morphological_schedule__size;
        _u_HSV = u_HSV;
        _l_HSV = l_HSV;
        _areaLim = areaLim;


        _cap.open(_camIdx);
        if (!_cap.isOpened()) {
            std::cerr << "Failed to open camera with index: " << _camIdx << std::endl;
            return false;
        }

        if (_options_autoBrightness) {
            _cap.set(cv::CAP_PROP_AUTO_EXPOSURE, 1);
        }
        _cap.set(cv::CAP_PROP_FRAME_WIDTH, static_cast<double>(_frameSize.x));
        _cap.set(cv::CAP_PROP_FRAME_HEIGHT, static_cast<double>(_frameSize.y));


        ptr_data_write = &_data_write;
        ptr_data_read = &_data_read;

        if (_options_useThreads) {
            _thread_processCam = std::thread(camObjTracker_processThreadFunc, this);
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            if (!_thread_processCam.joinable()) {
                std::cerr << "Failed to create thread for camera processing!" << std::endl;
                return false;
            }
        } else {
            // Call the process function directly
            this->_process__1_readCam();
            this->_process__2_resizeImg();
            this->_process__3_cvt();
            this->_process__4_colourFilter();
            this->_process__5_morphologicalOps();
            this->_process__6_findContours();
        }

        _init = true;
        return true;
    }


    void camObjTracker::setAutoBrightness(bool setAutoBrightness) {
        std::lock_guard<std::mutex> lock(_mtx_dataAccess);
        if (setAutoBrightness == _options_autoBrightness) {
            return;
        }
        if (_cap.isOpened()) {
            _cap.set(cv::CAP_PROP_AUTO_EXPOSURE, setAutoBrightness ? 1 : 0);
        }
        _options_autoBrightness = setAutoBrightness;
    }
    void camObjTracker::setFrameSize(int width, int height) {
        std::lock_guard<std::mutex> lock(_mtx_dataAccess);
        if (width <= 0 || height <= 0) {
            std::cerr << "Frame size must be positive!" << std::endl;
            return;
        }
        if (_cap.isOpened()) {
            _cap.set(cv::CAP_PROP_FRAME_WIDTH, width);
            _cap.set(cv::CAP_PROP_FRAME_HEIGHT, height);
        }
        _frameSize.x = width;
        _frameSize.y = height;
    }
    void camObjTracker::setAreaLim(int areaLim) {
        std::lock_guard<std::mutex> lock(_mtx_dataAccess);
        if (areaLim == _areaLim) {
            return;
        }
        if (areaLim < 0) {
            std::cerr << "Area limit must be non-negative!" << std::endl;
            return;
        }
        _areaLim = areaLim;
    }
    void camObjTracker::setMorphologicalSchedule(std::vector<int> schedule) {
        std::lock_guard<std::mutex> lock(_mtx_dataAccess);
        if (schedule == _morphological_schedule__size) {
            return;
        }
        _morphological_schedule__size = schedule;
    }
    void camObjTracker::setHSV(std::vector<int> u_HSV, std::vector<int> l_HSV) {
        std::lock_guard<std::mutex> lock(_mtx_dataAccess);
        if (u_HSV == _u_HSV && l_HSV == _l_HSV) {
            return;
        }
        _u_HSV = u_HSV;
        _l_HSV = l_HSV;
    }
    

    void camObjTracker::_process__1_readCam() {
        _cap >> ptr_data_write->imgRaw;
        if (ptr_data_write->imgRaw.empty()) {
            std::cerr << "Failed to read frame from camera!" << std::endl;
        }
    }
    void camObjTracker::_process__2_resizeImg() {
        // cv::resize(ptr_data_write->imgRaw, ptr_data_write->imgOriginal, cv::Size(static_cast<int>(_frameSize.x), static_cast<int>(_frameSize.y)));
        ptr_data_write->imgOriginal = ptr_data_write->imgRaw.clone();
        cv::flip(ptr_data_write->imgOriginal, ptr_data_write->imgFlipped, 1);
    }
    void camObjTracker::_process__3_cvt() {
        cv::cvtColor(ptr_data_write->imgFlipped, ptr_data_write->imgHSV, cv::COLOR_BGR2HSV);
    }
    void camObjTracker::_process__4_colourFilter() {
        cv::inRange(ptr_data_write->imgHSV, cv::Scalar(_l_HSV[0], _l_HSV[1], _l_HSV[2]), 
                    cv::Scalar(_u_HSV[0], _u_HSV[1], _u_HSV[2]), ptr_data_write->imgThreshold);
    }
    void camObjTracker::_process__5_morphologicalOps() {
        for (int op : _morphological_schedule__size) {
            if (op > 0) {
                cv::dilate(ptr_data_write->imgThreshold, ptr_data_write->imgThreshold, cv::Mat(), cv::Point(-1, -1), op);
            } else if (op < 0) {
                cv::erode(ptr_data_write->imgThreshold, ptr_data_write->imgThreshold, cv::Mat(), cv::Point(-1, -1), -op);
            }
        }
    }
    void camObjTracker::_process__6_findContours() {
        cv::findContours(ptr_data_write->imgThreshold, ptr_data_write->contours, ptr_data_write->hierarchy, 
                         cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);

        ptr_data_write->cnt_area = 0;
        ptr_data_write->allCnt_pos.clear();

        for (const auto& contour : ptr_data_write->contours) {
            double area = cv::contourArea(contour);
            if (area > _areaLim) {
                cv::Moments m = cv::moments(contour);
                if (m.m00 != 0) {
                    pos2d<float> center(static_cast<float>(m.m10 / m.m00), static_cast<float>(m.m01 / m.m00));
                    ptr_data_write->allCnt_pos.push_back(center);
                    ptr_data_write->cnt_area += static_cast<float>(area);
                }
            }
        }

        if (ptr_data_write->allCnt_pos.size() > 0) {
            pos2d<float> avgPos(0, 0);
            for (const auto& pos : ptr_data_write->allCnt_pos) {
                avgPos.x += pos.x;
                avgPos.y += pos.y;
            }
            avgPos.x /= static_cast<float>(ptr_data_write->allCnt_pos.size());
            avgPos.y /= static_cast<float>(ptr_data_write->allCnt_pos.size());
            ptr_data_write->cnt_pos = avgPos;
        }
    }
    

    bool camObjTracker::isInit() {
        return _init;
    }
    bool camObjTracker::isRunning() {
        return _isRunning;
    }
    camObjTrackerData camObjTracker::data() {
        std::lock_guard<std::mutex> lock(_mtx_dataSwitch);
        return *ptr_data_read;
    }

    camObjTrackerData camObjTracker::updateData() {
        if(!isInit()) {
            std::cerr << "Class object is not initialized!" << std::endl;
            return;
        }
        if(_options_useThreads) {
            std::cerr << "Class object is running in thread mode. Use `data()` to get the latest data." << std::endl;
            return;
        }
        
        _process__1_readCam();
        _process__2_resizeImg();
        _process__3_cvt();
        _process__4_colourFilter();
        _process__5_morphologicalOps();
        _process__6_findContours();

        auto temp = ptr_data_write;
        ptr_data_write = ptr_data_read;
        ptr_data_read = temp;

        return *ptr_data_read;
    }
}