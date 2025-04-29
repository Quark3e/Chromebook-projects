
#include <imguiwin.hpp>
#include <vector>
#include <string>
#include <iostream>

#include <useful.hpp>

#include <Eigen/Dense>


inline std::vector<float> solveServoDriftRegression(
    const std::vector<float>& refrValues,
    const std::vector<float>& readValues,
    size_t numPolynomialTerms
) {
    if (refrValues.size() != readValues.size() || refrValues.empty()) {
        throw std::invalid_argument("Input vectors must have the same non-zero size.");
    }

    size_t n = refrValues.size();
    Eigen::MatrixXd X(n, numPolynomialTerms);
    Eigen::VectorXd Y(n);

    // Construct the design matrix X and the target vector Y
    for (size_t i = 0; i < n; ++i) {
        float x = refrValues[i];
        Y(i) = readValues[i];
        for (size_t j = 0; j < numPolynomialTerms; ++j) {
            X(i, j) = std::pow(x, j);
        }
    }

    // Solve for the coefficients using the normal equation: (X^T * X) * coeffs = X^T * Y
    Eigen::VectorXd coeffs = (X.transpose() * X).ldlt().solve(X.transpose() * Y);

    // Convert Eigen vector to std::vector<float>
    std::vector<float> result(coeffs.data(), coeffs.data() + coeffs.size());
    return result;
}

int main(int argc, char** argv) {
    GUINC::imguiwin win(false);
    GUINC::win_dim = ImVec2(500, 500);

    std::vector<float> refrValues = {-1, 0, 1, 2, 3, 4, 5};
    std::vector<float> readValues = {-0.5, 0, 1.5, 2.8, 3.1, 4.4, 5.0};


    size_t numPolynomialTerms = 10; // Linear regression
    std::vector<float> coeffs = solveServoDriftRegression(refrValues, readValues, numPolynomialTerms);

    std::cout << "Coefficients: ";
    for (size_t i = 0; i < coeffs.size(); ++i) {
        std::cout << coeffs[i] << " ";
    }
    std::cout << std::endl;

    ImVec2 pos_origo(250, 250);
    pos2d<float> range_x(-10, 10);
    pos2d<float> range_y(-10, 10);
    float scal_x = range_x.getDelta()/GUINC::win_dim.x;
    float scal_y = range_y.getDelta()/GUINC::win_dim.y;


    win.init();
    while(win.running()) {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 0, 0, 1));
        win.newFrame();
        if(!win.running()) break;

        ImGui::SetNextWindowSize(ImVec2(500, 500), ImGuiCond_Always);
        ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(1, 1, 1, 0.9f));
        if(ImGui::Begin("Test Window", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar)) {
            ImGui::Text("Servo Drift Regression");
            ImGui::PopStyleColor();

            ImDrawList* draw_list = ImGui::GetWindowDrawList();
            draw_list->AddLine(ImVec2(0, pos_origo.y), ImVec2(GUINC::win_dim.x, pos_origo.y), IM_COL32(5, 5, 5, 255), 1.0f);
            draw_list->AddLine(ImVec2(pos_origo.x, 0), ImVec2(pos_origo.x, GUINC::win_dim.y), IM_COL32(5, 5, 5, 255), 1.0f);

            for(int i=0; i<500; i++) {
                float x = float(i-pos_origo.x)*scal_x;
                float y = 0; //coeffs[0] + coeffs[1]*(x);
                for(size_t n=0; n<coeffs.size(); n++) {
                    y += coeffs[n]*std::pow(x, n);
                }
                draw_list->AddCircleFilled(ImVec2(i, pos_origo.y-(y/scal_y)), 1.0f, IM_COL32(0, 255, 0, 255), 10);
            }
            for(size_t i=0; i<refrValues.size(); i++) {
                float x = (refrValues[i])/scal_x;
                float y = (readValues[i])/scal_y;
                draw_list->AddCircleFilled(ImVec2(pos_origo.x+x, pos_origo.y-y), 5.0f, IM_COL32(255, 0, 0, 255), 10);
            }

            ImGui::End();
        }

        win.endFrame();
        ImGui::PopStyleColor();
    }


    return 0;
}