

#include "nodeChart.hpp"


namespace gNC {

    struct gNODE;

    struct gLINK {
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
        std::string label   = ""; //optional
        std::string desc    = ""; //optional
        std::string bodyText= ""; //optional

        /// @details GUI related
        float width = 100; /// [unit: px] width of the bounding box ROI
        float height= 50; /// [unit: px] height of the bounding box ROI

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
            float par_width=100, float par_height=50,
            float par_posX_in=0, float par_posY_in=25,
            float par_posX_out=100, float par_posY_out=25,
            float par_posX_add=31, float par_posX_share=63
        ): label{par_label}, desc{par_desc}, bodyText{par_bodyText}, ln_in{par_ln_in}, ln_out{par_ln_out}, ln_add{par_ln_add}, ln_share{par_ln_share} {
            width   = par_width;
            height  = par_height;
            pos[0]  = par_posX;
            pos[1]  = par_posY;
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

        //Address of the last `NC::NODE` element in the list
        gNC::gNODE* _lastAddedNode = nullptr;
        //Address of the last `NC::LINK` element in the list
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
        guiNodeChart(/* args */);

        size_t size(int whatList=0);

        gNC::gNODE* lastAdded_NODE();
        gNC::gLINK* lastAdded_LINK();

        gNC::gNODE operator[](size_t i) const;


        gNC::gNODE* NODE_add(
            float pos_x,
            float pos_y,
            float width = default_gNODE.width, //default_NODE_width,
            float height= default_gNODE.height,
            std::string label   = "",
            std::string desc    = "",
            std::string bodyText= ""
        );

    };
    
    


}
