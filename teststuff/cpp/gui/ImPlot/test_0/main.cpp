
#include <iostream>
#include <string>
#include <vector>

#include <cmath>

#include <pos2d.hpp>

#include <imguiwin.hpp>
#include <implot.h>


GUINC::imguiwin guiwin(false, 1000, 1000);

int main(int argc, char** argv) {
    
    int   bar_data[11];
    float x_data[1000];
    float y_data[1000];

    std::vector<float> data_x;
    std::vector<float> data_y;
    
    for(size_t i=0; i<1000; i++) {
        if(i<11) {
            bar_data[i] = i;
        }
        x_data[i] = i;
        y_data[i] = std::sin(double(i));

        data_x.push_back(i*(10.0/1000));
        data_y.push_back(4.0*std::sin(double(i)*((2*M_PI)/1000))+5);
    }

    
    assert(guiwin.init());
    while(guiwin.running()) {
        try {
            guiwin.newFrame();
            ImPlot::CreateContext();

            if (ImPlot::BeginPlot("My Plot")) {
                ImPlot::SetupAxes("X", "Y");
                ImPlot::SetupAxesLimits(0, 11, 0, 10);
                ImPlot::PlotBars("My Bar Plot", bar_data, 11);
                // ImPlot::PlotLine("My Line Plot", x_data, y_data, 1000);
                ImPlot::PlotLine("My Line Plot", data_x.data(), data_y.data(), data_x.size());

                ImPlot::EndPlot();
            }

            ImPlot::DestroyContext();
            guiwin.endFrame();
        }
        catch(const std::exception& e) {
            std::cerr << e.what() << '\n';
            exit(1);
        }
        
    }


    return 0;
}