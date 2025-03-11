#pragma once
#ifndef HPP__HEXCLAW_REMOTE_GUI__GLOBAL_VARIABLES
#define HPP__HEXCLAW_REMOTE_GUI__GLOBAL_VARIABLES

#include "includes.hpp"
#include "imgui_extras.hpp"
#include <diy_dictionary.hpp>

#include <thread>
#include <mutex>
#include <atomic>

#include <vector>

#include <wirelessCOM.hpp>
#include <nodeChart/extra_imgui.hpp>


extern std::string programCWD; //NOT USED
extern std::string __DIR_PROJECT_ROOT;
extern std::string __DIR_PROJECT_GCODE;


extern ALLEGRO_BITMAP *bitmap_test;
extern ALLEGRO_DISPLAY* display;


/**
 * 
 * typed dict to hold the different image formats and their properties with their format name as key, with case sensitive
 * ex:
 *  ```
 *  imageFormats {
 *      "HSV"   : {
 *          "n-bytes" : int
 *      },
 *      "RGB"   : {
 *          "n-bytes" : int
 *      },
 *      "RGBA"  : {
 *          "n-bytes" : int
 *      },
 *      "GRAY"  : {
 *          "n-bytes" : int
 *      }
 *  }
 *  ```
 */
extern DIY::typed_dict<std::string, DIY::typed_dict<std::string, size_t>> imageFormats;

extern int mode;

/**
 * performance check on the loadBitmap_fromBitArrray function
 */
extern PERF::perf_isolated perf_loadBitmap_func;


inline void* getPixelPtr(
    size_t x,
    size_t y,
    ALLEGRO_LOCKED_REGION* lockedReg
) {
    // lockedReg->data
    return ((char*)lockedReg->data + x*lockedReg->pixel_size + lockedReg->pitch*y);
}


inline void HelpMarker(const char* desc, const char* symb="(?)") {
    ImGui::TextDisabled(symb);
    if (ImGui::BeginItemTooltip())
    {
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

//not sure if allegro5 is thread safe, so not sure if i can safely call al_put_pixel whilst running other allegro methods..
/// Mutex specifically for processing the bitArrays into ALLEGRO_BITMAP variable
// std::mutex mtx_allegroBitmapProcess;

class al_bmp_threadClass {
    private:
    ALLEGRO_BITMAP*         bmp;
    bool _init = false;

    public:
    /// @brief main std::mutex object for communicating between the members of this object
    std::mutex              mtx;
    /// @brief Main grayscale data container/array
    std::vector<uint8_t>    arr;
    std::string             col_format;
    std::atomic<size_t>     width;
    std::atomic<size_t>     height;
    std::atomic<bool>       incomplete{false};
    std::atomic<bool>       running{true};
    std::atomic<bool>*      runningPtr;
    std::atomic<bool>       newTask{false};
    std::atomic<bool>       localRunning{false}; //boolean to check whether the threadTask function is running

    ALLEGRO_BITMAP* BMP() { return bmp; }
    al_bmp_threadClass(
        size_t _width,
        size_t _height,
        std::string _format,
        std::atomic<bool>* _runner = nullptr,
        bool _incomplete = false
    ): _init(true), width(_width), height(_height), col_format(_format), incomplete(_incomplete) {
        if(!_runner) runningPtr = &(this->running);
        else runningPtr = _runner;
    }
    // ~al_bmp_threadClass() {
    //     al_destroy_bitmap(bmp);
    // }
    void destroy() {
        assert(this->_init);
        al_destroy_bitmap(bmp);
    }
    
    bool init() {
        this->bmp = al_create_bitmap(width, height);
        return (bmp);
    }
};
/**
 * @brief Dedicated class object instance for thread bitmap loading
 * 
 */
extern al_bmp_threadClass bmpObj;

inline void threadTask_bitArrayProcess(
    al_bmp_threadClass* classBMP_obj
) {
    std::unique_lock<std::mutex> u_lck_al_bmp((*classBMP_obj).mtx, std::defer_lock);
    (*classBMP_obj).localRunning = true;
    while((*classBMP_obj).runningPtr->load()) {
        if((*classBMP_obj).newTask.load()) {
            // mtx_print("tBit: new task");
            (*classBMP_obj).mtx.lock();
        
            if(loadBitmap_fromBitArray(
                (*classBMP_obj).BMP(),
                &((*classBMP_obj).arr),
                (*classBMP_obj).col_format,
                (*classBMP_obj).width,
                (*classBMP_obj).height,
                (*classBMP_obj).incomplete
            )) {
                
            }

            (*classBMP_obj).newTask = false;
            (*classBMP_obj).mtx.unlock();

            // mtx_print("size: "+formatNumber((*classBMP_obj).arr.size(), 0, 0));
            // mtx_print("tBit: task finished");
        }
        else {
            
        }
        (*classBMP_obj).newTask = false;
    }
    (*classBMP_obj).localRunning = false;
}

// extern ALLEGRO_THREAD   *th_allegThread;
// extern ALLEGRO_MUTEX    *th_allegMutex;
// extern ALLEGRO_COND     *th_allegCond;
/// @brief 
/// @param th_alleg 
/// @param arg 
/// @return 
/// @warning NOT USED
inline void* th_allegFunc(ALLEGRO_THREAD* th_alleg, void* arg) {
    al_bmp_threadClass& bmpClassObj = bmpClassObj;
    std::unique_lock<std::mutex> u_lck_al_bmp(bmpClassObj.mtx, std::defer_lock);

    bmpClassObj.localRunning = true;
    while(bmpClassObj.runningPtr->load() /*&& !al_get_thread_should_stop(th_allegThread)*/) {
        if(bmpClassObj.newTask.load()) {
            // mtx_print("T1: new task:");
            bmpClassObj.mtx.lock();
            // al_lock_mutex(th_allegMutex);
            
            loadBitmap_fromBitArray(
                bmpClassObj.BMP(),
                &(bmpClassObj.arr),
                bmpClassObj.col_format,
                bmpClassObj.width,
                bmpClassObj.height,
                bmpClassObj.incomplete
            );

            bmpClassObj.newTask = false;
            bmpClassObj.mtx.unlock();
            
            bmpClassObj.newTask = false;
            
        }
        else {
            
        }
    }
    bmpClassObj.localRunning = false;
}

class threadClass_telemetry_receiver;
void threadClass_telemetry_receiver_main_loop(threadClass_telemetry_receiver *telemPtr);
class threadClass_telemetry_receiver {
    private:
    std::string _info_name = "threadClass_telemetry_receiver";

    /// @brief Main std::thread object running the telemetry receiver
    std::thread _threadObj;
    
    bool _init = false;
    
    std::atomic<bool> _run_loop{true};
    
    nodemcu_orient _orientObj;
    
    std::string _orientObj_IP   = DEFAULT__IPADDR;
    int         _orientObj_PORT = DEFAULT__PORT;

    // void var_init();

    public:
    vec3<float> data_accelerometer{0, 0, 0};  // accelerometer values
    vec3<float> data_gyroscope{0, 0, 0};      // gyroscope values
    vec3<float> data_tilt{0, 0, 0};           // filtered tilt variables: {x: yaw, y: pitch, z: roll}
    vec3<float> data_tilt_RAW{0, 0, 0};       // raw tilt variables: {x: yaw, y: pitch, z: roll}

    std::chrono::milliseconds loop_delay_milliseconds{30};   // minimum millisecond duration per thread function loop iteration.

    std::mutex mtx_telemetry_data;   // Main std::mutex for accessing the public `data_` variables

    threadClass_telemetry_receiver(threadClass_telemetry_receiver&& other) = default;
    threadClass_telemetry_receiver(std::string _board_IP=DEFAULT__IPADDR, int _board_PORT=DEFAULT__PORT, bool _initialise=true);
    threadClass_telemetry_receiver(bool _initialise);
    ~threadClass_telemetry_receiver();

    int init();
    bool isInit();
    void join();
    
    friend void threadClass_telemetry_receiver_main_loop(threadClass_telemetry_receiver *telemPtr);

};


// boolean for whether the main program loop is to be running (this set to `false` will close the program)
extern std::atomic<bool> running;

extern NETWORK_DATA_THREADCLASS t_bitArr;

// extern nodemcu_orient orientObj;
extern threadClass_telemetry_receiver telemetryObj;

extern pos3d<float> input_IK_pos;
extern pos3d<float> input_IK_orient;

extern servo_angles_6DOF output_IK_angles; //mtx dependant

// float output_FK_pos[3]   = {0, 0, 0};
// float output_FK_orient[3]= {0, 0, 0};


extern pressed_key__struct guiKeys;
/// @brief Main keyBinds handler class
extern DIY_KBH::keyBind_handler keyBinds;


extern bool input_IK_enterPress;

// extern bool keys__undo;
// extern bool keys__redo;


// extern bool _ctrl_enter__pressed; //`ctrl+enter`
// extern bool _undo__pressed; //`ctrl+z`
// extern bool _redo__pressed; //`ctrl+y` or `ctrl+shift+z`


// inline bool IsLegacyNativeDupe(ImGuiKey key) {
//     return key >= 0 && key < 512 && ImGui::GetIO().KeyMap[key] != -1;
// }

extern DIY::typed_dict<std::string, std::string> guiSettings_desc;
extern DIY::typed_dict<std::string, bool*> guiSettings;
// extern DIY::typed_dict<std::string, std::string> guiSettings

extern bool guisetting_link_to_server;
extern bool guisetting_findOrient;

//whether to take and display performance/delays throughout the "checkpoints"/"segments"
extern bool takePerf_tab_0;

/**
 * @brief Timeline/snapshot history "node"/change addition.
 * 
 * @tparam storeType - the type of variable stored in history vector
 * @param tmp reference to the temprorary object to add to the timeline. NOTE:this is meant to be the changed/new variant
 * @param historyVec reference to the vector that stores the history/snapshots
 * @param idx index of where on the history/snapshot vector that's currently indexed/used/displayed
 * @param _MAX maximum size of the history vector that when the length is beyond, will delete the oldest snapshot
 * @note if `idx` isn't at the end of the vector `historyVec` then this function will cut and delete everything after `idx` and add
 * then change from there, essentially pruning the branch
 */
template<typename storeType>
inline void basic_timeline_add(storeType& tmp, std::vector<storeType>& historyVec, int& idx, int _MAX) {
    //Timeline change: addition
    if(idx>=_MAX) { //timeline addition: same branch: beyond MAX lim
        historyVec.erase(historyVec.begin()); //delete oldest
        historyVec.push_back(tmp); //add new
    }
    else if(idx>=static_cast<int>(historyVec.size())-1) { //timeline addition: same branch: still whithin MAX lim
        historyVec.push_back(tmp);
        idx+=1;
    }
    else { //timeline addition: new branch creation:
        historyVec.erase(historyVec.begin()+idx+1, historyVec.end());
        historyVec.push_back(tmp);
        idx+=1;
    }
}

inline void Delays_table(PERF::perf_isolated& perfRef, const char* childID, const char* tableID, const char* sepText="") {
    
    if(ImGui::BeginChild(childID, ImVec2(WIN_INPUT_SETTINGS_WIDTH, 200))) {
        ImGui::SeparatorText(sepText);
        if(ImGui::BeginTable(tableID, 2)) {
            std::vector<float> perf_delays = perfRef.delays();
            std::vector<std::string> perf_names = perfRef.names();
            for(int i=0; i<perf_names.size(); i++) {
                ImGui::TableNextRow();
                // ImGui::SetNextItemWidth(perf_name.length());
                ImGui::TableSetColumnIndex(0);
                // ImGui::AlignTextToFramePadding();
                ImGui::TextUnformatted(perf_names[i].c_str());
                ImGui::TableSetColumnIndex(1);
                // ImGui::TextUnformatted("test");
                // ImGui::SetNextItemWidth(5);
                // ImGui::TableSetColumnIndex(1);
                // ImGui::AlignTextToFramePadding();
                ImGui::Text((formatNumber<float>(perf_delays[i],5,2)+"ms").c_str());
                // if(i>=5) break;
            }
            ImGui::EndTable();
        }
        ImGui::EndChild();
    }
}

#endif