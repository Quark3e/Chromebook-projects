

#include "globals_includes.hpp"


std::string programCWD;

DIY::typed_dict<std::string, gNC::guiNodeChart> projects;


ImDrawList* project_draw_list = nullptr;


ImVec2 dim__main        = ImVec2(1280, 700);
ImVec2 dim__menu__detail= ImVec2(300, 600);

ImVec2 dim__menu__detail__offset    = ImVec2(20, 60);


ImVec2 _coef_dim__win_fileExplorer = ImVec2(0.7, 0.8);
ImVec2 dim__win_fileExplorer() {
    static ImVec2 _retur(-1, -1);
    _retur.x = dim__main[0] * _coef_dim__win_fileExplorer[0];
    _retur.y = dim__main[1] * _coef_dim__win_fileExplorer[1];
    return _retur;
}


ImVec2 padding__content_fileExplorer = ImVec2(10, 10);

std::vector<ImVec2>& _pos__sub_fileExplorer_addressBar(bool _update) {
    static std::vector<ImVec2> _retur(3, ImVec2(-1, -1));
    if(!_update) return _retur;

    _retur[0] = ImVec2(
        _pos__sub_fileExplorer_navPanel()[1].x,
        0
    );
    _retur[1] = ImVec2(
        _retur[0].x + dim__win_fileExplorer().x*0.6,
        _retur[0].y + 100
    );
    _retur[2] = ImVec2(_retur[1].x-_retur[0].x, _retur[1].y-_retur[0].y);
    return _retur;
}
std::vector<ImVec2>& _pos__sub_fileExplorer_searchBar(bool _update) {
    static std::vector<ImVec2> _retur(3, ImVec2(-1, -1));
    if(!_update) return _retur;

    _retur[0] = ImVec2(
        _pos__sub_fileExplorer_addressBar()[1].x,
        _pos__sub_fileExplorer_addressBar()[0].y
    );
    _retur[1] = ImVec2(
        dim__win_fileExplorer().x,
        _pos__sub_fileExplorer_addressBar()[1].y
    );
    _retur[2] = ImVec2(_retur[1].x-_retur[0].x, _retur[1].y-_retur[0].y);
    return _retur;
}
std::vector<ImVec2>& _pos__sub_fileExplorer_headings(bool _update) {
    static std::vector<ImVec2> _retur(3, ImVec2(-1, -1));
    if(!_update) return _retur;

    _retur[0] = ImVec2(
        _pos__sub_fileExplorer_navPanel()[1].x,
        _pos__sub_fileExplorer_addressBar()[1].y
    );
    _retur[1] = ImVec2(
        dim__win_fileExplorer().x,
        _retur[0].y + 50
    );
    _retur[2] = ImVec2(_retur[1].x-_retur[0].x, _retur[1].y-_retur[0].y);
    return _retur;
}
std::vector<ImVec2>& _pos__sub_fileExplorer_contents(bool _update) {
    static std::vector<ImVec2> _retur(3, ImVec2(-1, -1));
    if(!_update) return _retur;

    _retur[0] = ImVec2(
        _pos__sub_fileExplorer_navPanel()[1].x,
        _pos__sub_fileExplorer_addressBar()[1].y
        // _pos__sub_fileExplorer_headings()[1].y
    );
    _retur[1] = ImVec2(
        dim__win_fileExplorer().x,
        dim__win_fileExplorer().y - 150
    );
    _retur[2] = ImVec2(_retur[1].x-_retur[0].x, _retur[1].y-_retur[0].y);
    return _retur;
}
std::vector<ImVec2>& _pos__sub_fileExplorer_detailsPanel(bool _update) {
    static std::vector<ImVec2> _retur(3, ImVec2(-1, -1));
    if(!_update) return _retur;

    _retur[0] = ImVec2(
        _pos__sub_fileExplorer_navPanel()[1].x,
        _pos__sub_fileExplorer_contents()[1].y
    );
    _retur[1] = ImVec2(
        dim__win_fileExplorer().x,
        dim__win_fileExplorer().y
    );
    _retur[2] = ImVec2(_retur[1].x-_retur[0].x, _retur[1].y-_retur[0].y);
    return _retur;
}
std::vector<ImVec2>& _pos__sub_fileExplorer_navPanel(bool _update) {
    static std::vector<ImVec2> _retur(3, ImVec2(-1, -1));
    if(!_update) return _retur;

    _retur[0] = ImVec2(0, 0);
    _retur[1] = ImVec2(
        _retur[0].x + 200,
        dim__win_fileExplorer().y
    );
    _retur[2] = ImVec2(_retur[1].x-_retur[0].x, _retur[1].y-_retur[0].y);
    return _retur;
}



bool lockMove_screen    = true;
bool lockMove_node      = true;


int decay_mouseClick_left   = 0;
int decay_mouseClick_right  = 0;

int mouseAction_left    = -1;
int mouseAction_right   = -1;

bool mouseDrag_left      = false;

int mouseTimer_decay    = 20;


int static_mouseAction_left= -1;


// int keyHistory_len = 100;
// std::vector<int> keyHistory;
// std::vector<int> keyHistory_mouseLeft;

std::string formatBytes(
    off_t _storage,
    int width,
    int decimals,
    bool use_sciNot,
    std::string align
) {
    std::stringstream ss;
    static std::string _type[4] = {"B", "KB", "MB", "GB"};

    if(!use_sciNot) ss << std::fixed;
    ss << (align=="left"? std::left : std::right) << std::setw(width) << std::setprecision(decimals);
    for(int i=0; i<4; i++) {
        if(_storage<pow(10, (i+1)*3)) {
            ss << static_cast<double>(_storage)/pow(10, i*3);
            ss << " "+_type[i];
            return ss.str();
        }
    }
    ss << static_cast<double>(_storage)/pow(10, 12);
    ss << " TB";
    return ss.str();
}
