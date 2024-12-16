#pragma once
#ifndef HPP__HEXCLAW_REMOTE_GUI__GLOBAL_VARIABLES
#define HPP__HEXCLAW_REMOTE_GUI__GLOBAL_VARIABLES

#include <includes.hpp>

#include <diy_dictionary.hpp>

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

#define _BM_DEFINE false

inline void* getPixelPtr(
    size_t x,
    size_t y,
    ALLEGRO_LOCKED_REGION* lockedReg
) {
    return (lockedReg->data + x*lockedReg->pixel_size + lockedReg->pitch*y);
}

/**
 * @brief Load assigned ALLEGRO_BITMAP with uncompressed data/bits from a bit array
 * 
 * @param _bm_toLoad pointer to the ALLEGRO_BITMAP to load the data into/onto
 * @param _bitArray the bit-array/std::vector<uint8_t> of bits to load into the bitmap
 * @param _colourFormat name of the colour format/type to load the data as, which'll
 *  also set the colour channels and bit separation. Is defined according to `imageFormats`
 * @param _width number of pixels for the width
 * @param _height number of pixels for the height
 * @param _incompleteArray whether the input bitArray is incomplete/not-the-full-size. Used
 * as an indicator for whether the input `bitArray` is intentionally not the full expected size (n-channels * width * height).
 * If this is set to `true` then the function will still load in the existing data and return true. If false and the input bitArray
 * is not the expected size then it'll not load the data in and instead return false.
 * this is set to `true` the function will load the
 * @return true if the data has been successfully loaded in
 * @return false if an error occurred
 */
inline bool loadBitmap_fromBitArray(
    ALLEGRO_BITMAP* _bitmap,
    std::vector<uint8_t>* _bitArray,
    std::string _colourFormat,
    size_t _width,
    size_t _height,
    bool _incompleteArray = false
) {
    static bool init = true;
    assert(_bitmap);
    size_t numBytes = imageFormats.get(_colourFormat).get("n-bytes");
    if((!_incompleteArray && _bitArray->size() != numBytes * _width * _height)) {
        return false;
    }

    if(_BM_DEFINE) perf_loadBitmap_func.set_T0("set_target()");

    int pixel = 0, currByte = 0;
    size_t pos[2] = {0, 0};
    ALLEGRO_COLOR col;
    
    ALLEGRO_LOCKED_REGION* lockedReg = al_lock_bitmap(_bitmap, ALLEGRO_PIXEL_FORMAT_ABGR_8888, ALLEGRO_LOCK_WRITEONLY);
    if(!lockedReg) {
        return false;
    }
    assert(lockedReg);

    if(_BM_DEFINE) perf_loadBitmap_func.set_T1("set_target()");
    if(_BM_DEFINE) perf_loadBitmap_func.set_T0("loop start",  _bitArray->size());

    size_t iCnt = 0;
    size_t iLim = _bitArray->size() / 60;
    bool iTrue = true;

    for(size_t i=0; i<_bitArray->size(); i++) {
        if(currByte>=numBytes-1) { //new pixel
            
            // pos[0] = i%_width;
            // pos[1] = floor(float(i)/_height);
            // mtx_print("T1: newPixel: i:"+std::to_string(i));
            if(pos[0]>al_get_bitmap_width(_bitmap)) {
                std::cerr << "i:"<< i << " ";
                std::cerr << pos[0] << " - "<<al_get_bitmap_width(_bitmap);
                std::cerr << "; width x too far."<<std::endl;
                exit(1);
            }
            if(pos[1]>al_get_bitmap_height(_bitmap)) {
                std::cerr << "i:"<< i << " ";
                std::cerr << pos[1] << " - "<<al_get_bitmap_height(_bitmap);
                std::cerr << "; height y too far."<<std::endl;
                exit(1);
            }
            int startIdx = i-numBytes;

            uint32_t _colour = 0;
    
            // if(init && _bitArray->operator[](startIdx)==250) std::cout << startIdx << std::endl;
            if(iCnt<iLim) {
                iTrue = false;
                iCnt++;
            }
            else {
                iCnt = 0;
                iTrue = true;
            }
            // // std::cout << "i: " << i << std::endl;
            if(_BM_DEFINE && iTrue) perf_loadBitmap_func.set_T0("map colour", iLim);

            if(_colourFormat=="HSV") {
                std::vector<int> _RGB = convert_HSV_RGB({_bitArray->operator[](startIdx), _bitArray->operator[](startIdx), _bitArray->operator[](startIdx)});
                _colour = (255<<24) + (unsigned(_RGB[2])<<16) + (unsigned(_RGB[1])<<8) + (unsigned(_RGB[0]));
            }
            else if(_colourFormat=="RGB") {
                // col = al_map_rgb(unsigned(_bitArray->operator[](startIdx)), unsigned(_bitArray->operator[](startIdx+1)), unsigned(_bitArray->operator[](startIdx+2)));
                _colour = (255<<24) + (unsigned(_bitArray->operator[](startIdx+2))<<16) + (unsigned(_bitArray->operator[](startIdx+1))<<8) + unsigned(_bitArray->operator[](startIdx));
            }
            else if(_colourFormat=="RGBA") {
                // col = al_map_rgba(unsigned(_bitArray->operator[](startIdx)), unsigned(_bitArray->operator[](startIdx+1)), unsigned(_bitArray->operator[](startIdx+2)), unsigned(_bitArray->operator[](startIdx+3)));
                _colour = (unsigned(_bitArray->operator[](startIdx+3))<<24) + (unsigned(_bitArray->operator[](startIdx+2))<<16) + (unsigned(_bitArray->operator[](startIdx+1))<<8) + unsigned(_bitArray->operator[](startIdx));
            }
            else if(_colourFormat=="GRAY") {
                // col = al_map_rgb(unsigned(_bitArray->operator[](startIdx)), unsigned(_bitArray->operator[](startIdx)), unsigned(_bitArray->operator[](startIdx)));
                // col = al_map_rgb(200, 200, 200);
                // _colour = (255<<24) + (unsigned(_bitArray->operator[](startIdx))<<16) + (unsigned(_bitArray->operator[](startIdx))<<8) + unsigned(_bitArray->operator[](startIdx));
                uint8_t *ptr = _bitArray->data();
                _colour = (255<<24) + (unsigned(ptr[startIdx])<<16) + (unsigned(ptr[startIdx])<<8) + (unsigned(ptr[startIdx]));
            }
            if(_BM_DEFINE && iTrue) perf_loadBitmap_func.set_T1("map colour");
            // // if(_BM_DEFINE && iTrue) perf_loadBitmap_func.set_T0("al_put pix", 60);
            // al_put_pixel(pos[0], pos[1], col);
            if(_BM_DEFINE && iTrue) perf_loadBitmap_func.set_T0("assign px", iLim);


            // void* _temp = getPixelPtr(pos[0], pos[1], lockedReg);
            // uint32_t* _temp2 = (uint32_t*)(_temp);
            // *_temp2 = _colour;
            *((uint32_t*)(getPixelPtr(pos[0], pos[1], lockedReg))) = _colour; //0x208CE0; //BGR //00100000 10001100 11100000 //32 140 224


            if(_BM_DEFINE && iTrue) perf_loadBitmap_func.set_T1("assign px");

            pixel++;
            currByte = 0;
            if(pos[0]+1<_width) pos[0]++;
            else {
                pos[0] = 0;
                pos[1]++;
            }
        }
        else {
            currByte++;
        }
    }
    if(_BM_DEFINE) perf_loadBitmap_func.set_T1("loop start");
    if(_BM_DEFINE) perf_loadBitmap_func.set_T0("set_target 2()");

    // void* _temp = getPixelPtr(200, 200, lockedReg);
    // uint32_t* _temp2 = (uint32_t*)(_temp);
    // *_temp2 = 0xFF208CE0;

    // *((uint32_t*)(getPixelPtr(201, 200, lockedReg))) = 0xFF208CE0;
    // *((uint32_t*)(getPixelPtr(1, 1, lockedReg))) = 0xFF208CE0;
    // mtx_print("T1: unlock bitmap");
    al_unlock_bitmap(_bitmap);

    // mtx_print("bmp: new bitmap loaded");

    // al_set_target_backbuffer(display);
    if(_BM_DEFINE) perf_loadBitmap_func.set_T1("set_target 2()");

    if(init) init = false;
    return true;
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

// boolean for whether the main program loop is to be running (this set to `false` will close the program)
extern std::atomic<bool> running;

extern NETWORK_DATA_THREADCLASS t_bitArr;


extern float input_IK_pos[3];
extern float input_IK_orient[3];

extern float output_IK_angles[6]; //mtx dependant

// float output_FK_pos[3]   = {0, 0, 0};
// float output_FK_orient[3]= {0, 0, 0};

extern bool input_IK_enterPress;
extern bool keys__undo;
extern bool keys__redo;


extern bool _ctrl_enter__pressed; //`ctrl+enter`
extern bool _undo__pressed; //`ctrl+z`
extern bool _redo__pressed; //`ctrl+y` or `ctrl+shift+z`


inline bool IsLegacyNativeDupe(ImGuiKey key) {
    return key >= 0 && key < 512 && ImGui::GetIO().KeyMap[key] != -1;
}

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