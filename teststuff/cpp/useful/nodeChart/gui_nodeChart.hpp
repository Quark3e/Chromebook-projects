
#pragma once
#ifndef HPP_GUI_NODECHART
#define HPP_GUI_NODECHART


#include "globals_includes.hpp"


// extern template<typename _varType> int checkExistence(_varType toFind, const std::vector<_varType>& toSearch);
// extern template<typename _varType> int checkExistence(_varType toFind, const std::list<_varType>& toSearch);
// extern template<typename _varType> int checkExistence(_varType toFind, _varType toSearch[], int arrLen);
// extern template<typename addrType> std::string ptrToStr(addrType toConv);


namespace gNC {


    static auto to_ImVec2 = [](pos2d toConv) { return ImVec2(toConv.x, toConv.y); };
    static auto to_pos2d  = [](ImVec2 toConv){ return pos2d(toConv.x, toConv.y); };

    class guiNodeChart;
    struct gNODE;
    // struct gLINK;

    struct timeline;

    // static auto add_nodePos = [](ImVec2 addTo, gNC::gNODE* toAdd) { return ImVec2(addTo.x+toAdd->pos[0], addTo.y+toAdd->pos[1]); };

    struct gLINK {
        guiNodeChart* inChart;
        bool _init = false;
        std::string addr    = ""; // `std::string` of this `gNC::gLINK` instance/object address

        std::string label   = ""; //[optional]
        std::string desc    = ""; //[optional]
        std::string bodyText= ""; //[optional]


        /**
         *   int tm_sec;			 Seconds.	[0-60] (1 leap second) 
         *   int tm_min;			 Minutes.	[0-59] 
         *   int tm_hour;			 Hours.	[0-23] 
         *   int tm_mday;			 Day.		[1-31] 
         *   int tm_mon;			 Month.	[0-11] 
         *   int tm_year;			 Year	- 1900.
         *   int tm_wday;			 Day of week.	[0-6]
         *   int tm_yday;			 Days in year.[0-365]
         *   int tm_isdst;			 DST.		[-1/0/1]
         * 
         */
        tm date{0, 0, 0, 1, 0, 2000-1900, 0, 0, 0}; //[optional]


        /**
         * Integer code for source type: Whether it's "out" or "share":
         * - {`out`: `1`, `share_0`: `3`, `share_1`: `5`}
         */
        int type_src    = -1;
        /**
         * Integer code for dest type: WHether it's "in" our "add"
         * - {`in`: `0`, `add_0`: `2`, `add_1`: `4`}
         */
        int type_dest   = -0;

        gNC::gNODE* src = nullptr;  // link source
        gNC::gNODE* dest= nullptr;  // link destination


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
        float _gui__bezier_min  = 10;

        ImVec2 Pos_src;     // Absolute 2d coordinates of src connection: By defualt this is at the src node's out point
        ImVec2 Pos_dest;    // Absolute 2d coordinates of dest connection: By default this is at the dest node's in point
        // ImVec2 Pos_s1;      // coordinates of intermediary point 1: src side
        // ImVec2 Pos_d1;      // coordinates of intermediary point 1: dest side
        ImVec2 Pos_center;  // 2d coordinate of the point between Pos_src and Pos_dest

        int link_lineWidth = 12;

        int link_gui__lineWidth = 5;

        /**
         * 
         *  - `0` - off: default
         *  - `1` - off: hover
         *  - `2` - on : click/press
         */
        int draw__state = 0;

        /**
         * Container for all the points that form the link as drawn
         *
         * [0] - src
         * [.size()-1] - dest
         * 
         * make it avoid going through the node.
         * if x or y axis between two points are same then it's a straight line.
         */
        std::vector<ImVec2> link_points;

        /**
         * Container for **all** the points that form the link, including the points
         * that form the bezier curve(s)
         * 
         */
        std::vector<ImVec2> link_points_raw;
        bool link_points_raw__updated = false;

        // /**
        //  * Container of container for the polynomial coefficients (linear) for each line that forms `link_points_raw`:
        //  * format: {
        //  *     {`a0`, `a1`},
        //  *     {`a0`, `a1`}
        //  * }
        //  */
        // std::vector<std::vector<float>> link_points_coeffs;

        /**
         * Container with the xy limits:
         * lims = {
         *     {x_min, x_max},
         *     {y_min, y_max}
         * }
         * 
         * lims[0] = {x_min, x_max},
         * lims[1] = {y_min, y_max}
         */
        std::vector<std::vector<float>> link_lims;

        gLINK();
        /**
         * @brief Construct a new gLINK object
         * 
         * @param par_type_src type of src connection: {`out`: `1`, `share0`: `3`, `share1`: `5`}
         * @param par_type_dest type of dest conneciton: {`in`: `0`, `add0`: `2`, `add1`: `4`}
         * @param par_src `gNC::gNODE*` to src node
         * @param par_dest `gNC::gNODE*` to dest node
         * @param par_label `std::string` of link label
         * @param par_desc `std::string` of link desc
         */
        gLINK(
            int par_type_src, int par_type_dest,
            gNC::gNODE* par_src, gNC::gNODE* par_dest,
            std::string par_label="",std::string par_desc="", std::string par_bodyText=""
        );
        /**
         * @brief Default constructor for gNC::gLINK
         */
        void move_link(ImVec2 par_pos_src=ImVec2(-2, -2), ImVec2 par_pos_dest=ImVec2(-2, -2)/*, ImVec2 par_pos_s1=ImVec2(-1, -1), ImVec2 par_pos_d1=ImVec2(-1, -1)*/);
        void draw_link(std::vector<ImDrawList*> draw_win, ImVec2 screen_offset);
        bool region(ImVec2 cursor, ImVec2 _offset = ImVec2(0, 0));
    };

    struct gNODE {
        gNC::guiNodeChart* inChart;
        std::string addr    = ""; // `std::string` of this `gNC::gNODE` instance/object address

        std::string label   = ""; //[optional]
        std::string desc    = ""; //[optional]
        std::string bodyText= ""; //[optional]

        /**
         *   int tm_sec;			 Seconds.	[0-60] (1 leap second) 
         *   int tm_min;			 Minutes.	[0-59] 
         *   int tm_hour;			 Hours.	[0-23] 
         *   int tm_mday;			 Day.		[1-31] 
         *   int tm_mon;			 Month.	[0-11] 
         *   int tm_year;			 Year	- 1900.
         *   int tm_wday;			 Day of week.	[0-6]
         *   int tm_yday;			 Days in year.[0-365]
         *   int tm_isdst;			 DST.		[-1/0/1]
         * 
         */
        tm date; //[optional]

        bool init = false;

        /**
         * Different modes for the layout of the gNode:
         *  - `0` - horizontal: left->right
         *  - `1` - horizontal: right->left (text is left->right)
         *  - `2` - vertical:   top->down
         *  - `3` - vertical:   down->top
         */
        int layout = 0;

        /// @details GUI related
        float width = 160; /// [unit: px] width of the bounding box ROI
        float height= 140; /// [unit: px] height of the bounding box ROI


        /// [unit: px] Absolute(relative to screen) {x, y} coordinate of the top left corner of this NODE ROI.
        ImVec2 pos;

        ImVec2 pos_in       = ImVec2{  0, 70}; // [unit: px] Position of the `ln_in` node bounding box
        ImVec2 pos_out      = ImVec2{160, 70};// [unit: px] Position of the `ln_out` node bounding box
        ImVec2 pos_add_0    = ImVec2{ 51,  0}; // [unit: px] Position of the `ln_add` node bounding box 
        ImVec2 pos_add_1    = ImVec2{ 51,140};
        ImVec2 pos_share_0  = ImVec2{110,  0}; // [unit: px] Position of the `ln_share` node bounding box
        ImVec2 pos_share_1  = ImVec2{110,140};
        
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
         *  - `0` - off: default
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
        );

        void setPos(int x, int y);
        void setDim(int w, int h);
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
        bool modified = true;
        std::string project_name = "";

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
        gNC::gNODE& last();


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
            std::string bodyText= "",
            ImVec2 pos_interm_src = ImVec2(-1, -1),
            ImVec2 pos_interm_dest= ImVec2(-1, -1)
        );
        gNC::gLINK* LINK_create(
            gNC::gNODE* NODE_src,
            gNC::gNODE* NODE_dest,
            int type_src,
            int type_dest,
            std::string label   = "",
            std::string desc    = "",
            std::string bodyText= "",
            ImVec2 pos_interm_src = ImVec2(-1, -1),
            ImVec2 pos_interm_dest= ImVec2(-1, -1)
        );
        gNC::gLINK* LINK_create_loose(
            ImVec2 loosePos,
            gNC::gNODE* _NODE,
            int type_NODE_connection,
            std::string label   = "",
            std::string desc    = "",
            std::string bodyText= "",
            ImVec2 pos_interm_NODE = ImVec2(-1, -1)
        );
        int LINK_swapSrc(gNC::gLINK* toSwap, gNC::gNODE* newSrc, int srcType);
        int LINK_swapDest(gNC::gLINK* toSwap, gNC::gNODE* newDest, int destType);
        int LINK_delete(gNC::gLINK* LINK_toDelete);


        int draw();

        int saveToFile(
            std::string filename,
            bool overwrite = false
        );
        /**
         * @brief Load nodeChart from .json saveFile into this class object
         * 
         * @param filename `std::string` of the the json file path, preferably absolute, or relative to cwd
         * @param verbose whether to print actions on terminal
         * @return int 
         */
        int loadFile(
            std::string filename,
            bool verbose = false
        );
    };

    /**
     * Arbitrary time unit used by gNC::timeObject and gNC::timeline to sort and indicate the order of each timeObject
     * 
     */
    struct timeUnit {
        size_t value;

        timeUnit(size_t _value);
        timeUnit(const timeUnit &_obj);

        operator int()      const { return static_cast<int>(value); }
        operator size_t()   const { return value; }
        
        timeUnit operator+=(timeUnit const& _obj);
        timeUnit operator-=(timeUnit const& _obj);
        timeUnit operator+(timeUnit const& _obj);
        timeUnit operator-(timeUnit const& _obj);
        timeUnit operator*(timeUnit const& _obj);
        timeUnit operator/(timeUnit const& _obj);
        bool    operator==(timeUnit const& _obj);
        bool    operator!=(timeUnit const& _obj);
        bool    operator>=(timeUnit const& _obj);
        bool    operator<=(timeUnit const& _obj);
        bool    operator<(timeUnit const& _obj);
        bool    operator>(timeUnit const& _obj);
        
    };
    /**
     * Time object representing a `gNC::gNODE` in a timeline
     * 
     */
    struct timeObject {
        timeUnit    start;
        timeUnit    end;
        gNODE*      objNode;
        size_t      channel;

        /**
         * @brief Whether a timeUnit value is within the start-end of this timeObject
         * 
         * @param _value the value to check
         * @return int code for whether it is not within [`0`], is within but closer to start [`1`], is in the middle [`2`] or
         * within and closer to end [`3`].
         */
        int is_within(timeUnit _value);
        /**
         * @brief Whether the input gNC::timeUnit is a side of this time object
         * 
         * @param _value the gNC::timeUnit value to check if it's a side or not
         * @param _margin the margin of error for which the checking is allowed to be wrong (so if `_margin==2`, `_value==120`, and a side is `119`, it'll return correct)
         * @return int code of whether the value is a side:
         *  - `0` no side found
         *  - `1` _start side
         *  - `2` _end side
         */
        int is_side(timeUnit _value, size_t _margin=0);
        /**
         * @brief move the start timeUnit
         * 
         * @param _newStart new value to set as start
         * @param _min_width minimum start-end width of the timeObject allowed (so the width doesnt become smaller than this number)
         * @param _only_check whether not change and to just check
         * @return int code: `0`-successful, otherwise failure
         */
        int move_start(timeUnit _newStart, size_t _min_width=1, bool _only_check=false);
        /**
         * @brief move the start timeUnit
         * 
         * @param _newEnd new value to set as start
         * @param _min_width minimum start-end width of the timeObject allowed (so the width doesnt become smaller than this number)
         * @param _only_check whether not change and to just check
         * @return int code: `0`-successful, otherwise failure
         */
        int move_end(timeUnit _newEnd, size_t _min_width=1, bool _only_check=false);

    };

    class timeline {
    private:
        std::string _info_name = "gNC::timeline";
        /// @brief Main timeObject container
        std::vector<timeObject> _objects;

        /**
         * @brief resolve conflicts with merge method
         * 
         * @param _old reference to the old/already-existing timeObject.timeUnit
         * @param _new reference to the new timeObject.timeUnit
         * @param _new_loc_before boolean for whether the new timeObject is located ahead of the old timeObject
         * @param _conflictMergeMethod dedicated conflict merge method
         * @return int for whether it was successful or not: `0`-successful.
         */
        int _conflict_resolver(
            gNC::timeUnit& _old,
            gNC::timeUnit& _new,
            bool _new_loc_before,
            int _conflictMergeMethod
        );
        /**
         * @brief Find this->object insert position based on the new timeObject's _start, _end and _channel value
         * 
         * @param _start new object's start timeUnit
         * @param _end new object's end timeUnit
         * @param _channel new object's _channel
         * @param _ins_indices reference to elements where the new timeObject is to be place within, [0]-minimum, [1]-maximum.
         * If the element==`-1` then none have been found for that "side"/"end":
         *  so for example, if the elements by the end are:
         *  `{-1, 0}`   - is the smallest. Insert before element [0]: `.insert(itr(0))`
         *  `{ 2, 3}`   - is between elements [2] and [3]: `.insert(itr(3))` (because inserting at index 3 pushes the existing element at 3 to 4)
         *  `{ 4, -1}`  - is after element [4], is the last: `.push_back()`
         *  `{-1, -1}`  - no timeObject has been found to be compared to so just append this at the end of objects vector-
         * @param _ins_conflicts reference to booleans for whether a conflict has been found at either side ([0]-_start, [1]-_end) of the placement search.
         * @param _vec pointer to the `std::vector<timeObject>` container to use as the timeline timeObject container
         * @return int for return code. `0`-success, otherwise an error has occurred.
         */
        int _find_insert_pos(
            gNC::timeUnit   _start,
            gNC::timeUnit   _end,
            size_t          _channel,
            std::vector<int>&   _ins_indices,
            std::vector<bool>&  _ins_conflicts,
            std::vector<timeObject>* _vec=nullptr
        );

        /**
         * Number of channels within a timeline that's allowed to exist.
         * If value is `0` then no limit is set.
         */
        size_t _channel_limit = 1;
    public:
        /// @brief copy version of the main _objects std::vector used for visualising temporary changes. Place this in gNC::timeline method args if changes are to be temporary
        std::vector<timeObject> forVisuals;

        void update_forVisuals();

        timeline();
        /**
         * @brief Set the channel limit of the timline
         * 
         * @param _val the new channel limit
         * @param _vec pointer to the `std::vector<timeObject>` container to use as the timeline timeObject container
         * @warning if any timeObjects are located at an erased channel then it'll delete all those objects.
         */
        void set_channel_limit(size_t _val, std::vector<timeObject>* _vec=nullptr);
        /**
         * add a new timeObject to the timeline for a node
         * 
         * @param _nodePtr pointer to the node to add to as the timeObject
         * @param _start timeUnit for the timeObject start
         * @param _end timeUnit for the timeObject end. Cannot be the same as `_start`. If it's the same or less than `_start` arg
         * @param _channel what timeline channel to place the timeObject in: 
         * if `0`-whichever with smallest conflict;
         * if `>=channel_limit`-same as 0
         * @param _conflictMergeMethod integer flag/code for how to resolve any timeUnit conflicts that might arise where this timeObject is placed.
         *  If the new object's start-end is placed between two other objects and their end-start distance is smaller than the width(start-end) of this object
         * it'll count as a conflict, otherwise it'll automatically move/shift this objects start/end so it fits.
         * `0` - prio new object, crop everything else in conflict;
         * `1` - prio old objects, crop the new object
         * `2` - prio object located former, crop objects after
         * `3` - prio object located latter, crop objects former.
         * `4` - prio neither side and crop both objects to the midle ground.
         * @param _vec pointer to the `std::vector<timeObject>` container to use as the timeline timeObject container
         * @return int for whether the function successfully ran or any errors happened: `0`-success
         * @note if the _start and _end range of ths new timeObject "envelopes"/surrounds another existing pre-existing timeObject then the function will return
         * as an error since removing/replacing existing timeObject's must be done with another function
         */
        int add_timeObject(gNC::gNODE *_nodePtr, gNC::timeUnit _start, gNC::timeUnit _end, size_t _channel=0, int _conflictMergeMethod=0, std::vector<timeObject>* _vec=nullptr);

        /**
         * @brief move a timeObject in the timeline
         * 
         * @param _nodePtr pointer to the `gNC::gNODE` assosciated with the timeObject
         * @param _start new start timeUnit
         * @param _end new end timeUnit
         * @param _channel channel for the new placement
         * @param _conflictMergeMethod method of resolving placement conflicts
         * @param _vec pointer to the `std::vector<timeObject>` container to use as the timeline timeObject container. If this is set to `nullptr` then this function will do an "imaginary" move for visual purposes with an internal static timeline vector.
         * @return int code for whether this function call was successful [`0`] or an error has occurred [`!=0`]
         */
        int move_timeObject(gNC::gNODE *_nodePtr, gNC::timeUnit _start, gNC::timeUnit _end, size_t _channel, int _conflictMergeMethod=0, std::vector<timeObject>* _vec=nullptr);

        /**
         * @brief Delete timeObject from timeline
         * 
         * @param _nodePtr pointer to the node that is stored in timeline as the timeObject
         * @param _vec pointer to the `std::vector<timeObject>` container to use as the timeline timeObject container
         * @return int `0`-if successful, `-1`-pointer doesnt exist
         */
        int delete_timeObject(gNC::gNODE* _nodePtr, std::vector<timeObject>* _vec=nullptr);
        /**
         * @brief Get a copy of the timeObject stored in timeline
         * 
         * @param _nodePtr pointer to the timeObject that's representing this gNODE pointer.
         * @param _vec pointer to the `std::vector<timeObject>` container to use as the timeline timeObject container
         * @return timeObject value copy of the stored timeObject. If a timeObject couldn't be found then it'll throw invalid_argument error.
         */
        timeObject get_timeObject(gNC::gNODE* _nodePtr, std::vector<timeObject>* _vec=nullptr);
        /**
         * @brief Get a copy of the timeObject stored in timeline
         * 
         * @param _findAt gNC::timeUnit timeline position for where to find a timeObject at.
         * @param _channel channel for where to find the timeObject. If `0` then it'll search in every channel.
         * @param _vec pointer to the `std::vector<timeObject>` container to use as the timeline timeObject container
         * @return timeObject value copy of the stored timeObject. If a timeObject couldn't be found then it'll throw invalid_argument error.
         */
        timeObject get_timeObject(timeUnit _findAt, size_t _channel=0, std::vector<timeObject>* _vec=nullptr);
        /**
         * @brief Move the side(s)/end(s) to a new timeUnit value
         * 
         * @param _oldVal the old value which is where the side is located.
         * @param _newVal the new value to move the side(s) to.
         * @param _toMove pointer to the gNODE timeObject who's side is to be moved to the new value.
         * if this is set to `nullptr` then it'll move all(/both) sides that are placed at _oldVal to _newVal.
         * @param _channel size_t value of the channel to make the move in. If==`0` then search through every channel.
         * If the given arg vlaue is bigger than the set limit it'll instead search the "highest channel" (the one with biggest number)
         * @param _vec pointer to the `std::vector<timeObject>` container to use as the timeline timeObject container
         * @return int for the return code. `0`-success, `-1`-no timeObject's side is located at given _oldVal
         */
        int move_sides(timeUnit _oldVal, timeUnit _newVal, gNC::gNODE* _toMove=nullptr, size_t _channel=0, std::vector<timeObject>* _vec=nullptr);
    };


    void reset_Ptr_menus();

    /**
     * "Static"/"Global" value/address for displaying menu: node_details
     *  - `nullptr`: dont display node details menu
     *  - `{addr}` : display that address
     */
    extern gNODE* nodePtr_menu__node_details;
    /**
     * "Global" address for `gNC::gLINK` object to call for right Click link menu
     * 
     */
    extern gNODE* nodePtr_menu__rightClick;

    /**
     * Address to the start node which a dragConnectCreate action has been started
     */
    extern gNODE* nodePtr__dragConnectCreate_start;


    extern gLINK* linkPtr_menu__link_details;

    extern gLINK* linkPtr_menu__rightClick;

    /**
     * Temporary intermediate link object for creating links with drag and connect/drop.
     * Link end nodes `src` and `dest` are set to `nullptr` and type_{`src`/`dest`} are set to -1.
     * No member functions can/should be called with this link.
     * When a drag and connect action has started at the start node, the member variable `_init` will be set to `true`.
     */
    extern gLINK dragConnectCreate_tempLink;

    /**
     * `dragConnectCreate_tempLink` link formation option:
     * "`Whether a link creation should be snapped into creation without the user having to specifically drop the link
     * at the correct connection point, but instead only have to drop it on the node and it'll auto snap to place`"
     * `0` - `false`: do not use this feature. The user has to drop the link correctly
     * `1` - `true` : do use this feature, and have it snap in on either `in` or `out`
     * `>1`- `true` : do use this feature, and have it snap in on either `add` or `share`
     */
    extern int option_dragConnectCreate_tempLink_snapIn;
    /**
     * Which end was first created during a dragConnectCreate action start
     * `0` - dest
     * `1` - src
     */
    extern int dragConnectCreate_startedEnd;

    void _menu__node_details(gNC::gNODE* toDetail);
    void _menu__link_details(gNC::gLINK* toDetail);
    void _menu__rightClick(gNC::guiNodeChart* _chart);
    void _menu__timeline(gNC::guiNodeChart* _chart);

    extern std::string _file__fileExplorer;
    /**
     * file explorer menu button labels.
     */
    extern std::vector<std::string> _mode__types;
    extern int         _mode__fileExplorer;        //`1`-open, `2`-save, `3`-load, `4`-save as
    extern int         _mode__fileExplorer_prev;
    /// @brief valid extensions for the file explorer to open
    extern std::vector<std::string> _valid__extensions;
    void        _menu__fileExplorer();

    extern bool _winFocused__node_details;
    extern bool _winFocused__link_details;
}



#endif