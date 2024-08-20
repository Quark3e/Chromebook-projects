
#pragma once
#ifndef HPP_GUI_NODECHART
#define HPP_GUI_NODECHART


// #include "nodeChart.hpp"
#include <iostream>
#include <string>
#include <sstream>

#include <algorithm>

#include <vector>
#include <list>

#include <dirent.h>
#include <filesystem>


// #include <HC_useful/useful.hpp>

// #include "guiNC_constants.hpp"


template<typename _varType> int checkExistence(_varType toFind, const std::vector<_varType>& toSearch);
template<typename _varType> int checkExistence(_varType toFind, const std::list<_varType>& toSearch);
template<typename _varType> int checkExistence(_varType toFind, _varType toSearch[], int arrLen);
template<typename addrType> std::string ptrToStr(addrType toConv);

namespace gNC {

    struct gNODE;

    struct gLINK {
        std::string addr    = "";

        std::string label   = ""; //optional
        std::string desc    = ""; //optional

        /**
         * Integer code for source type: Whether it's "out" or "share":
         * - {`out`: `1`, `share`: `3`}
         */
        int type_src;
        /**
         * Integer code for dest type: WHether it's "in" our "add"
         * - {`in`: `0`, `add`: `2`}
         */
        int type_dest;

        gNC::gNODE* src;  // link source
        gNC::gNODE* dest; // link destination


        gLINK(
            int par_type_src, int par_type_dest,
            gNC::gNODE* par_src, gNC::gNODE* par_dest,
            std::string par_label="",std::string par_desc=""
        ) {
            if(par_type_src!=1 && par_type_src!=3 && par_type_dest!=0 && par_type_dest!=2) std::runtime_error("ERROR: NC::LINK() constructor par_type_{..} is invalid");

            this->type_src  = par_type_src;
            this->type_dest = par_type_dest;
            this->src  = par_src;
            this->dest = par_dest;
            this->label = par_label;
            this->desc  = par_desc;
        }
    };

    struct gNODE {
        std::string addr    = "";

        std::string label   = ""; //optional
        std::string desc    = ""; //optional
        std::string bodyText= ""; //optional

        bool init = false;

        /**
         * Different modes for the layout of the gNode:
         *  - `0` - horizontal: left->right
         *  - `1` - horizontal: right->left (text is left->right)
         *  - `2` - vertical:   top->down
         *  - `3` - vertical:   down->top
         */
        int layout;

        /// @details GUI related
        float width = 200; /// [unit: px] width of the bounding box ROI
        float height= 150; /// [unit: px] height of the bounding box ROI

        /// [unit: px] Absolute(relative to screen) {x, y} coordinate of the top left corner of this NODE ROI.
        float pos[2];

        float pos_in[2]     = {0,  25}; // [unit: px] Position of the `ln_in` node bounding box
        float pos_out[2]    = {100, 25}; // [unit: px] Position of the `ln_out` node bounding box
        float pos_add_0[2]  = {31,  0}; // [unit: px] Position of the `ln_add` node bounding box 
        float pos_add_1[2]  = {31, 50};
        float pos_share_0[2]= {63,  0}; // [unit: px] Position of the `ln_share` node bounding box
        float pos_share_1[2]= {63, 50};
        
        float ROI_attach[2] = {5, 5};   // [unit: px] Width and Height of the `in`, `out`, `add`, `share` boxes

        float fillet_radius = 0;

        std::vector<gNC::gLINK*> ln_in;     // type 0 //optional
        std::vector<gNC::gLINK*> ln_out;    // type 1 //optional
        std::vector<gNC::gLINK*> ln_add;    // type 2 //optional
        std::vector<gNC::gLINK*> ln_share;  // type 3 //optional
        gNODE(
            float par_posX, float par_posY,
            std::string par_label="",std::string par_desc="", std::string par_bodyText="",
            std::vector<gNC::gLINK*> par_ln_in=std::vector<gNC::gLINK*>(),  std::vector<gNC::gLINK*> par_ln_out=std::vector<gNC::gLINK*>(),
            std::vector<gNC::gLINK*> par_ln_add=std::vector<gNC::gLINK*>(), std::vector<gNC::gLINK*> par_ln_share=std::vector<gNC::gLINK*>(),
            int par_layout = 0,
            float par_width=160,    float par_height=140,
            float par_posX_in=0,    float par_posY_in=25,
            float par_posX_out=100, float par_posY_out=25,
            float par_posX_add=31,  float par_posY_add=0,
            float par_posX_share=63,float par_posY_share=0

        ): label{par_label}, desc{par_desc}, bodyText{par_bodyText}, ln_in{par_ln_in}, ln_out{par_ln_out}, ln_add{par_ln_add}, ln_share{par_ln_share} {
            if(checkExistence<int>(par_layout, std::vector<int>{0,1,2,3})==-1) std::runtime_error("ERROR: gNC::gNODE constructor: par_layout is an invalid value");
            layout = par_layout;
            width   = par_width;
            height  = par_height;
            pos[0]  = par_posX;
            pos[1]  = par_posY;
            pos_in[0]   = par_posX_in;  pos_in[1]   = par_posY_in;
            pos_out[0]  = par_posX_out; pos_out[1]  = par_posY_out;
            pos_add_0[0]    = par_posX_add;     pos_add_0[1]    = par_posY_add;
            pos_share_0[0]  = par_posX_share;   pos_share_0[1]  = par_posY_share;
            if(layout==0 || layout==1) { //define the opposing connection points positions
                pos_add_1[0]    = pos_add_0[0];
                pos_add_1[1]    = height*2-pos_add_0[1];
                pos_share_1[0]  = pos_share_0[0];
                pos_share_1[1]  = height*2-pos_share_0[1];
            }
            else if(layout==2 || layout==3) {
                pos_add_1[1]    = pos_add_0[1];
                pos_add_1[0]    = width*2-pos_add_0[0];
                pos_share_1[1]  = pos_share_0[1];
                pos_share_1[0]  = width*2-pos_share_0[0];
            }


        }

        void setPos(int x, int y) {
            pos[0] = x;
            pos[1] = y;
        }
        void setDim(int w, int h) {
            width   = w;
            height  = h;
        }

    };


    class guiNodeChart {
        private:
        std::string _info_name = "gNC::guiNodeChart::";

        static const gNC::gNODE default_gNODE;
        // static const float default_NODE_width   = 100;

        /**
         * Main storage for the different nodes. The nodes will be differentiated by their element
         * index or element address in the std::list container `_nodes`.
         */
        std::list<gNC::gNODE> _nodes;
        /**
         * Main storage for the different links between nodes. The links will be differentiated
         * by their element address in the std::list container `_links`
         */
        std::list<gNC::gLINK> _links;

        //Address of the last `gNC::gNODE` element in the list
        gNC::gNODE* _lastAddedNode = nullptr;
        //Address of the last `gNC::gLINK` element in the list
        gNC::gLINK* _lastAddedLink = nullptr;

        template<typename storedType>
        int _find_ptr_idx(const std::list<storedType>& toCheck, storedType* ptr_toFind);
        template<typename storedType>
        auto _find_ptr_itr(const std::list<storedType>& toCheck, storedType* ptr_toFind);

        template<typename storedType>
        int _vecfind_ptr_idx(const std::vector<storedType>& toCheck, storedType toFind);
        template<typename storedType>
        auto _vecfind_ptr_itr( std::vector<storedType>& toCheck, storedType toFind);


        public:
        int screen_pos[2] = {0, 0};
        int screen_pos_delta[2] = {0, 0};

        int screen_dim[2] = {1280, 720};


        guiNodeChart(/* args */);

        int setScreen_pos(int x, int y, int moveMode = 0);
        int setScreen_dim(int w, int h);


        size_t size(int whatList=0);

        std::list<gNC::gNODE> NODES();
        std::list<gNC::gLINK> LINKS();

        gNC::gNODE* lastAdded_NODE();
        gNC::gLINK* lastAdded_LINK();

        gNC::gNODE operator[](size_t i) const;


        gNC::gNODE* NODE_create(
            float pos_x,
            float pos_y,
            std::string label   = "",
            std::string desc    = "",
            std::string bodyText= "",
            float width = default_gNODE.width, //default_NODE_width,
            float height= default_gNODE.height
        );
        int NODE_delete(size_t NODE_idx, bool leaveFloating=false);
        int NODE_delete(gNC::gNODE* NODE_toDelete, bool leaveFloating=false);

        int NODE_move(gNC::gNODE* NODE_toMove, float new_X, float new_Y);


        gNC::gLINK* LINK_create(
            size_t NODE_src_idx,
            size_t NODE_dest_idx,
            int type_src,
            int type_dest,
            std::string label   = "",
            std::string desc    = ""
        );
        gNC::gLINK* LINK_create(
            gNC::gNODE* NODE_src,
            gNC::gNODE* NODE_dest,
            int type_src,
            int type_dest,
            std::string label   =  "",
            std::string desc    =  ""
        );
        int LINK_swapSrc(gNC::gLINK* toSwap, gNC::gNODE* newSrc, int srcType);
        int LINK_swapDest(gNC::gLINK* toSwap, gNC::gNODE* newDest, int destType);
        int LINK_delete(gNC::gLINK* LINK_toDelete);


        int draw();

        int save(
            std::string filename,
            bool overwrite = false
        );
    };
    
}


template<typename _varType> int checkExistence(_varType toFind, const std::vector<_varType>& toSearch) {
    for(size_t i=0; i<toSearch.size(); i++) {
        if(toSearch.at(i)==toFind) return static_cast<int>(i);
    }
    return -1;
}
template<typename _varType> int checkExistence(_varType toFind, const std::list<_varType>& toSearch) {
    int count=0;
    for(auto itr=toSearch.begin(); itr!=toSearch.end(); ++itr) {
        if(*itr==toFind) return count;

        count++;
    }

    return -1;
}
template<typename _varType> int checkExistence(_varType toFind, _varType toSearch[], int arrLen) {
    for(int i=0; i<arrLen; i++) {
        if(toSearch[i]==toFind) return i;
    }
    return -1;
}

// template<class T> int searchVec(std::vector<T> vec, T toFind);
/**
 * @brief Search and find the vector index position of a certain value
 * 
 * @tparam T -data type of elements to look through
 * @param vec vector to search through
 * @param toFind value to find in the vector
 * @return int index of where on `vec` the given `toFind` value exists.
 * @note if the value is not found in the vector then the function will return -1
 */
template<class T> int searchVec(std::vector<T> vec, T toFind) {
    typename std::vector<T>::iterator idx = find(vec.begin(), vec.end(), toFind);
    if(idx!=vec.end()) return idx-vec.begin();
    else return -1;
    // int idx = -1;
    // for(size_t i=0; i<vec.size(); i++) {
    // 	if(vec.at(i)==toFind) {
    //     	idx=i;
    //         break;
    //     }
    // }
    // return idx;
}


#endif