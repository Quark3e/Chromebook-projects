

#include "nodeChart.hpp"


namespace gNC {

    struct gNODE;

    struct gLINK: NC::LINK {

    };

    struct gNODE: NC::NODE {
        float width; /// [unit: px] width of the bounding box ROI
        float height; /// [unit: px] height of the bounding box ROI

        /// [unit: px] Absolute(relative to screen) {x, y} coordinate of the top left corner of this NODE ROI.
        float pos[2];

        float pos_in[2];    // [unit: px] Position of the `ln_in` node bounding box
        float pos_out[2];   // [unit: px] Position of the `ln_out` node bounding box
        float pos_add[2];   // [unit: px] Position of the `ln_add` node bounding box
        float pos_share[2]; // [unit: px] Position of the `ln_share` node bounding box
        
        float ROI_attach[2];// [unit: px] Width and Height of the `in`, `out`, `add`, `share` boxes

        float fillet_radius = 0;

    };


    class guiNodeChart {
        private:
        /* data */
        public:
        guiNodeChart(/* args */);
    };
    
    


}
