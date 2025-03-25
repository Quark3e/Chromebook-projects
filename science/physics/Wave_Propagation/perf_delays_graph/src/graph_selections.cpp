
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

bool selected_init = true;

void graph__default() {
    static bool init = true;

    /**
     * [0] thread
     * [0][0] delay type
     */
    static std::vector<std::vector<float>> iterSum;
    static std::vector<unsigned> plotColours{
        IM_COL32(204, 215,  78, 255*0.9),
        IM_COL32(  4,  79,  13, 255*0.9),
        IM_COL32( 91,  50,  50, 255*0.9),
        IM_COL32(106, 191, 251, 255*0.9),
        IM_COL32(144, 192, 100, 255*0.9),
        IM_COL32(  0, 169, 113, 255*0.9),
        IM_COL32(  0,  70, 169, 255*0.9),
        IM_COL32(169,   0,   0, 255*0.9)
    };

    static std::vector<double> values_x;
    ImDrawList* graphDrawList = ImGui::GetWindowDrawList();

    if(init) {
        selected_init = true;
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
        }
    }
    if(selected_init) {

        std::vector<float> yMaxValues;
        for(size_t i=0; i<iterSum.size(); i++) {
            yMaxValues.push_back(*std::max_element(iterSum[i].begin(), iterSum[i].end()));
        }
        float maxValue = *std::max_element(yMaxValues.begin(), yMaxValues.end());
        graphObj.setRange_x(values_x[0], values_x[values_x.size()-1]);
        graphObj.setRange_y(0, maxValue/1000);
        graphObj.setSpacing_x(1);
        graphObj.setSpacing_y(0.5);

    }

    for(size_t i=0; i<iterSum[0].size(); i++) {
        std::vector<double> values_y;
        for(size_t n=0; n<iterSum.size(); n++) {
            if(i<iterSum[0].size()-2) values_y.push_back(iterSum[n][i]);
            else values_y.push_back(iterSum[n][i]/1000.0);
        }
        graphObj.plotData(values_x, values_y, plotType_line, plotColours[i]);
    }

    ImVec2 graphWinPos = ImGui::GetWindowPos();
    
    ImGui::SetNextWindowPos(ImVec2(graphWinPos.x+20, graphWinPos.y+20));
    ImGui::SetNextWindowSize(ImVec2(250, 170));
    ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(255, 255, 255, 200));
    if(ImGui::Begin("graph legend", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoTitleBar)) {
        ImVec2 legendWinPos = ImGui::GetWindowPos();
        ImGui::SetCursorPosY(12);
        for(size_t i=0; i<iterSum[0].size(); i++) {
            ImVec2 itemPos = ImVec2(legendWinPos.x+5, legendWinPos.y+ImGui::GetTextLineHeightWithSpacing()*i+20);
            graphDrawList->AddLine(itemPos, ImVec2_add(itemPos, ImVec2(20, 0)), plotColours[i]/*IM_COL32(120, 120, 120, 200)*/, 7);
            graphDrawList->AddLine(itemPos, ImVec2_add(itemPos, ImVec2(20, 0)), plotColours[i], 5);
            ImGui::SetCursorPosX(30);
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(100, 100, 100, 250));
            ImGui::Text(Loaded_Data__perfDelays[0].getKey(i).c_str());
            ImGui::PopStyleColor();
        }
    }
    ImGui::PopStyleColor();
    ImGui::End();

    selected_init = false;
    init = false;
}
void graph__totalTime() {
    static bool init = true;


    static std::vector<double> values_x;
    static std::vector<double> values_y;

    static pos2d<size_t> nbRounding(2, 2);
    /**
     * [0] thread
     * [0][0] iteration
     */
    static std::vector<std::vector<double>> threadSum;

    static std::vector<std::vector<int>> plotColours;
    if(init) {
        pos2d<float> HSV_range(0, 180);
        float HSV_spacing = HSV_range.delta()/(Loaded_Data__perfDelays.size());
        for(size_t i=0; i<Loaded_Data__perfDelays.size(); i++) {
            std::vector<float> HSV_col{HSV_range[0], 80, 99};
            std::vector<float> rgbConv = convert_HSV_RGB(HSV_col);
            plotColours.push_back(std::vector<int>{int(rgbConv[0]), int(rgbConv[1]), int(rgbConv[2]), 255});
            HSV_range[0] += HSV_spacing;
        }
    }

    ImDrawList* graphDrawList = ImGui::GetWindowDrawList();

    static size_t dataItemLoc;
    static size_t numIter;
    static pos2d<double> graphLim_y;
    

    if(init) {
        selected_init = true;

        dataItemLoc = Loaded_Data__perfDelays[0].size()-1;
        numIter = Loaded_Data__perfDelays[0][dataItemLoc].size();
        graphLim_y = pos2d<double>(Loaded_Data__perfDelays[0][dataItemLoc][0]/1000, Loaded_Data__perfDelays[0][dataItemLoc][0]/1000);

        for(size_t th=0; th<Loaded_Data__perfDelays.size(); th++) {
            threadSum.push_back(std::vector<double>(numIter, 0));
            std::vector<float>& dictArrRef = Loaded_Data__perfDelays[th][dataItemLoc];
            for(size_t i=0; i<numIter; i++) {
                if((nbRounding[0]+nbRounding[1])>0) {
                    // pos2d<size_t> nbLim(
                    //     (nbRounding[0]>i? 0 : i-nbRounding[0]),
                    //     (nbRounding[1]+i>=numIter? numIter : i+nbRounding[1])
                    // );
                    // std::vector<size_t> nbRoundRange(nbRounding[0]+nbRounding[1]+1);
                    double nbRoundSum = 0;
                    size_t numSums = 0;
                    for(size_t ii=(i<nbRounding[0]? 0 : i-nbRounding[0]); ii<((nbRounding[1]+i)>=numIter? numIter : nbRounding[1]+i); ii++) {
                        // nbRoundRange[ii+nbLim[0]] = dictArrRef[ii]/1000;
                        nbRoundSum+=dictArrRef[ii]/1000;
                        numSums++;
                    }
                    // std::cout << numSums << " | " << nbRoundSum << std::endl;
                    threadSum[th][i] = nbRoundSum/double(numSums);
                }
                else {
                    threadSum[th][i] = dictArrRef[i]/1000;
                }
                
                if(threadSum[th][i]>graphLim_y[1]) {
                    graphLim_y[1] = threadSum[th][i];
                }
                else if(threadSum[th][i]<graphLim_y[0]) {
                    graphLim_y[0] = threadSum[th][i];
                }
            }
        }
        for(size_t i=0; i<numIter; i++) {
            values_x.push_back(i+1);
        }
    }
    if(selected_init) {
        
        std::vector<double> maxValues;
        std::vector<double> minValues;
        for(size_t n=0; n<Loaded_Data__perfDelays.size(); n++) {
            maxValues.push_back(*std::max_element(threadSum[n].begin(), threadSum[n].end()));
            minValues.push_back(*std::min_element(threadSum[n].begin(), threadSum[n].end()));
        }
        graphLim_y[0] = *std::min_element(minValues.begin(), minValues.end());
        graphLim_y[1] = *std::max_element(maxValues.begin(), maxValues.end());

        graphObj.setRange_x(1, numIter);
        graphObj.setRange_y(graphLim_y[0]*0.99, graphLim_y[1]*1.01);
        graphObj.setSpacing_x(10);
        graphObj.setSpacing_y((graphLim_y[1]-graphLim_y[0])*0.1);
    }

    for(size_t th=0; th<threadSum.size(); th++) {
        graphObj.plotData(values_x, threadSum[th], plotType_line, IM_COL32(plotColours[th][0], plotColours[th][1], plotColours[th][2], plotColours[th][3]));
    }

    ImVec2 graphWinPos = ImGui::GetWindowPos();

    ImGui::SetNextWindowPos(ImVec2(graphWinPos.x+dim__graphWindow.x-20, graphWinPos.y+20), 0, ImVec2(1, 0));
    ImGui::SetNextWindowSize(ImVec2(50, ImGui::GetTextLineHeightWithSpacing()*Loaded_Data__perfDelays.size()+20));
    ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(255, 255, 255, 200));
    if(ImGui::Begin("graph legend", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoTitleBar)) {
        ImVec2 legendWinPos = ImGui::GetWindowPos();
        ImGui::SetCursorPosY(12);
        for(size_t i=0; i<Loaded_Data__perfDelays.size(); i++) {
            ImVec2 itemPos = ImVec2(legendWinPos.x+5, legendWinPos.y+ImGui::GetTextLineHeightWithSpacing()*i+20);
            graphDrawList->AddLine(itemPos, ImVec2_add(itemPos, ImVec2(20, 0)), IM_COL32(plotColours[i][0]*0.1, plotColours[i][1]*0.1, plotColours[i][2]*0.1, plotColours[i][3]), 7);
            graphDrawList->AddLine(itemPos, ImVec2_add(itemPos, ImVec2(20, 0)), IM_COL32(plotColours[i][0], plotColours[i][1], plotColours[i][2], plotColours[i][3]), 5);
            ImGui::SetCursorPosX(30);
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(100, 100, 100, 250));
            ImGui::Text(formatNumber(Loaded_Data__perfDelays.getKey(i)).c_str());
            ImGui::PopStyleColor();
        }
    }
    ImGui::PopStyleColor();
    ImGui::End();

    selected_init = false;
    init = false;
}
void graph__processTime() {

}
void graph__iterationDelays() {

}
