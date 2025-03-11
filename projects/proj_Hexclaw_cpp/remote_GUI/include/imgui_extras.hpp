#pragma once
#ifndef HPP__IMGUI_EXTRAS
#define HPP__IMGUI_EXTRAS

#include "includes.hpp"

inline void ToggleButton(const char* str_id, bool* v, void (*callBack_func)(void*)=nullptr, void *context=nullptr) {
    ImVec2 p = ImGui::GetCursorScreenPos();
    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    float height = ImGui::GetFrameHeight();
    float width = height * 1.55f;
    float radius = height * 0.50f;

    if (ImGui::InvisibleButton(str_id, ImVec2(width, height)))
        *v = !*v;
    ImU32 col_bg;
    if (ImGui::IsItemHovered())
        col_bg = *v ? IM_COL32(145+20, 211, 68+20, 255) : IM_COL32(218-20, 218-20, 218-20, 255);
    else
        col_bg = *v ? IM_COL32(145, 211, 68, 255) : IM_COL32(218, 218, 218, 255);

    draw_list->AddRectFilled(p, ImVec2(p.x + width, p.y + height), col_bg, height * 0.5f);
    draw_list->AddCircleFilled(ImVec2(*v ? (p.x + width - radius) : (p.x + radius), p.y + radius), radius - 1.5f, IM_COL32(255, 255, 255, 255));
    if(callBack_func) callBack_func(context);
}


#define _BM_DEFINE false

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
    // assert(lockedReg);

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


#endif