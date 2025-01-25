

// #include "globals_includes.hpp"

#include "globals_variables.hpp"

bool            __ON_TERMINAL   = false;
unsigned int    __PROGRAM_FRAMES= (unsigned int)-1;


// bool running_main       = true;
// bool opt__enable_grid   = true;

DIY::typed_dict<std::string, DIY::typed_dict<std::string, bool>> _SETTINGS({
    {
        "Program", { // [0]
            {"Running Main", true} // [0][0]
        }
    }, {
        "View", { // [1]
            {"Draw Grid", true} // [1][0]
        }
    }, {
        "Timeline", { // [2]
            {"use timeObject snap", true}, // [2][0]
            {"test 1", false},
            {"test 2", true}
        }
    }, {
        "File", { // [3]
            {"Auto Save", true} // [3][0]
        }
    }
});

int __GLOBAL_FLAGS__WIN_RESIZED = 0;
int __GLOBAL_FLAGS__WIN_CLOSING = 0;


std::chrono::system_clock::time_point   __CURRENT__TIME_POINT   = std::chrono::system_clock::now();
time_t                                  __CURRENT__TIME_T       = std::chrono::system_clock::to_time_t(__CURRENT__TIME_POINT);
std::string                             __CURRENT__TIME_STRING  = std::string(ctime(&__CURRENT__TIME_T));

std::string programCWD;

#if _WIN32
std::string __dir_programRoot   = "/Users/berkh/Projects/Github_repo/Chromebook-projects/teststuff/cpp/useful/nodeChart/";
std::string __dir_saveFiles     = "/Users/berkh/Projects/Github_repo/Chromebook-projects/teststuff/cpp/useful/nodeChart/saveFiles/";
#else
std::string __dir_programRoot   = "/home/berkhme/github_repo/Chromebook-projects/teststuff/cpp/useful/nodeChart/";
std::string __dir_saveFiles     = "/home/berkhme/github_repo/Chromebook-projects/teststuff/cpp/useful/nodeChart/saveFiles/";
#endif

// DIY::typed_dict<std::string, gNC::guiNodeChart> projects;
DIY::typed_dict<std::string, nc_proj> projects;


ImDrawList* project_draw_list = nullptr;


ImVec2 dim__main        = ImVec2(1280, 700);
ImVec2 dim__menu__detail= ImVec2(300, 600);

ImVec2 pos__menu__detail__offset    = ImVec2(20, 60);


ImVec2 _coef_dim__win_fileExplorer  = ImVec2(0.7, 0.8);
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
        _retur[0].x + ImGui::GetWindowSize().x*0.6,
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
        ImGui::GetWindowSize().x,
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
        ImGui::GetWindowSize().x,
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
        ImGui::GetWindowSize().x,
        ImGui::GetWindowSize().y - 150
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
        ImGui::GetWindowSize().x,
        ImGui::GetWindowSize().y
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
        ImGui::GetWindowSize().y
    );
    _retur[2] = ImVec2(_retur[1].x-_retur[0].x, _retur[1].y-_retur[0].y);
    return _retur;
}

ImVec2 _coef_dim__win_timeline      = ImVec2(1, 250);
ImVec2 dim__win_timeline() {
    static ImVec2 _retur(-1, -1);
    _retur.x = (_coef_dim__win_timeline[0]>1? _coef_dim__win_timeline[0] : _coef_dim__win_timeline[0] * dim__main[0]);
    _retur.y = (_coef_dim__win_timeline[1]>1? _coef_dim__win_timeline[1] : _coef_dim__win_timeline[1] * dim__main[1]);
    return _retur;
}


pressed_key__struct guiKeys;

void _keyBind__Master_Close() {
    std::cout << " Master Close called." << std::endl;
    _SETTINGS[0][0] = false;
}
DIY_KBH::keyBind_handler keyBinds({
    {"undo",    {ImGuiKey_ReservedForModCtrl, ImGuiKey_LeftCtrl, ImGuiKey_Z}},
    {"redo",    {ImGuiKey_ReservedForModCtrl, ImGuiKey_LeftCtrl, ImGuiKey_ReservedForModShift, ImGuiKey_LeftShift, ImGuiKey_Z}},
    {"Master Close",{ImGuiKey_ReservedForModCtrl, ImGuiKey_LeftCtrl, ImGuiKey_W}, _keyBind__Master_Close},
    {"Ctrl+Enter",  {ImGuiKey_ReservedForModCtrl, ImGuiKey_LeftCtrl, ImGuiKey_Enter}},
    {"MouseLeft",   {ImGuiKey_MouseLeft}},
    {"MouseRight",  {ImGuiKey_MouseRight}}
});


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
