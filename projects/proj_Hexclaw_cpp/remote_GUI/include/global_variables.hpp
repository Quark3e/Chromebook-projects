#pragma once
#ifndef HPP__HEXCLAW_REMOTE_GUI__GLOBAL_VARIABLES
#define HPP__HEXCLAW_REMOTE_GUI__GLOBAL_VARIABLES

#include "includes.hpp"


extern ALLEGRO_BITMAP *bitmap_test;

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

extern ALLEGRO_DISPLAY* display;

// extern int my_image_width;
// extern int my_image_height;
// extern GLuint my_image_texture;


extern int mode;

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
    std::vector<uint8_t> _bitArray,
    std::string _colourFormat,
    size_t _width,
    size_t _height,
    bool _incompleteArray = false
) {
    static bool init = true;
    assert(_bitmap);
    size_t numBytes = imageFormats.get(_colourFormat).get("n-bytes");
    if(
        (!_incompleteArray && _bitArray.size() != numBytes * _width * _height)
        // imageFormats.find(_colourFormat)
    ) {
        return false;
    }

    int pixel = 0, currByte = 0;
    size_t pos[2] = {0, 0};
    ALLEGRO_COLOR col;
    al_set_target_bitmap(_bitmap);
    
    for(size_t i=0; i<_bitArray.size(); i++) {
        if(currByte>=numBytes-1) { //new pixel
            pos[0] = i%_width;
            pos[1] = floor(float(i)/_height);
            int startIdx = i-numBytes;
            // if(init && _bitArray[startIdx]==250) std::cout << startIdx << std::endl;
            
            if(_colourFormat=="HSV") {

            }
            else if(_colourFormat=="RGB") {
                col = al_map_rgb(unsigned(_bitArray[startIdx]), unsigned(_bitArray[startIdx+1]), unsigned(_bitArray[startIdx+2]));
            }
            else if(_colourFormat=="RGBA") {
                col = al_map_rgba(unsigned(_bitArray[startIdx]), unsigned(_bitArray[startIdx+1]), unsigned(_bitArray[startIdx+2]), unsigned(_bitArray[startIdx+3]));
            }
            else if(_colourFormat=="GRAY") {
                col = al_map_rgb(unsigned(_bitArray[startIdx]), unsigned(_bitArray[startIdx]), unsigned(_bitArray[startIdx]));
                // col = al_map_rgb(200, 200, 200);
            }
            al_put_pixel(pos[0], pos[1], col);

            pixel++;
            currByte = 0;
        }
        else {
            currByte++;
        }
    }

    al_set_target_backbuffer(display);

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

// boolean for whether the main program loop is to be running (this set to `false` will close the program)
extern bool running;

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