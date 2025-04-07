#include "camObjTracker.hpp"

namespace CVTRACK {

    camObjTracker::camObjTracker(
        bool __callInit,
        int __camIndex,
        int __frameWidth,
        int __frameHeight,
        bool __setAutoBrightness,
        bool __useThreads,
        std::vector<int> __morphological_schedule__size,
        std::vector<int> __l_HSV,
        std::vector<int> __u_HSV,
        int areaLim
    ) : _camIdx(__camIndex), _frameSize(__frameWidth, __frameHeight), 
        _options_autoBrightness(__setAutoBrightness), _options_useThreads(__useThreads),
        _morphological_schedule__size(__morphological_schedule__size), _u_HSV(__u_HSV), _l_HSV(__l_HSV), _areaLim(areaLim)
    {
        if (__callInit) {
            this->init(__camIndex, __frameWidth, __frameHeight, __setAutoBrightness, __useThreads, __morphological_schedule__size, __u_HSV, __l_HSV, areaLim);
        }
    }
    camObjTracker::camObjTracker(const camObjTracker& _other) {
        // Copy constructor implementation
        _cap = _other._cap;
        // _thread_processCam = _other._thread_processCam;
        _options_autoBrightness = _other._options_autoBrightness;
        _options_useThreads = _other._options_useThreads;
        _init = _other._init;
        _isRunning = _other._isRunning.load();
        ptr_data_write = &_data_write;
        ptr_data_read = &_data_read;
        _frameSize = _other._frameSize;
        _areaLim = _other._areaLim;
        _l_HSV = _other._l_HSV;
        _u_HSV = _other._u_HSV;
        _camIdx = _other._camIdx;
        _morphological_schedule__size = _other._morphological_schedule__size;
    }
    camObjTracker::camObjTracker(camObjTracker&& _other) noexcept
        : _cap(std::move(_other._cap)),
        _thread_processCam(std::move(_other._thread_processCam)),
        _options_autoBrightness(_other._options_autoBrightness),
        _options_useThreads(_other._options_useThreads),
        _init(_other._init),
        _isRunning(_other._isRunning.load()),
        _data_write(std::move(_other._data_write)),
        _data_read(std::move(_other._data_read)),
        _frameSize(_other._frameSize),
        _areaLim(_other._areaLim),
        _l_HSV(std::move(_other._l_HSV)),
        _u_HSV(std::move(_other._u_HSV)),
        _camIdx(_other._camIdx),
        _morphological_schedule__size(std::move(_other._morphological_schedule__size))
    {
        // Update pointers to point to the moved data
        ptr_data_write = &_data_write;
        ptr_data_read = &_data_read;

        // Reset the state of _other
        _other._init = false;
        _other._isRunning = false;
        _other.ptr_data_write = nullptr;
        _other.ptr_data_read = nullptr;
    }
    camObjTracker& camObjTracker::operator=(camObjTracker&& _other) {
        if (this != &_other) {
            // Stop the current thread if running
            if (_isRunning) {
                _isRunning = false;
                if (_thread_processCam.joinable()) {
                    _thread_processCam.join();
                }
            }
    
            // Release the current camera if opened
            if (_cap.isOpened()) {
                _cap.release();
            }
    
            // Move all members from _other to this instance
            _cap = std::move(_other._cap);
            // _mtx_dataSwitch = std::move(_other._mtx_dataSwitch);
            // _mtx_dataAccess = std::move(_other._mtx_dataAccess);
            _thread_processCam = std::move(_other._thread_processCam);
    
            _options_autoBrightness = _other._options_autoBrightness;
            _options_useThreads = _other._options_useThreads;
            _init = _other._init;
            _isRunning = _other._isRunning.load();
    
            _data_write = std::move(_other._data_write);
            _data_read = std::move(_other._data_read);
            ptr_data_write = &_data_write;
            ptr_data_read = &_data_read;
    
            _frameSize = _other._frameSize;
            _areaLim = _other._areaLim;
            _l_HSV = std::move(_other._l_HSV);
            _u_HSV = std::move(_other._u_HSV);
            _camIdx = _other._camIdx;
            _morphological_schedule__size = std::move(_other._morphological_schedule__size);
    
            // Reset the state of _other
            _other._init = false;
            _other._isRunning = false;
            _other.ptr_data_write = nullptr;
            _other.ptr_data_read = nullptr;
        }
        return *this;
    }
    camObjTracker& camObjTracker::operator=(const camObjTracker& _other) {
        if (this != &_other) {
            // Copy all members from _other to this instance
            _cap = _other._cap;
            // _thread_processCam = _other._thread_processCam;
            _options_autoBrightness = _other._options_autoBrightness;
            _options_useThreads = _other._options_useThreads;
            // _init = _other._init;
            // _isRunning = _other._isRunning.load();
            ptr_data_write = &_data_write;
            ptr_data_read = &_data_read;
            _frameSize = _other._frameSize;
            _areaLim = _other._areaLim;
            _l_HSV = _other._l_HSV;
            _u_HSV = _other._u_HSV;
            _camIdx = _other._camIdx;
            _morphological_schedule__size = _other._morphological_schedule__size;
        }
        return *this;
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
        int __camIndex,
        int __frameWidth,
        int __frameHeight,
        bool __setAutoBrightness,
        bool __useThreads,
        std::vector<int> __morphological_schedule__size,
        std::vector<int> __l_HSV,
        std::vector<int> __u_HSV,
        int areaLim
    ) {
        if (_isRunning) {
            throw std::runtime_error("Camera is already running!");
            return false;
        }
        if (_init) {
            throw std::runtime_error("Class object is already initialized!");
            return false;
        }

        putenv("OPENCV_VIDEOIO_PRIORITY_MSMF=0"); // Disable MSMF backend for Windows

        _camIdx = __camIndex;
        _frameSize = pos2d<float>(__frameWidth, __frameHeight);
        _options_useThreads = __useThreads;
        _options_autoBrightness = __setAutoBrightness;
        _morphological_schedule__size = __morphological_schedule__size;
        _u_HSV = __u_HSV;
        _l_HSV = __l_HSV;
        _areaLim = areaLim;


        _cap.open(_camIdx);
        if (!_cap.isOpened()) {
            throw std::runtime_error("Failed to open camera with index: " + std::to_string(_camIdx));
            return false;
        }

        if (_options_autoBrightness) {
            _cap.set(cv::CAP_PROP_AUTO_EXPOSURE, 1);
        }
        // _cap.set(cv::CAP_PROP_FRAME_WIDTH, static_cast<double>(_frameSize.x));
        // _cap.set(cv::CAP_PROP_FRAME_HEIGHT, static_cast<double>(_frameSize.y));


        ptr_data_write = &_data_write;
        ptr_data_read = &_data_read;

        if (_options_useThreads) {
            _thread_processCam = std::thread(&CVTRACK::camObjTracker::camObjTracker_processThreadFunc, this);
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            if (!_thread_processCam.joinable()) {
                throw std::runtime_error("Failed to create thread for camera processing!");
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


    void camObjTracker::setAutoBrightness(bool __setAutoBrightness) {
        std::lock_guard<std::mutex> lock(_mtx_dataAccess);
        if (__setAutoBrightness == _options_autoBrightness) {
            return;
        }
        if (_cap.isOpened()) {
            _cap.set(cv::CAP_PROP_AUTO_EXPOSURE, __setAutoBrightness ? 1 : 0);
        }
        _options_autoBrightness = __setAutoBrightness;
    }
    void camObjTracker::setFrameSize(int width, int height) {
        std::lock_guard<std::mutex> lock(_mtx_dataAccess);
        if (width <= 0 || height <= 0) {
            throw std::runtime_error("Frame size must be positive!");
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
            throw std::runtime_error("Area limit must be non-negative!");
            return;
        }
        _areaLim = areaLim;
    }
    void camObjTracker::setMorphologicalSchedule(std::vector<int> __schedule) {
        std::lock_guard<std::mutex> lock(_mtx_dataAccess);
        if (__schedule == _morphological_schedule__size) {
            return;
        }
        _morphological_schedule__size = __schedule;
    }
    void camObjTracker::setHSV(std::vector<int> __l_HSV, std::vector<int> __u_HSV) {
        if (__u_HSV.size() != 3 || __l_HSV.size() != 3) {
            throw std::runtime_error("HSV values must be of size 3!");
            return;
        }
        if (__u_HSV == _u_HSV && __l_HSV == _l_HSV) {
            return;
        }
        std::lock_guard<std::mutex> lock(_mtx_dataAccess_hsv);
        _u_HSV = __u_HSV;
        _l_HSV = __l_HSV;
    }
    void camObjTracker::setConsolePrintMutex(std::mutex* __mtx_cout) {
        if(!_options_useThreads) {
            throw std::runtime_error("setConsolePrintMutex(): Class object is not running in thread mode!");
            return;
        }
        std::lock_guard<std::mutex> lock(_mtx_dataAccess);
        if (__mtx_cout == nullptr) {
            throw std::runtime_error("Console print mutex cannot be null!");
            return;
        }
        _mtx_cout = __mtx_cout;
    }


    void camObjTracker::exitThreadLoop() {
        _isRunning = false;
        if (_thread_processCam.joinable()) {
            _thread_processCam.join();
        }
    }

    void camObjTracker::_process__1_readCam() {
        _cap >> ptr_data_write->imgRaw;
        if (ptr_data_write->imgRaw.empty()) {
            throw std::runtime_error("Failed to read frame from camera!");
        }
    }
    void camObjTracker::_process__2_resizeImg() {
        cv::resize(ptr_data_write->imgRaw, ptr_data_write->imgOriginal, cv::Size(static_cast<int>(_frameSize.x), static_cast<int>(_frameSize.y)));
        // ptr_data_write->imgOriginal = ptr_data_write->imgRaw.clone();
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
    bool camObjTracker::isThreadLoopInit() {
        if(!_options_useThreads) {
            throw std::runtime_error("isThreadLoopInit(): Class object is not running in thread mode!");
        }
        if(!_init) {
            throw std::runtime_error("isThreadLoopInit(): Class object is not initialized!");
        }
        return _threadLoopInit;
    }
    bool camObjTracker::isRunning() {
        if(!_options_useThreads) {
            throw std::runtime_error("isRunning(): Class object is not running in thread mode!");
        }
        return _isRunning;
    }
    camObjTrackerData camObjTracker::data() {
        if(!_options_useThreads) {
            std::cout << "WARNING: Class object is not running in thread mode. Use `updateData()` to get the latest data." << std::endl;
        }
        else {
            std::lock_guard<std::mutex> lock(_mtx_dataSwitch);
        }
        return *ptr_data_read;
    }

    camObjTrackerData& camObjTracker::updateData() {
        if(!isInit()) {
            throw std::runtime_error("Class object is not initialized!");
        }
        if(_options_useThreads) {
            throw std::runtime_error("Class object is running in thread mode. Use `data()` to get the latest data.");
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