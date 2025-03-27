
#include "graph_methods.hpp"
#include <useful.hpp>


void imguigraph::_update_drawGraphVar() {
    _draw_graph_axis_x = ImVec2_add(_pos_origo, ImVec2(_dim_graphWindow.x, 0));
    _draw_graph_axis_y = ImVec2_add(_pos_origo, ImVec2(0, -_dim_graphWindow.y));

    _scal_x = (_range_x[1]-_range_x[0]) / _dim_graphWindow.x;
    _scal_y = (_range_y[1]-_range_y[0]) / _dim_graphWindow.y;
}
imguigraph::imguigraph(pos2d<float> dim_draw, pos2d<float> pos_origo)
: _dim_graphWindow{dim_draw}, _pos_origo{pos_origo.x, pos_origo.y}
{

}
imguigraph::imguigraph(const imguigraph& _obj)
: _dim_graphWindow(_obj._dim_graphWindow),
_range_x(_obj._range_x), _range_y(_obj._range_y),
_spacing_x(_obj._spacing_x), _spacing_y(_obj._spacing_y),
_pos_origo(_obj._pos_origo),
_draw_graph_axis_x(_obj._draw_graph_axis_x), _draw_graph_axis_y(_obj._draw_graph_axis_y),
_scal_x(_obj._scal_x), _scal_y(_obj._scal_y)
{

}
imguigraph::~imguigraph() {

}

imguigraph& imguigraph::operator=(const imguigraph& _obj) {
    _dim_graphWindow = _obj._dim_graphWindow;
    _range_x = _obj._range_x;
    _range_y = _obj._range_y;
    _spacing_x = _obj._spacing_x;
    _spacing_y = _obj._spacing_y;
    _pos_origo = _obj._pos_origo;
    _draw_graph_axis_x = _obj._draw_graph_axis_x;
    _draw_graph_axis_y = _obj._draw_graph_axis_y;
    _scal_x = _obj._scal_x;
    _scal_y = _obj._scal_y;

    return *this;
}


void imguigraph::setPos_GraphOrigo(pos2d<float> pos) {
    _pos_origo = ImVec2(pos.x, pos.y);
    _update_drawGraphVar();
}
void imguigraph::set_graphDim(pos2d<float> dim) {
    _dim_graphWindow.x = dim.x;
    _dim_graphWindow.y = dim.y;
    _update_drawGraphVar();
}
void imguigraph::setRange_x(double min, double max) {
    _range_x.x = min;
    _range_x.y = max;
    _update_drawGraphVar();
}
void imguigraph::setRange_y(double min, double max) {
    _range_y.x = min;
    _range_y.y = max;
    _update_drawGraphVar();
}

void imguigraph::setSpacing_x(double spacing) {
    _spacing_x = spacing;
    _update_drawGraphVar();
}
void imguigraph::setSpacing_y(double spacing) {
    _spacing_y = spacing;
    _update_drawGraphVar();
}

void imguigraph::plotData(
    std::vector<double> x_values,
    std::vector<double> y_values,
    imguigraph_plotType _typeOfPlot,
    ImGuiCol plotColor
) {
    if(x_values.size()==0) throw std::invalid_argument("ERROR: plotData: x_values can't be empty.");
    if(y_values.size()==0) throw std::invalid_argument("ERROR: plotData: y_values can't be empty.");
    if(x_values.size()!=y_values.size()) throw std::invalid_argument("ERROR: plotData: x_values and y_values are not the same size");

    ImDrawList* graphDrawList = ImGui::GetWindowDrawList();
    ImVec2 pixelPos(0, 0);
    switch (_typeOfPlot) {
        case plotType_line: {
            ImVec2 pixelPos2(0, 0);
            pixelPos = ImVec2(_pos_origo.x+(x_values[0]-_range_x[0])/_scal_x, _pos_origo.y-(y_values[0]-_range_y[0])/_scal_y);
            for(size_t i=1; i<x_values.size(); i++) {
                pixelPos2 = ImVec2(_pos_origo.x+(x_values[i]-_range_x[0])/_scal_x, _pos_origo.y-(y_values[i]-_range_y[0])/_scal_y);
                graphDrawList->AddLine(pixelPos, pixelPos2, plotColor, 3);
                pixelPos = pixelPos2;
            }
        }
        break;
        case plotType_dotted: {
            for(size_t i=1; i<x_values.size(); i++) {
                pixelPos = ImVec2(_pos_origo.x+(x_values[i]-_range_x[0])/_scal_x, _pos_origo.y-(y_values[i]-_range_y[0])/_scal_y);
                graphDrawList->AddCircleFilled(pixelPos, 10, plotColor, 10);
            }
        }
        break;
    default:
        throw std::invalid_argument("Invalid _typeOfPlot argument");
        break;
    }

}

void imguigraph::drawBackground() {
    ImDrawList* graphDrawList = ImGui::GetWindowDrawList();

    graphDrawList->AddLine(ImVec2_subtract(_pos_origo, ImVec2(10, 0)), _draw_graph_axis_x, IM_COL32(10,10,10,250), 2);
    graphDrawList->AddLine(ImVec2_subtract(_pos_origo, ImVec2(0, -10)), _draw_graph_axis_y, IM_COL32(10,10,10,250), 2);
    graphDrawList->AddTriangleFilled(
        ImVec2_add(_draw_graph_axis_x, ImVec2(0,  7)),
        ImVec2_add(_draw_graph_axis_x, ImVec2(0, -7)),
        ImVec2_add(_draw_graph_axis_x, ImVec2(10, 0)),
        IM_COL32(10,10,10,250)
    );
    graphDrawList->AddTriangleFilled(
        ImVec2_add(_draw_graph_axis_y, ImVec2(7, 0)),
        ImVec2_add(_draw_graph_axis_y, ImVec2(-7, 0)),
        ImVec2_add(_draw_graph_axis_y, ImVec2( 0, -10)),
        IM_COL32(10,10,10,250)
    );

    for(float x=_range_x[0]; x<_range_x[1]; x+=_spacing_x) {
        float scal = (x-_range_x[0])/(_range_x[1]-_range_x[0]);
        ImVec2 linePos(_pos_origo.x + scal*_dim_graphWindow.x, _pos_origo.y);
        graphDrawList->AddLine(linePos, ImVec2_add(linePos, ImVec2(0, -_dim_graphWindow.y)), IM_COL32(170,170,170,200), 1);
    }
    for(float y=_range_y[0]; y<_range_y[1]; y+=_spacing_y) {
        float scal = (y-_range_y[0])/(_range_y[1]-_range_y[0]);
        ImVec2 linePos(_pos_origo.x, _pos_origo.y - scal*_dim_graphWindow.y);
        graphDrawList->AddLine(linePos, ImVec2_add(linePos, ImVec2(_dim_graphWindow.x, 0)), IM_COL32(170,170,170,200), 1);
    }
    
    graphDrawList->AddCircle(_pos_origo, 5, IM_COL32(10,10,10,250), 10);
    graphDrawList->AddText(_pos_origo, IM_COL32(10,10,10,250), "0");
    for(size_t i_x=0; i_x<=(_range_x[1]-_range_x[0])/_spacing_x; i_x++) {
        ImVec2 pos(_pos_origo.x+(i_x*_spacing_x)/_scal_x-5, _pos_origo.y+ImGui::GetTextLineHeightWithSpacing());
        ImGui::SetCursorPos(pos);
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0,0,0,255));
        graphDrawList->AddText(pos, IM_COL32(0,0,0,255), formatNumber(i_x*_spacing_x+_range_x[0], 0, 1).c_str());
        ImGui::PopStyleColor();
    }
    for(size_t i_y=0; i_y<=(_range_y[1]-_range_y[0])/_spacing_y; i_y++) {
        ImVec2 pos(_pos_origo.x-50, _pos_origo.y-(i_y*_spacing_y)/_scal_y-ImGui::GetTextLineHeightWithSpacing()/2);
        ImGui::SetCursorPos(pos);
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0,0,0,255));
        graphDrawList->AddText(pos, IM_COL32(0,0,0,255), formatNumber(i_y*_spacing_y+_range_y[0], 0, 1).c_str());
        ImGui::PopStyleColor();
    }

}

