
#include "graph_selections.hpp"
#include "variables.hpp"
#include "useful.hpp"

DIY::typed_dict<std::string, void (*)()> graph_selections({
    {"Default", graph__default},
    {"Total time", graph__totalTime},
    {"processTime", graph__processTime},
    {"iterationDelays", graph__iterationDelays}
});


size_t selected__graph_selections = 0;

void graph__default() {
    static bool init = true;
    // std::vector<float> 

    /**
     * [0] thread
     * [0][0] delay type
     */
    static std::vector<std::vector<float>> iterSum;
    static std::vector<unsigned> plotColours{
        IM_COL32(234, 245,  78, 255*0.5),
        IM_COL32(  4,  79,  13, 255*0.5),
        IM_COL32( 91,  50,  50, 255*0.5),
        IM_COL32(106, 191, 251, 255*0.5),
        IM_COL32(144, 192, 100, 255*0.5),
        IM_COL32(  0, 169, 113, 255*0.5),
        IM_COL32(  0,  70, 169, 255*0.5),
        IM_COL32(169,   0,   0, 255*0.5)
    };

    std::vector<double> values_x;
    
    // if(init) {

        iterSum = std::vector<std::vector<float>>(Loaded_Data__perfDelays.size(), std::vector<float>(Loaded_Data__perfDelays[0].size(), 0));
        if(iterSum[0].size() != plotColours.size()) throw std::runtime_error("ERROR: plotColours not same len.");
        for(size_t n=0; n<Loaded_Data__perfDelays.size(); n++) {
            values_x.push_back(Loaded_Data__perfDelays.getKey(n));
            for(size_t i=0; i<Loaded_Data__perfDelays[n].size(); i++) {
                for(size_t itr=0; itr<Loaded_Data__perfDelays[n][i].size(); itr++) {
                    iterSum[n][i] += Loaded_Data__perfDelays[n][i][itr];
                }
                iterSum[n][i] /= Loaded_Data__perfDelays[n][i].size();
            }
            if(n==0) std::cout << formatVector(iterSum[n]) << std::endl;
        }

        std::vector<float> yMaxValues;
        for(size_t i=0; i<iterSum.size(); i++) {
            yMaxValues.push_back(*std::max_element(iterSum[i].begin(), iterSum[i].end()));
        }
        float maxValue = *std::max_element(yMaxValues.begin(), yMaxValues.end());
        // std::cout << maxValue << std::endl;
        // std::cout << formatVector(iterSum[0]) << std::endl;
        graphObj.setRange_x(values_x[0], values_x[values_x.size()-1]);
        graphObj.setRange_y(0, maxValue/1000);
        graphObj.setSpacing_x(1);
        graphObj.setSpacing_y(0.5);

    // }
        // graphObj.plotData({0, 3}, {0, 1});

    for(size_t i=0; i<iterSum[0].size(); i++) {
        std::vector<double> values_y;
        for(size_t n=0; n<iterSum.size(); n++) {
            if(i<iterSum[0].size()-2) values_y.push_back(iterSum[n][i]);
            else values_y.push_back(iterSum[n][i]/1000.0);
        }
        // if(i==0) std::cout << formatVector(values_y) << std::endl;
        graphObj.plotData(values_x, values_y, plotType_line, plotColours[i]);
    }
    
    init = false;
}
void graph__totalTime() {

}
void graph__processTime() {

}
void graph__iterationDelays() {

}
