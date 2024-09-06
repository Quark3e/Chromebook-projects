
#pragma once
#ifndef HPP_GUI_NODECHART
#define HPP_GUI_NODECHART


#include "globals_includes.hpp"



template<typename _varType> int checkExistence(_varType toFind, const std::vector<_varType>& toSearch);
template<typename _varType> int checkExistence(_varType toFind, const std::list<_varType>& toSearch);
template<typename _varType> int checkExistence(_varType toFind, _varType toSearch[], int arrLen);
template<typename addrType> std::string ptrToStr(addrType toConv);


namespace gNC {


    static auto to_ImVec2 = [](pos2d toConv) { return ImVec2(toConv.x, toConv.y); };
    static auto to_pos2d  = [](ImVec2 toConv){ return pos2d(toConv.x, toConv.y); };

    struct gNODE;
    // struct gLINK;

    // static auto add_nodePos = [](ImVec2 addTo, gNC::gNODE* toAdd) { return ImVec2(addTo.x+toAdd->pos[0], addTo.y+toAdd->pos[1]); };

    struct gLINK {
        std::string addr    = ""; // `std::string` of this `gNC::gLINK` instance/object address

        std::string label   = ""; //[optional]
        std::string desc    = ""; //[optional]

        /**
         * Integer code for source type: Whether it's "out" or "share":
         * - {`out`: `1`, `share_0`: `3`, `share_1`: `5`}
         */
        int type_src;
        /**
         * Integer code for dest type: WHether it's "in" our "add"
         * - {`in`: `0`, `add_0`: `2`, `add_1`: `4`}
         */
        int type_dest;

        gNC::gNODE* src;    // link source
        gNC::gNODE* dest;   // link destination


        /**
         * Different modes for the layout of the gNode:
         *  - `0` - horizontal: left->right
         *  - `1` - horizontal: right->left (text is left->right)
         *  - `2` - vertical:   top->down
         *  - `3` - vertical:   down->top
         */
        int layout = 0;

        float min__connect  = 50;
        float min__node     = 100;
        float lim__sameSide = 150;

        ImVec2 Pos_src;     // Absolute 2d coordinates of src connection: By defualt this is at the src node's out point
        ImVec2 Pos_dest;    // Absolute 2d coordinates of dest connection: By default this is at the dest node's in point
        // ImVec2 Pos_s1;      // coordinates of intermediary point 1: src side
        // ImVec2 Pos_d1;      // coordinates of intermediary point 1: dest side
        ImVec2 Pos_center;  // 2d coordinate of the point between Pos_src and Pos_dest

        /**
         * [0] - src
         * 
         * [.size()-1] - dest
         * 
         * make it avoid going through the node.
         * if x or y axis between two points are same then it's a straight line.
         */
        std::vector<ImVec2> link_points;


        gLINK(
            int par_type_src, int par_type_dest,
            gNC::gNODE* par_src, gNC::gNODE* par_dest,
            std::string par_label="",std::string par_desc=""
        ): Pos_src(-1,-1), Pos_dest(-1,-1)/*, Pos_s1(-1,-1), Pos_d1(-1,-1)*/ {
            if(searchVec<int>(std::vector<int>{1, 3, 5}, par_type_src)==-1 && searchVec<int>(std::vector<int>{0, 2, 4}, par_type_dest)==-1) std::runtime_error("ERROR: gNC::gLINK() constructor par_type_{..} is invalid");
            if(par_src==nullptr && par_dest==nullptr) std::runtime_error("ERROR: gNC::gLINK(): constructor: both `src` and `dest` can't be nullptr");

            this->type_src  = par_type_src;
            this->type_dest = par_type_dest;
            this->src  = par_src;
            this->dest = par_dest;
            this->label = par_label;
            this->desc  = par_desc;
        }
        void move_link(ImVec2 par_pos_src=ImVec2(-1, -1), ImVec2 par_pos_dest=ImVec2(-1, -1)/*, ImVec2 par_pos_s1=ImVec2(-1, -1), ImVec2 par_pos_d1=ImVec2(-1, -1)*/);
        void draw_link(std::vector<ImDrawList*> draw_win, ImVec2 screen_offset);
    };

    struct gNODE {
        std::string addr    = ""; // `std::string` of this `gNC::gNODE` instance/object address

        std::string label   = ""; //[optional]
        std::string desc    = ""; //[optional]
        std::string bodyText= ""; //[optional]

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
        ImVec2 pos;

        ImVec2 pos_in       = ImVec2{0,  25}; // [unit: px] Position of the `ln_in` node bounding box
        ImVec2 pos_out      = ImVec2{100, 25};// [unit: px] Position of the `ln_out` node bounding box
        ImVec2 pos_add_0    = ImVec2{31,  0}; // [unit: px] Position of the `ln_add` node bounding box 
        ImVec2 pos_add_1    = ImVec2{31, 50};
        ImVec2 pos_share_0  = ImVec2{63,  0}; // [unit: px] Position of the `ln_share` node bounding box
        ImVec2 pos_share_1  = ImVec2{63, 50};
        
        ImVec2 ROI_attach = ImVec2{14, 14};   // [unit: px] Width and Height of the `in`, `out`, `add`, `share` connect point bounding boxes


        float fillet_radius = 0;

        /**
         * Different states of the node connections. Mainly used in cosmetics/visuals.
         *  container index to node connection:
         *  - [0] - `in`
         *  - [1] - `out`
         *  - [2] - `add_0`
         *  - [3] - `share_0`
         *  - [4] - `add_1`
         *  - [5] - `share_1`
         * 
         * index integer value to state correlation:
         *  - `0` - off: defauult
         *  - `1` - off: hover
         *  - `2` - on : click
        */
        std::vector<int> state_connections = std::vector<int>{0, 0, 0, 0, 0, 0};


        std::vector<gNC::gLINK*> ln_in;     // type 0 //[optional]
        std::vector<gNC::gLINK*> ln_out;    // type 1 //[optional]
        std::vector<gNC::gLINK*> ln_add;    // type 2 //[optional]
        std::vector<gNC::gLINK*> ln_share;  // type 3 //[optional]

        gNODE(
            float par_posX, float par_posY,
            std::string par_label="",std::string par_desc="", std::string par_bodyText="",
            std::vector<gNC::gLINK*> par_ln_in=std::vector<gNC::gLINK*>(),  std::vector<gNC::gLINK*> par_ln_out=std::vector<gNC::gLINK*>(),
            std::vector<gNC::gLINK*> par_ln_add=std::vector<gNC::gLINK*>(), std::vector<gNC::gLINK*> par_ln_share=std::vector<gNC::gLINK*>(),
            int par_layout = 0,
            float par_width=160,    float par_height=140,
            float par_posX_in=0,    float par_posY_in=70,
            float par_posX_out=160, float par_posY_out=70,
            float par_posX_add=51,  float par_posY_add=0,
            float par_posX_share=110,float par_posY_share=0

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
            // std::cout<<pos_add_0[0]<< " "<<pos_add_0[1]<<" | " <<pos_share_0[0]<< " "<<pos_share_0[1] << " - ";
            if(layout==0 || layout==1) { //define the opposing connection points positions
                pos_add_1[0]    = pos_add_0[0];
                pos_add_1[1]    = height-pos_add_0[1];
                pos_share_1[0]  = pos_share_0[0];
                pos_share_1[1]  = height-pos_share_0[1];
            }
            else if(layout==2 || layout==3) {
                pos_add_1[1]    = pos_add_0[1];
                pos_add_1[0]    = width-pos_add_0[0];
                pos_share_1[1]  = pos_share_0[1];
                pos_share_1[0]  = width-pos_share_0[0];
            }
            // std::cout<<pos_add_1[0]<< " "<<pos_add_1[1]<<" | " <<pos_share_1[0]<< " "<<pos_share_1[1] << std::endl;

        }

        void setPos(int x, int y) {
            pos[0] = x;
            pos[1] = y;
        }
        void setDim(int w, int h) {
            width   = w;
            height  = h;
        }
        ImVec2 getConnectionPos(int connectionID);
        int getConnectionType(int typeDef);
        void draw_connection(std::vector<ImDrawList*> draw_win, ImVec2 nodePos);
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

        template<typename storedType> int _find_ptr_idx(const std::list<storedType>& toCheck, storedType* ptr_toFind);
        template<typename storedType> auto _find_ptr_itr(const std::list<storedType>& toCheck, storedType* ptr_toFind);

        template<typename storedType> int _vecfind_ptr_idx(const std::vector<storedType>& toCheck, storedType toFind);
        template<typename storedType> auto _vecfind_ptr_itr( std::vector<storedType>& toCheck, storedType toFind);

        void update_connect(gNC::gLINK* toCheck, ImVec2 srcPos=ImVec2(-1,-1), ImVec2 destPos=ImVec2(-1,-1));

        ImVec2 add_nodePos(ImVec2 addTo, gNC::gNODE* toAdd) { return ImVec2(addTo.x+toAdd->pos[0], addTo.y+toAdd->pos[1]); }

        public:
        gNC::guiNodeChart* thisPtr = nullptr;
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

        gNC::gNODE& operator[](size_t i) const;


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

        int NODE_move(gNC::gNODE* NODE_toMove, float new_X, float new_Y, int moveMode=0);


        gNC::gLINK* LINK_create(
            size_t NODE_src_idx,
            size_t NODE_dest_idx,
            int type_src,
            int type_dest,
            std::string label   = "",
            std::string desc    = "",
            ImVec2 pos_interm_src = ImVec2(-1, -1),
            ImVec2 pos_interm_dest= ImVec2(-1, -1)
        );
        gNC::gLINK* LINK_create(
            gNC::gNODE* NODE_src,
            gNC::gNODE* NODE_dest,
            int type_src,
            int type_dest,
            std::string label   =  "",
            std::string desc    =  "",
            ImVec2 pos_interm_src = ImVec2(-1, -1),
            ImVec2 pos_interm_dest= ImVec2(-1, -1)
        );
        gNC::gLINK* LINK_create_loose(
            ImVec2 loosePos,
            gNC::gNODE* _NODE,
            int type_NODE_connection,
            std::string label   = "",
            std::string desc    = "",
            ImVec2 pos_interm_NODE = ImVec2(-1, -1)
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


    /**
     * "Static"/Global value for displaying menu: node_details
     *  - `nullptr`: dont display node details menu
     *  - `{addr}` : display that address
     */
    extern gNODE* nodePtr_menu__node_details;


    void _menu__node_details(gNC::gNODE* toDetail);
    void _menu__rightClick(guiNodeChart* chart, gNODE* _node = nullptr);
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




#endif