
#include "includes.hpp"
#include "settings.hpp"
#include "variables.hpp"
#include "graph_selections.hpp"
#include "graph_methods.hpp"


int main(int argc, char** argv) {
    try {
        loadCSV_file("perf_delaysFile__Wave_Propagation.csv");
        if(Loaded_Data__perfDelays.size()==0) perror("ERROR: loaded data has a size of 0.");

        guiwin_nc::win_dim = guiwin_nc::toImVec2(dim__programWindow);
        pos2d<float> graphReg_padding = (dim__graphWindow-dim__graphRegion).modify([](int _var){return _var/2;}).cast<float>();
        graphObj.set_graphDim(dim__graphRegion.cast<float>());
        graphObj.setPos_GraphOrigo({
            graphReg_padding.x+dim__selectionsWindow.x, 
            float(dim__graphWindow.y-graphReg_padding.y)
        });
    
        ImGuiWindowFlags bgWindowFlags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoTitleBar;
        ImGuiCol bgWindowCol = IM_COL32(220, 220, 220, 255);

        graphWin.init();
        while(graphWin.running()) {
            graphWin.newFrame();
            if(!graphWin.running()) break;

            ImGui::SetNextWindowPos(ImVec2(0,0));
            ImGui::SetNextWindowSize(guiwin_nc::toImVec2(dim__selectionsWindow));
            ImGui::PushStyleColor(ImGuiCol_WindowBg, bgWindowCol);
            if(ImGui::Begin("selectionsWindow", NULL, bgWindowFlags)) {
                ImGui::PopStyleColor();
                for(size_t i=0; i<graph_selections.size(); i++) {
                    if(ImGui::Button(graph_selections.getKey(i).c_str())) {
                        selected__graph_selections = i;
                    }
                }
            }
            ImGui::End();
            ImGui::SetNextWindowPos(ImVec2(dim__selectionsWindow.x, 0));
            ImGui::SetNextWindowSize(guiwin_nc::toImVec2(dim__graphWindow));
            ImGui::PushStyleColor(ImGuiCol_WindowBg, bgWindowCol);
            if(ImGui::Begin("graphWindow", NULL, bgWindowFlags)) {
                ImGui::PopStyleColor();
                graphObj.drawBackground();
                graph_selections[selected__graph_selections]();
            }
            ImGui::End();
    
    
            graphWin.endFrame();
        }
        
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        exit(1);
    }
    


    return 0;
}
