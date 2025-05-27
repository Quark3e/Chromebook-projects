
#include <iostream>
#include <string>
#include <vector>

#include <cmath>

#include <pos2d.hpp>

#include <imguiwin.hpp>
#include <implot3d.h>


GUINC::imguiwin guiwin(false, 1000, 1000);

int main(int argc, char** argv) {
    std::cout << "test" << std::endl;
    size_t numPoints = 1000;

    std::vector<float> data_x;
    std::vector<float> data_y;
    std::vector<float> data_z;

    std::vector<float> emptyData(numPoints, 0);
    
    for(size_t i=0; i<numPoints; i++) {

        data_x.push_back(std::cos(double(i)*((10*M_PI)/numPoints)));
        data_z.push_back(std::sin(double(i)*((10*M_PI)/numPoints)));
        data_y.push_back(i*((10*M_PI)/numPoints));
    }

    assert(guiwin.init());
    ImPlot3D::CreateContext();
    while(guiwin.running()) {
        try {
            guiwin.newFrame();

            ImGui::SetWindowFontScale(1.1);
            if (ImPlot3D::BeginPlot("My Plot", ImVec2(1000, 1000))) {
                ImPlot3D::SetupLegend(ImPlot3DLocation(1));
                // ImPlot3D::SetupBoxScale(2, 2, 2);
                ImPlot3D::SetupAxes("cos", "angle[rad]", "sin");
                ImPlot3D::SetupAxesLimits(-1, 1, 0, 10*M_PI, -1, 1);
                // ImPlot::PlotLine("My Line Plot", x_data, y_data, numPoints);

                ImPlot3D::PlotLine("Line_Cosine", data_x.data(), data_y.data(), emptyData.data(), numPoints);
                ImPlot3D::PlotLine("Line_Sine", emptyData.data(), data_y.data(), data_z.data(), numPoints);
                ImPlot3D::PlotLine("Line_Total", data_x.data(), data_y.data(), data_z.data(), numPoints);
                    
                ImPlot3D::EndPlot();
            }

            guiwin.endFrame();
        }
        catch(const std::exception& e) {
            std::cerr << e.what() << '\n';
            exit(1);
        }
    }
    ImPlot3D::DestroyContext();
    return 0;
}
