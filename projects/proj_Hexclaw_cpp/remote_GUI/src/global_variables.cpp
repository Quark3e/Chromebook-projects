
#include "../include/global_variables.hpp"


#if _WIN32
std::string __DIR_PROJECT_ROOT  = "/Users/berkh/Projects/Github_repo/Chromebook-projects/projects/proj_Hexclaw_cpp/remote_GUI/";
std::string __DIR_PROJECT_GCODE = "/Users/berkh/Projects/Github_repo/Chromebook-projects/projects/proj_Hexclaw_cpp/gCode/";
#else
std::string __DIR_PROJECT_ROOT  = "/home/berkhme/github_repo/Chromebook-projects/projects/proj_Hexclaw_cpp/remote_GUI/";
std::string __DIR_PROJECT_GCODE = "/home/berkhme/github_repo/Chromebook-projects/projects/proj_Hexclaw_cpp/gCode/";
#endif

std::mutex mtx_cout;

ALLEGRO_BITMAP *bitmap_test;

DIY::typed_dict<std::string, DIY::typed_dict<std::string, size_t>> imageFormats(
    {"HSV", "RGB", "RGBA", "GRAY"}, {
        DIY::typed_dict<std::string, size_t>(
            {"n-bytes"}, {3}),
        DIY::typed_dict<std::string, size_t>(
            {"n-bytes"}, {3}),
        DIY::typed_dict<std::string, size_t>(
            {"n-bytes"}, {4}),
        DIY::typed_dict<std::string, size_t>(
            {"n-bytes"}, {1})
    }
);

ALLEGRO_DISPLAY* display = nullptr;

// int my_image_width = 0;
// int my_image_height = 0;
// GLuint my_image_texture = 0;


// ALLEGRO_THREAD  *th_allegThread;
// ALLEGRO_MUTEX   *th_allegMutex;
// ALLEGRO_COND    *th_allegCond;



PERF::perf_isolated perf_loadBitmap_func;


int mode = 0;


al_bmp_threadClass bmpObj(640, 480, "GRAY", &running);


// std::atomic<bool> threadClass_telemetry_receiver::_run_loop{true};
// vec3<float> threadClass_telemetry_receiver::data_accelerometer{0, 0, 0}; // accelerometer values
// vec3<float> threadClass_telemetry_receiver::data_gyroscope{0, 0, 0}; // gyroscope values
// vec3<float> threadClass_telemetry_receiver::data_tilt{0, 0, 0}; // filtered tilt variables: {x: yaw, y: pitch, z: roll}
// vec3<float> threadClass_telemetry_receiver::data_tilt_RAW{0, 0, 0}; // raw tilt variables: {x: yaw, y: pitch, z: roll}
// std::chrono::milliseconds threadClass_telemetry_receiver::loop_delay_milliseconds(30); // minimum millisecond duration per thread function loop iteration.
// nodemcu_orient threadClass_telemetry_receiver::_orientObj;
// std::string threadClass_telemetry_receiver::_orientObj_IP   = DEFAULT__IPADDR;
// int         threadClass_telemetry_receiver::_orientObj_PORT = DEFAULT__PORT;
// std::mutex  threadClass_telemetry_receiver::mtx_telemetry_data;

void threadClass_telemetry_receiver_main_loop(threadClass_telemetry_receiver *telemPtr) {
    std::unique_lock<std::mutex> u_lck_teleData(telemPtr->mtx_telemetry_data, std::defer_lock);
    // mtx_print("threadClass_telemetry_receiver: main_loop started.");
    while(telemPtr->_run_loop.load()) {
        auto start_time = std::chrono::steady_clock::now();
        try {
            telemPtr->_orientObj.update(false);
        } catch(const std::exception& e) {

        }
            
        u_lck_teleData.lock();
        telemPtr->data_accelerometer.newData(telemPtr->_orientObj.accel);
        telemPtr->data_gyroscope.newData(telemPtr->_orientObj.gyro);
        telemPtr->data_tilt.newData(0, telemPtr->_orientObj.pitch, telemPtr->_orientObj.roll);
        telemPtr->data_tilt_RAW.newData(0, telemPtr->_orientObj.Pitch, telemPtr->_orientObj.Roll);
        u_lck_teleData.unlock();

        // mtx_print("threadClass_telemetry_receiver: new data received.");
        auto diffTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now()-start_time);
        if(diffTime<telemPtr->loop_delay_milliseconds) {
            std::this_thread::sleep_for(telemPtr->loop_delay_milliseconds-diffTime);
        }
    }
}
// void threadClass_telemetry_receiver::var_init() {
//     _run_loop = true;

//     data_accelerometer  = vec3<float>{0, 0, 0}; // accelerometer values
//     data_gyroscope      = vec3<float>{0, 0, 0}; // gyroscope values
//     data_tilt           = vec3<float>{0, 0, 0}; // filtered tilt variables: {x: yaw, y: pitch, z: roll}
//     data_tilt_RAW       = vec3<float>{0, 0, 0}; // raw tilt variables: {x: yaw, y: pitch, z: roll}

//     loop_delay_milliseconds = std::chrono::milliseconds(30);    // minimum millisecond duration per thread function loop iteration.

//     _orientObj = nodemcu_orient(_orientObj_IP, _orientObj_PORT, false);
// }
threadClass_telemetry_receiver::threadClass_telemetry_receiver(std::string _board_IP, int _board_PORT, bool _initialise): _orientObj(_board_IP, _board_PORT, false) {
    // _orientObj_IP    = _board_IP;
    // _orientObj_PORT  = _board_PORT;
    // this->var_init();
    if(_initialise) {
        int init_code = 0;
        if((init_code = this->init())) throw std::runtime_error(_info_name+"::threadClass_telemetry_receiver(std::string, int, bool) --> this->init() failed: "+std::to_string(init_code));
    }
}
threadClass_telemetry_receiver::threadClass_telemetry_receiver(bool _initialise): _orientObj(false) {
    // _orientObj_IP    = DEFAULT__IPADDR;
    // _orientObj_PORT  = DEFAULT__PORT;
    // this->var_init();
    if(_initialise) {
        int init_code = 0;
        if((init_code = this->init())) throw std::runtime_error(_info_name+"::threadClass_telemetry_receiver(std::string, int, bool) --> this->init() failed: "+std::to_string(init_code));
    }
}
threadClass_telemetry_receiver::~threadClass_telemetry_receiver() {
    if(_threadObj.joinable()) {
        _run_loop = false;
        _threadObj.join();
    }
}
int threadClass_telemetry_receiver::init() {
    if(this->_init) {
        throw std::runtime_error(this->_info_name+"::init() has already been called.");
        // std::cout << this->_info_name+"::init() has already been called." << std::endl;
        // return 0;
    }
    int _orientObj_initCode = 0;
    if((_orientObj_initCode = _orientObj.connectObj.init())) {
        std::cout << this->_info_name+"::init() --> _orientObj.connectObj.init() failed." << std::endl;
        return -1;
    }
    assert(_orientObj.connectObj.isInit() && "somehow _orientObj.connectObj.isInit() is false even after successfull init call.");
    _threadObj = std::thread(threadClass_telemetry_receiver_main_loop, this);
    this->_init = true;
    return 0;
}
bool threadClass_telemetry_receiver::isInit() { return this->_init; }
void threadClass_telemetry_receiver::join() {
    // if(_threadObj.joinable()) {
        _run_loop = false;
        _threadObj.join();
    // }
}

threadClass_telemetry_receiver telemetryObject(false);

std::atomic<bool> running{true};

std::mutex print_mtx;
NETWORK_DATA_THREADCLASS t_bitArr(false, "192.168.1.177", 1086);

// nodemcu_orient orientObj("192.168.1.117", 1089, false);
threadClass_telemetry_receiver telemetryObj(false);


float input_IK_pos[3]    = {0, 200, 150};
float input_IK_orient[3] = {0, 0, 0};

float output_IK_angles[6]= {0, 0, 0, 0, 0, 0}; //mtx dependant

// float output_FK_pos[3]   = {0, 0, 0};
// float output_FK_orient[3]= {0, 0, 0};


pressed_key__struct guiKeys;

void keyBind_undo() {
    std::cout << "----- keyBind called: undo" << std::endl;
}
void keyBind_redo() {
    std::cout << "----- keyBind called: redo" << std::endl;
}
void keyBind_MasterClose() {
    std::cout << "----- keyBind called: Master Close" << std::endl;
    running = false;
}
DIY_KBH::keyBind_handler keyBinds({
    {"undo",    {ImGuiKey_LeftCtrl, ImGuiKey_ReservedForModCtrl, ImGuiKey_Z}, keyBind_undo},
    {"redo",    {ImGuiKey_LeftCtrl, ImGuiKey_ReservedForModCtrl, ImGuiKey_ReservedForModShift, ImGuiKey_LeftShift, ImGuiKey_Z}, keyBind_redo},
    {"Master Close",   {ImGuiKey_LeftCtrl, ImGuiKey_ReservedForModCtrl, ImGuiKey_W}, keyBind_MasterClose},
    {"ctrlEnter",{ImGuiKey_LeftCtrl,ImGuiKey_ReservedForModCtrl, ImGuiKey_Enter}, nullptr}
});


bool input_IK_enterPress = false;


DIY::typed_dict<std::string, std::string> guiSettings_desc= HW_KINEMATICS::setting_desc;
DIY::typed_dict<std::string, bool*> guiSettings;
// DIY::typed_dict<std::string, std::string> guiSettings

bool guisetting_link_to_server = true;
bool guisetting_findOrient = true;

bool takePerf_tab_0 = false;


