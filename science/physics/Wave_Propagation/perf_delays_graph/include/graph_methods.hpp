#pragma once
#ifndef HPP__WavePropagation_sim_delays_graph__graph_methods
#define HPP__WavePropagation_sim_delays_graph__graph_methods

#include "includes.hpp"
// #include "variables.hpp"
#include "graph_selections.hpp"


enum imguigraph_plotType {
    plotType_line,
    plotType_dotted
};

/// Temporary placement in this header/src files. Need to move into separate header/source file pair or a library
class imguigraph {
    private:
    pos2d<float> _dim_graphWindow{1000, 1000};

    pos2d<double> _range_x{0, 100};
    pos2d<double> _range_y{0, 100};
    
    double _scal_x = 1;
    double _scal_y = 1;

    double _spacing_x = 10;
    double _spacing_y = 10;

    ImVec2 _pos_origo{0, 0};

    ImVec2 _draw_graph_axis_x{100, 0};
    ImVec2 _draw_graph_axis_y{0, 100};
    void _update_drawGraphVar();

    public:
    imguigraph(pos2d<float> dim_draw, pos2d<float> pos_origo);
    imguigraph(const imguigraph& _obj);
    ~imguigraph();

    imguigraph& operator=(const imguigraph& _obj);

    void setPos_GraphOrigo(pos2d<float> pos);
    void set_graphDim(pos2d<float> dim);

    void setRange_x(double min, double max);
    void setRange_y(double min, double max);

    void setSpacing_x(double spacing);
    void setSpacing_y(double spacing);

    void plotData(
        std::vector<double> x_values,
        std::vector<double> y_values,
        imguigraph_plotType _typeOfPlot = plotType_line,
        ImGuiCol plotColor = IM_COL32(200, 100, 100, 200)
    );

    void drawBackground();
};




#endif //HPP__WavePropagation_sim_delays_graph__graph_methods