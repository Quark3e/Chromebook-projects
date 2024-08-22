

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_allegro5.h>


#include "gui_nodeChart.hpp"
#include <HC_useful/useful.hpp>


const gNC::gNODE gNC::guiNodeChart::default_gNODE = {
    0, 0,
    "", "", ""
};


template<typename storedType>
int gNC::guiNodeChart::_find_ptr_idx(const std::list<storedType>& toCheck, storedType* ptr_toFind) {
    int found = -1;
    for(auto itr=toCheck.begin(); itr!=toCheck.end(); ++itr) {
        found ++;
        if(&*itr==ptr_toFind) {
            return found;
        }
    }
    return found;
}
template<typename storedType>
auto gNC::guiNodeChart::_find_ptr_itr(const std::list<storedType>& toCheck, storedType* ptr_toFind) {
    int found = -1;
    for(auto itr=toCheck.begin(); itr!=toCheck.end(); ++itr) {
        found++;
        if(&(*itr)==ptr_toFind) {
            return itr;
        }
    }
    return toCheck.end();
}

template<typename storedType>
int gNC::guiNodeChart::_vecfind_ptr_idx(const std::vector<storedType>& toCheck, storedType toFind) {
    int found = -1;
    for(auto itr=toCheck.begin(); itr!=toCheck.end(); ++itr) {
        found++;
        if(*itr==toFind) {
            return found;
        }
    }
    return found;
}
template<typename storedType>
auto gNC::guiNodeChart::_vecfind_ptr_itr( std::vector<storedType>& toCheck, storedType toFind) {
    int found = -1;
    for(auto itr=toCheck.begin(); itr!=toCheck.end(); ++itr) {
        found++;
        if(*itr==toFind) {
            return itr;
        }
    }
    return toCheck.end();
}

gNC::guiNodeChart::guiNodeChart(/* args */) {

}

/**
 * @brief move the background screen position and the objects on it.
 * 
 * @param x new x coordinate value
 * @param y new y coordinate value
 * @param moveMode
 *  - `0` absolute
 *  - `1` relative
 * @return int 
 */
int gNC::guiNodeChart::setScreen_pos(int x, int y, int moveMode) {
    assert(moveMode>0 && moveMode<2);

    switch (moveMode) {
    case 0:
        this->screen_pos_delta[0] = x-this->screen_pos[0];
        this->screen_pos_delta[1] = y-this->screen_pos[1];
        this->screen_pos[0] = x;
        this->screen_pos[1] = y;
        break;
    case 1:
        this->screen_pos_delta[0] = x;
        this->screen_pos_delta[1] = y;
        this->screen_pos[0]+=x;
        this->screen_pos[1]+=y;
        break;
    default:
        break;
    }

    return 0;
}
int gNC::guiNodeChart::setScreen_dim(int w, int h) {
    this->screen_dim[0] = w;
    this->screen_dim[1] = h;
    return 0;
}

size_t gNC::guiNodeChart::size(int whatList) {
    if(whatList==0) return this->_nodes.size();
    else if(whatList==1) return this->_links.size();
    std::runtime_error("ERROR: "+this->_info_name+"size(int): input arguemnt for `whatList` does not match available options");
    return 0;
}
gNC::gNODE* gNC::guiNodeChart::lastAdded_NODE() {
    return this->_lastAddedNode;
}
gNC::gLINK* gNC::guiNodeChart::lastAdded_LINK() {
    return this->_lastAddedLink;
}

gNC::gNODE gNC::guiNodeChart::operator[](size_t i) const {
    if(i>=this->_nodes.size()) std::runtime_error("ERROR: "+_info_name+"operator[](size_t) index is out of range");

    auto itr = this->_nodes.begin();
    advance(itr, i);
    return const_cast<gNC::gNODE&>(*itr);
}


std::list<gNC::gNODE> gNC::guiNodeChart::NODES() {
    return this->_nodes;
}
std::list<gNC::gLINK> gNC::guiNodeChart::LINKS() {
    return this->_links;
}

gNC::gNODE* gNC::guiNodeChart::NODE_create(
    float pos_x,
    float pos_y,
    std::string label,
    std::string desc,
    std::string bodyText,
    float width,
    float height
) {
    this->_nodes.push_back(gNC::gNODE(
        pos_x, pos_y, label, desc, bodyText
    ));

    this->_lastAddedNode = &(this->_nodes.back());
    this->_lastAddedNode->addr = ptrToStr<gNC::gNODE*>(this->_lastAddedNode);

    return this->_lastAddedNode;
}
int gNC::guiNodeChart::NODE_delete(size_t NODE_idx, bool leaveFloating) {
    if(NODE_idx >= this->_nodes.size()) std::runtime_error(
        "ERROR:: "+this->_info_name+"NODE_delete(size_t, bool):"+" arg for `NODE_idx` is out of range"
    );

    gNC::gNODE* erasePtr;
    std::list<gNC::gNODE>::iterator itr = this->_nodes.begin();
    advance(itr, NODE_idx);
    return this->NODE_delete(&*itr, leaveFloating);
}
int gNC::guiNodeChart::NODE_delete(gNC::gNODE* NODE_toDelete, bool leaveFloating) {
    std::list<gNC::gNODE>::const_iterator eraseItr = this->_find_ptr_itr<gNC::gNODE>(this->_nodes, NODE_toDelete);
    if(eraseItr==this->_nodes.end()) std::runtime_error(
        "ERROR: "+this->_info_name+"NODE_delete(gNC::gNODE*, bool):"+
        " arg for node address does not exist in stored nodes."
    );

    if(!leaveFloating) {
        for(size_t i=0; i<4; i++) {
            const std::vector<gNC::gLINK*> linkVec = (
                i==0? NODE_toDelete->ln_in : 
                (i==1? NODE_toDelete->ln_out : 
                (i==2? NODE_toDelete->ln_add : 
                NODE_toDelete->ln_share))
            );

            std::vector<std::list<gNC::gLINK>::const_iterator> itrToErase;

            for(gNC::gLINK* plink: linkVec) {
                if(NODE_toDelete==plink->src) {
                    std::vector<gNC::gLINK*>& plink_type = (plink->type_dest==0? plink->dest->ln_in : plink->dest->ln_add);
                    for(gNC::gLINK* pplink: plink_type) {
                        if(pplink==plink) plink_type.erase(this->_vecfind_ptr_itr<gNC::gLINK*>(plink_type, plink));
                    }
                }
                else if(NODE_toDelete==plink->dest) {
                    std::vector<gNC::gLINK*>& plink_type = (plink->type_src==1? plink->src->ln_out : plink->src->ln_share);
                    for(gNC::gLINK* pplink: plink_type) {
                        if(pplink==plink) plink_type.erase(this->_vecfind_ptr_itr<gNC::gLINK*>(plink_type, plink));
                    }
                }
                itrToErase.push_back(this->_find_ptr_itr<gNC::gLINK>(this->_links, plink));
            }
            for(auto itrs: itrToErase) this->_links.erase(itrs);
        }
    }
    else if(leaveFloating) {
        for(size_t i=0; i<4; i++) {
            const std::vector<gNC::gLINK*> linkVec = (
                i==0? NODE_toDelete->ln_in : 
                (i==1? NODE_toDelete->ln_out : 
                (i==2? NODE_toDelete->ln_add : 
                NODE_toDelete->ln_share))
            );

            for(gNC::gLINK* plink: linkVec) {
                if(NODE_toDelete==plink->src)       plink->src  = nullptr;
                else if(NODE_toDelete==plink->dest) plink->dest = nullptr;
            }
        }
    }

    this->_nodes.erase(this->_find_ptr_itr<gNC::gNODE>(this->_nodes, NODE_toDelete));

    return 0;
}


int gNC::guiNodeChart::NODE_move(gNC::gNODE* NODE_toMove, float new_X, float new_Y) {
    auto moveItr = this->_find_ptr_itr<gNC::gNODE>(this->_nodes, NODE_toMove);
    if(moveItr==this->_nodes.end()) std::runtime_error(
        "ERROR: "+_info_name+"NODE_move(gNC::gNODE*, float, float)"+
        " arg for gNC::gNODE address is invalid"
    );

    NODE_toMove->pos[0] = new_X;
    NODE_toMove->pos[1] = new_Y;

    return 0;
}

gNC::gLINK* gNC::guiNodeChart::LINK_create(
    size_t NODE_src_idx,
    size_t NODE_dest_idx,
    int type_src,
    int type_dest,
    std::string label,
    std::string desc
) {
    if(NODE_src_idx>=this->_nodes.size() || NODE_dest_idx>=this->_nodes.size()) {
        std::runtime_error(
            "ERROR: "+this->_info_name+"LINK_create(size_t, size_t, int, int, std::string, std::string):"+
            " arg for `{..}_idx` is out of range"
        );
    }

    gNC::gNODE *ptrSrc, *ptrDest;
    std::list<gNC::gNODE>::iterator itr = this->_nodes.begin();
    advance(itr, NODE_src_idx);
    ptrSrc  = &*itr;
    itr = this->_nodes.begin();
    advance(itr, NODE_dest_idx);
    ptrDest = &*itr;

    return this->LINK_create(ptrSrc, ptrDest, type_src, type_dest, label, desc);
}
gNC::gLINK* gNC::guiNodeChart::LINK_create(
    gNC::gNODE* NODE_src,
    gNC::gNODE* NODE_dest,
    int type_src,
    int type_dest,
    std::string label,
    std::string desc
) {
    std::list<gNC::gNODE>::const_iterator
        eraseItr_src    = this->_find_ptr_itr<gNC::gNODE>(this->_nodes, NODE_src),
        eraseItr_dest   = this->_find_ptr_itr<gNC::gNODE>(this->_nodes, NODE_dest);

    if(eraseItr_src==this->_nodes.end() || eraseItr_dest==this->_nodes.end())
        std::runtime_error(
            "ERROR: "+this->_info_name+"LINK_create(gNC::gNODE*, gNC::gNODE*, int, int, std::string, std::string):"+
            " arg(s) for node address(es) does not exist in stored nodes."
        );
    if(type_src!=1 && type_src!=3 && type_dest!=0 && type_dest!=2) 
        std::runtime_error(
            "ERROR: "+this->_info_name+"LINK_create(gNC::gNODE*, gNC::gNODE*, int, int, std::string, std::string):"+
            " arg(s) for type_{..} is/are not valid."
        );

    this->_links.push_back(gNC::gLINK(type_src, type_dest, NODE_src, NODE_dest, label, desc));
    this->_lastAddedLink = &(this->_links.back());
    this->_lastAddedLink->addr = ptrToStr<gNC::gLINK*>(this->_lastAddedLink);
    return this->_lastAddedLink;
}
int gNC::guiNodeChart::LINK_swapSrc(gNC::gLINK* toSwap, gNC::gNODE* newSrc, int srcType) {
    if(srcType!=1 && srcType!=3) std::runtime_error("ERROR: "+this->_info_name+"LINK_swapSrc(gNC::gLINK*, gNC::gNODE*, int): invalid `srcType` input");
    if(_find_ptr_itr<gNC::gNODE>(_nodes, newSrc)==_nodes.end())
        std::runtime_error("ERROR: "+this->_info_name+"LINK_swapSrc(gNC::gLINK*, gNC::gNODE*, int): arg for `newSrc` is not a valid `gNC::gNODE` address");
    else if(_find_ptr_itr<gNC::gLINK>(_links, toSwap)==_links.end())
        std::runtime_error("ERROR: "+this->_info_name+"LINK_swapSrc(gNC::gLINK*, gNC::gNODE*, int): arg for `toSwap` is not a valid `gNC::gLINK` address");


    if(toSwap->src==newSrc) return 1;

    if(toSwap->src!=nullptr) {
        std::vector<gNC::gLINK*>& eraseVec = (toSwap->type_src==1? toSwap->src->ln_out : toSwap->src->ln_share);
        std::vector<gNC::gLINK*>::const_iterator deleteItr = _vecfind_ptr_itr<gNC::gLINK*>(eraseVec, toSwap);
        eraseVec.erase(deleteItr);
    }

    toSwap->src = newSrc;

    if(srcType==1)      newSrc->ln_out.push_back(toSwap);
    else if(srcType==3) newSrc->ln_share.push_back(toSwap);

    return 0;
}
int gNC::guiNodeChart::LINK_swapDest(gNC::gLINK* toSwap, gNC::gNODE* newDest, int destType) {
    if(destType!=0 && destType!=2) std::runtime_error("ERROR: "+this->_info_name+"LINK_swapDest(gNC::gLINK*, gNC::gNODE*, int): invalid `destType` input");
    if(_find_ptr_itr<gNC::gNODE>(_nodes, newDest)==_nodes.end())
        std::runtime_error("ERROR: "+this->_info_name+"LINK_swapDest(gNC::gLINK*, gNC::gNODE*, int): arg for `newDest` is not a valid `NC::NODE` address");
    else if(_find_ptr_itr<gNC::gLINK>(_links, toSwap)==_links.end())
        std::runtime_error("ERROR: "+this->_info_name+"LINK_swapDest(gNC::gLINK*, gNC::gNODE*, int): arg for `toSwap` is not a valid `NC::LINK` address");

    if(toSwap->dest==newDest) return 1;

    if(toSwap->dest!=nullptr) {
        // locate and erase the link address from the current dest NODE
        std::vector<gNC::gLINK*>& eraseVec = (toSwap->type_dest==0? toSwap->dest->ln_in : toSwap->dest->ln_add);
        std::vector<gNC::gLINK*>::const_iterator deleteItr = _vecfind_ptr_itr<gNC::gLINK*>(eraseVec, toSwap);
        eraseVec.erase(deleteItr);
    }
    // update dest address in the link
    toSwap->dest = newDest;

    // add NC::LINK address to the new NODE in correct std::vector container
    if(destType==0) newDest->ln_in.push_back(toSwap);
    else if(destType==2) newDest->ln_add.push_back(toSwap);

    return 0;
}
int gNC::guiNodeChart::LINK_delete(gNC::gLINK* LINK_toDelete) {
    std::list<gNC::gLINK>::const_iterator linkItr = _find_ptr_itr<gNC::gLINK>(this->_links, LINK_toDelete);
    if(linkItr==this->_links.end()) std::runtime_error("ERROR: "+this->_info_name+"LINK_delete(gNC::gLINK*): invalid `gNC::gLINK` address to delete");


    if(LINK_toDelete->src!=nullptr) {
        std::vector<gNC::gLINK*>& eraseVec = (LINK_toDelete->type_src==1? LINK_toDelete->src->ln_out : LINK_toDelete->src->ln_share);
        std::vector<gNC::gLINK*>::const_iterator deleteItr = _vecfind_ptr_itr<gNC::gLINK*>(eraseVec, LINK_toDelete);
        eraseVec.erase(deleteItr);
    }
    if(LINK_toDelete->dest!=nullptr) {
        std::vector<gNC::gLINK*>& eraseVec = (LINK_toDelete->type_dest==0? LINK_toDelete->dest->ln_in : LINK_toDelete->dest->ln_add);
        std::vector<gNC::gLINK*>::const_iterator deleteItr = _vecfind_ptr_itr<gNC::gLINK*>(eraseVec, LINK_toDelete);
        eraseVec.erase(deleteItr);
    }

    this->_links.erase(linkItr);

    return 0;
}



bool _draw__node_cosmetics(
    std::list<gNC::gNODE>::iterator itr,
    ImVec2 nodePos,
    ImDrawList* win_draw_list
) {
    bool result = true;
    static std::vector<int> buttons(4, 0);

    ImDrawList* local_drawList = ImGui::GetWindowDrawList();

    // std::cout << "-";
    for(size_t i=0; i<4; i++) {
        ImVec2 tempPos = (
            i==0? ImVec2((*itr).pos_in[0], (*itr).pos_in[1]) :
            (i==1? ImVec2((*itr).pos_out[0], (*itr).pos_out[1]) :
            (i==2? ImVec2((*itr).pos_add_0[0], (*itr).pos_add_0[1]) :
            ImVec2((*itr).pos_share_0[0], (*itr).pos_share_0[1])))
        );


        tempPos.x += nodePos.x;
        tempPos.y += nodePos.y;
        local_drawList->AddCircleFilled(tempPos, 7, IM_COL32(100, 100, 100, 255), 50);
        win_draw_list->AddCircleFilled( tempPos, 7, IM_COL32(100, 100, 100, 255), 50);

        // ImGui::SetCursorPos(tempPos);
        // ImGui::PushID(i);
        // ImGui::RadioButton("", &buttons[i], i);
        // ImGui::PopID();


        if(i==2) {
            tempPos = ImVec2((*itr).pos_add_1[0], (*itr).pos_add_1[1]);

        }
        switch (i) {
        case 2: tempPos = ImVec2((*itr).pos_add_1[0],   (*itr).pos_add_1[1]);
        case 3: if(i==3) { tempPos = ImVec2((*itr).pos_share_1[0], (*itr).pos_share_1[1]); }
        case 69:
            tempPos.x += nodePos.x;
            tempPos.y += nodePos.y;
            local_drawList->AddCircleFilled(tempPos, 7, IM_COL32(100, 100, 100, 255), 50);
            win_draw_list->AddCircleFilled( tempPos, 7, IM_COL32(100, 100, 100, 255), 50);

            // ImGui::SetCursorPos(tempPos);
            // ImGui::RadioButton("", &buttons[i], i*2);
            // ImGui::PopID();

            // win_draw_list->AddCircleFilled( tempPos, 5, IM_COL32(100, 100, 100, 255), 50);
            // local_drawList->AddCircleFilled(tempPos, 5, IM_COL32(100, 100, 100, 255), 50);
            break;
        default:
            break;
        }

        // std::cout << i<<" ";

    }
    // std::cout << std::endl;

    return result;
}


extern bool IsLegacyNativeDupe(ImGuiKey key);
extern std::vector<int>* update_keys(
    std::vector<int>* ptr_pressed_key = nullptr,
    size_t* ptr_num_keys_pressed = nullptr
);
extern bool isKeyPressed(int keyID, std::vector<int>* pressed_keys);

int gNC::guiNodeChart::draw() {
    static bool local_init = false;
    static std::vector<int>* pressed_keys;

    if(!local_init) pressed_keys = update_keys();


    ImGuiIO& io = ImGui::GetIO(); //(void)io;


    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    for(auto itr=_nodes.begin(); itr!=this->_nodes.end(); ++itr) {
        if(
            ((*itr).pos[0] + (*itr).width  + screen_pos[0] < 0 || (*itr).pos[0] + screen_pos[0] > screen_dim[0]) ||
            ((*itr).pos[1] + (*itr).height + screen_pos[1] < 0 || (*itr).pos[1] + screen_pos[1] > screen_dim[1])
        ) continue;

        ImVec2 nodePos = ImVec2((*itr).pos[0] + screen_pos[0], (*itr).pos[1] + screen_pos[1]);


        ImGuiWindowFlags win_flags = 0;

        win_flags |= ImGuiWindowFlags_NoResize;
        win_flags |= ImGuiWindowFlags_NoCollapse;
        if(local_init) win_flags |= ImGuiWindowFlags_NoFocusOnAppearing;


        ImGui::Begin((*itr).addr.c_str(), NULL, win_flags);
        ImGui::SetWindowSize(ImVec2(((*itr).width), (*itr).height));


        if(!(*itr).init) {
            ImGui::SetWindowPos(ImVec2((*itr).pos[0], (*itr).pos[1]));
            (*itr).init = true;
        }


        if(ImGui::IsWindowFocused()) {
            if(pressed_keys->size() > 0 && USEFUL::searchVec<int>(*pressed_keys, 655) != -1) {
                (*itr).pos[0] += io.MouseDelta.x;
                (*itr).pos[1] += io.MouseDelta.y;
            }
        }
        else {
            ImGui::SetWindowPos(nodePos);
        }


        _draw__node_cosmetics(itr, nodePos, draw_list);

        
        ImGui::End();

    }

    if(!local_init) local_init = true;
    return 0;
}

int gNC::guiNodeChart::save(
    std::string filename,
    bool overwrite
) {


    return 0;
}

template<typename addrType>
std::string ptrToStr(addrType toConv) {
    const void *address = static_cast<const void*>(toConv);
    std::stringstream ss;
    ss << address;
    return ss.str();
}




// /**
//  * @brief Search and find the vector index position of a certain value
//  * 
//  * @tparam T -data type of elements to look through
//  * @param vec vector to search through
//  * @param toFind value to find in the vector
//  * @return int index of where on `vec` the given `toFind` value exists.
//  * @note if the value is not found in the vector then the function will return -1
//  */
// template<class T> int searchVec(std::vector<T> vec, T toFind) {
//     typename std::vector<T>::iterator idx = find(vec.begin(), vec.end(), toFind);
//     if(idx!=vec.end()) return idx-vec.begin();
//     else return -1;
//     // int idx = -1;
//     // for(size_t i=0; i<vec.size(); i++) {
//     // 	if(vec.at(i)==toFind) {
//     //     	idx=i;
//     //         break;
//     //     }
//     // }
//     // return idx;
// }
