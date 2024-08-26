

#include "globals_includes.hpp"

/**
 * Get the relative position of a node's connection point from the connection id
 * 
 * ### Parameters:
 * `connectionID` integer identifier for each of the connection points:
 *  - `0` - `pos_in`
 *  - `1` - `pos_out`
 *  - `2` - `pos_add_0`
 *  - `3` - `pos_share_0`
 *  - `4` - `pos_add_1`
 *  - `5` - `pos_share_1`
 * ### Return:
 *  `ImVec2` of the relative coordinate
 */
ImVec2 gNC::gNODE::getConnectionPos(int connectionID) {

    switch (connectionID) {
    case 0: return pos_in;      break;
    case 1: return pos_out;     break;
    case 2: return pos_add_0;   break;
    case 3: return pos_share_0; break;
    case 4: return pos_add_1;   break;
    case 5: return pos_share_1; break;
    default:
        std::runtime_error("ERROR: gNC::gNODE::getConnectionPos(): connectionID is incorrect");
    }
    return ImVec2();
}

void gNC::gNODE::draw_connection(
    std::vector<ImDrawList*> draw_win,
    ImVec2 nodePos
) {
    static ImU32 colour_bg      = IM_COL32(100, 100, 100, 255);
    static ImU32 colour_hover   = IM_COL32(100, 100, 100, 160);
    static ImU32 colour_press   = IM_COL32(150, 150, 150, 160);
    static ImU32 colour_border  = IM_COL32(200, 200, 200, 255);

    for(int i=0; i<6; i++) {
        ImVec2 tempPos = getConnectionPos(i);
        tempPos.x += nodePos.x;
        tempPos.y += nodePos.y;
            
        // (
        // i==0?   ImVec2(pos_in[0]     + nodePos.x, pos_in[1]     + nodePos.y) :
        // (i==1?  ImVec2(pos_out[0]    + nodePos.x, pos_out[1]    + nodePos.y) :
        // (i==2?  ImVec2(pos_add_0[0]  + nodePos.x, pos_add_0[1]  + nodePos.y) :
        // (i==3?  ImVec2(pos_share_0[0]+ nodePos.x, pos_share_0[1]+ nodePos.y) :
        // (i==4?  ImVec2(pos_add_1[0]  + nodePos.x, pos_add_1[1]  + nodePos.y) :
        //         ImVec2(pos_share_1[0]+ nodePos.x, pos_share_1[1]+ nodePos.y)))))
        // );

        for(ImDrawList* el: draw_win) {
            el->AddCircleFilled(tempPos, ROI_attach[0]*0.5, colour_bg, 50);
            el->AddCircle(      tempPos, ROI_attach[0]*0.5, colour_border, 50, 3);

            switch (state_connections[i]) {
            case 1: el->AddCircleFilled(tempPos, ROI_attach[0]*0.5, colour_hover, 50); break;
            case 2: el->AddCircleFilled(tempPos, ROI_attach[0]*0.3, colour_press, 50); break;
            default:
                break;
            }

        }
    }

}

/**
 * @brief move the coordinates for the points that make up LINK draw
 * 
 * @param par_pos_src absolute 2d position of _src point
 * @param par_pos_dest absolute 2d position of _dest point
 * @param par_pos_s1 absolute 2d position of _s1 point
 * @param par_pos_d1 absolute 2d position of _d1 point
 */
void gNC::gLINK::move_link(
    ImVec2 par_pos_src,
    ImVec2 par_pos_dest,
    ImVec2 par_pos_s1,
    ImVec2 par_pos_d1
) {
    /**
     * If value is -2, that value is kept.
     * If value is -1, that value allowed to change (applies only to s1 and d1)
     * else: update it
     */

    // assert(
    //     !(par_pos_src.x  < 0 && par_pos_src.x  != -1 && par_pos_src.x  != -2) ||
    //     !(par_pos_src.y  < 0 && par_pos_src.y  != -1 && par_pos_src.y  != -2) ||
    //     !(par_pos_dest.x < 0 && par_pos_dest.x != -1 && par_pos_dest.x != -2) ||
    //     !(par_pos_dest.y < 0 && par_pos_dest.y != -1 && par_pos_dest.y != -2)
    // );

    if(par_pos_src.x  != -2) Pos_src.x  = par_pos_src.x;
    if(par_pos_src.y  != -2) Pos_src.y  = par_pos_src.y;
    if(par_pos_dest.x != -2) Pos_dest.x = par_pos_dest.x;
    if(par_pos_dest.y != -2) Pos_dest.y = par_pos_dest.y;

    ImVec2 pos_delta = ImVec2(Pos_dest.x - Pos_src.x, Pos_dest.y - Pos_src.y);
    ImVec2 pos_middle= ImVec2(Pos_src.x + pos_delta.x/2, Pos_src.y + pos_delta.y/2);
    this->Pos_center = pos_middle;

    for(int i=0; i<2; i++) {
        if(par_pos_s1[i]==-2) {}
        else if(par_pos_s1[i]==-1) {
            if(type_src==1) {
                if(layout==0 || layout==1) Pos_s1[i] = (i==0? pos_middle[i]-pos_delta[i]/4 : Pos_src[i]);
                else Pos_s1[i] = (i==1? pos_middle[i]-pos_delta[i]/4 : Pos_src[i]);
            }
            else Pos_s1[i] = Pos_src[i];
            // Pos_s1[i] = pos_middle[i] - pos_delta[i]/4;
        }
        else Pos_s1[i] = par_pos_s1[i];
        if(par_pos_d1[i]==-2) {}
        else if(par_pos_d1[i]==-1) {
            if(type_dest==0) {
                if(layout==0 || layout==1) Pos_d1[i] = (i==0? pos_middle[i]+pos_delta[i]/4 : Pos_dest[i]);
                else Pos_d1[i] = (i==1? pos_middle[i]-pos_delta[i]/4 : Pos_dest[i]);
            }
            else Pos_d1[i] = Pos_dest[i];
            // Pos_d1[i] = pos_middle[i] + pos_delta[i]/4;
        }
        else Pos_d1[i] = par_pos_d1[i];
    }


}
void gNC::gLINK::draw_link(
    std::vector<ImDrawList*> draw_win,
    ImVec2 screen_offset
) {
    /**
     * Need to find a way for a link between two nodes to be highlighted when
     *  - this link is clicked/focused
     *  - either node of this link is clicked/focused
     * 
     * when drawing the line on the node, only draw to the first intermediary point.
     */
    static ImU32 colour_bg      = IM_COL32(250, 241,  58, 204);
    static ImU32 colour_border  = IM_COL32(100, 100, 100, 255); //IM_COL32(102,  99, 28, 204);


    auto addOffs = [screen_offset](ImVec2 toAdd) {
        return ImVec2(toAdd.x + screen_offset.x, toAdd.y + screen_offset.y);
    };

    int bezierSegs = 10;


    pos2d point_C_src, point_C_dest;
    if(layout==0 || layout==1) {
        if(type_src==1) point_C_src = pos2d(Pos_center.x, Pos_s1.y);
        else            point_C_src = pos2d(Pos_s1.x, Pos_center.y);
        if(type_dest==0)point_C_dest= pos2d(Pos_center.x, Pos_d1.y);
        else            point_C_dest= pos2d(Pos_d1.x, Pos_center.y);
    }
    else {
        if(type_src==1) point_C_src = pos2d(Pos_s1.x, Pos_center.y);
        else            point_C_src = pos2d(Pos_center.x, Pos_s1.y);
        if(type_dest==0)point_C_dest= pos2d(Pos_d1.x, Pos_center.y);
        else            point_C_dest= pos2d(Pos_center.x, Pos_d1.y);
    }


    std::vector<pos2d> curveSrc = quadratic_bezier(
        to_pos2d(Pos_s1), to_pos2d(Pos_center),
        point_C_src,
        // ((layout==0 || layout==1)? pos2d(Pos_center.x, Pos_s1.y) : pos2d(Pos_s1.x, Pos_center.y)),
        bezierSegs
    );
    std::vector<pos2d> curveDest = quadratic_bezier(
        to_pos2d(Pos_d1), to_pos2d(Pos_center),
        point_C_dest,
        // ((layout==0 || layout==1)? pos2d(Pos_center.x, Pos_d1.y) : pos2d(Pos_d1.x, Pos_center.y)),
        bezierSegs
    );


    for(ImDrawList* el: draw_win) {

        el->AddLine(addOffs(Pos_src),  addOffs(Pos_s1), colour_border, 12);
        el->AddLine(addOffs(Pos_dest), addOffs(Pos_d1), colour_border, 12);

        // el->AddLine(addOffs(Pos_s1),   addOffs(Pos_d1), colour_border, 10);
        for(size_t i=0; i<curveSrc.size()-1; i++) {
            // el->AddCircle(addOffs(to_ImVec2(curveSrc[i])), 5, colour_bg, 10, 2);
            // el->AddLine(to_ImVec2(curveTest[i]), to_ImVec2(curveTest[i+1]), IM_COL32(70, 140, 210, 255), 10);
            el->AddLine(addOffs(to_ImVec2(curveSrc[i])),  addOffs(to_ImVec2(curveSrc[i+1])),  colour_border, 12);
            el->AddLine(addOffs(to_ImVec2(curveDest[i])), addOffs(to_ImVec2(curveDest[i+1])), colour_border, 12);
            // el->AddCircle(addOffs(to_ImVec2(curveSrc[i])), 10, colour_border, 10);
            // std::cout << curveSrc[i].getStr() << std::endl;
        }
        // std::cout << "-----"<<std::endl;
        // el->AddLine(addOffs(to_ImVec2(curveSrc[bezierSegs-1])), addOffs(Pos_center), colour_border, 10);
        // el->AddCircle(addOffs(Pos_center), 10, colour_border, 10);
        // el->AddCircleFilled(addOffs(to_ImVec2(curveSrc.back())), 20, IM_COL32(255, 0, 0, 255), 10);
        // std::cout << (&curveSrc.back())->getStr() << std::endl;
        // std::cout << "---------" << std::endl;


        el->AddLine(addOffs(Pos_src),  addOffs(Pos_s1), colour_bg, 8);
        el->AddLine(addOffs(Pos_dest), addOffs(Pos_d1), colour_bg, 8);

        for(size_t i=0; i<curveSrc.size()-1; i++) {
            el->AddLine(addOffs(to_ImVec2(curveSrc[i])),  addOffs(to_ImVec2(curveSrc[i+1])),  colour_bg, 8);
            el->AddLine(addOffs(to_ImVec2(curveDest[i])), addOffs(to_ImVec2(curveDest[i+1])), colour_bg, 8);
        }
        // el->AddLine(addOffs(Pos_s1),   addOffs(Pos_d1), colour_bg, 8);
    }

}


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

gNC::gNODE& gNC::guiNodeChart::operator[](size_t i) const {
    if(i>=this->_nodes.size()) std::runtime_error("ERROR: "+_info_name+"operator[](size_t) index is out of range");

    std::list<gNC::gNODE>::const_iterator itr = _nodes.begin();
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


int gNC::guiNodeChart::NODE_move(
    gNC::gNODE* NODE_toMove,
    float new_X,
    float new_Y,
    int moveMode
) {
    auto moveItr = this->_find_ptr_itr<gNC::gNODE>(this->_nodes, NODE_toMove);
    if(moveItr==this->_nodes.end()) std::runtime_error(
        "ERROR: "+_info_name+"NODE_move(gNC::gNODE*, float, float)"+
        " arg for gNC::gNODE address is invalid"
    );


    if(moveMode==0) {
        NODE_toMove->pos[0] = new_X;
        NODE_toMove->pos[1] = new_Y;
    }
    else if(moveMode==1) {
        NODE_toMove->pos[0] += new_X;
        NODE_toMove->pos[1] += new_Y;
    }
    else if(moveMode==-1) {}

    for(int v=0; v<4; v++) {
        std::vector<gNC::gLINK*>& checkVec = (
            v==0?  NODE_toMove->ln_in :
            (v==1? NODE_toMove->ln_out:
            (v==2? NODE_toMove->ln_add :
                   NODE_toMove->ln_share
            ))
        );
        ImVec2 connectPos1 = NODE_toMove->getConnectionPos(v);
        ImVec2 connectPos2 = (v==2 || v==3? NODE_toMove->getConnectionPos(v+2) : connectPos1);
        
        for(int n=0; n<2; n++) {
            connectPos1[n]+=NODE_toMove->pos[n];
            connectPos2[n]+=NODE_toMove->pos[n];
        }

        for(gNC::gLINK* lnk: checkVec) {
            float delta2, delta1;
            delta2 = getNDimDistance<ImVec2>(2, connectPos2, (v==0||v==2? lnk->Pos_src : lnk->Pos_dest));
            delta1 = getNDimDistance<ImVec2>(2, connectPos1, (v==0||v==2? lnk->Pos_src : lnk->Pos_dest));
            // std::cout << "["<<delta1<<", "<<delta2<<"] ";
            ImVec2& connectPos = (delta1 < delta2? connectPos1 : connectPos2);

            if(v==0 || v==2) { //in
                lnk->move_link(ImVec2(-2, -2), connectPos);
                if(moveMode!=-1) NODE_move(lnk->src,  0, 0, -1);
            }
            else if(v==1 || v==3) { //out
                lnk->move_link(connectPos, ImVec2(-2, -2) );
                if(moveMode!=-1) NODE_move(lnk->dest, 0, 0, -1);
            }
        }
    }


    return 0;
}

gNC::gLINK* gNC::guiNodeChart::LINK_create(
    size_t NODE_src_idx,
    size_t NODE_dest_idx,
    int type_src,
    int type_dest,
    std::string label,
    std::string desc,
    ImVec2 pos_interm_src,
    ImVec2 pos_interm_dest
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
    std::string desc,
    ImVec2 pos_interm_src,
    ImVec2 pos_interm_dest
) {
    std::list<gNC::gNODE>::const_iterator
        checkItr_src    = this->_find_ptr_itr<gNC::gNODE>(this->_nodes, NODE_src),
        checkItr_dest   = this->_find_ptr_itr<gNC::gNODE>(this->_nodes, NODE_dest);

    if(checkItr_src==this->_nodes.end() || checkItr_dest==this->_nodes.end()) {
        std::runtime_error(
            "ERROR: "+this->_info_name+"LINK_create(gNC::gNODE*, gNC::gNODE*, int, int, std::string, std::string):"+
            " arg(s) for node address(es) does not exist in stored nodes."
        );
    }
    if(searchVec<int>(std::vector<int>{1, 3, 5},type_src)==-1 && searchVec<int>(std::vector<int>{0, 2, 4},type_dest)==-1) {
        std::runtime_error(
            "ERROR: "+this->_info_name+"LINK_create(gNC::gNODE*, gNC::gNODE*, int, int, std::string, std::string):"+
            " arg(s) for type_{..} is/are not valid."
        );
    }

    this->_links.push_back(gNC::gLINK(type_src, type_dest, NODE_src, NODE_dest, label, desc));
    this->_lastAddedLink = &(this->_links.back());
    this->_lastAddedLink->addr = ptrToStr<gNC::gLINK*>(this->_lastAddedLink);

    if(type_src==1) _lastAddedLink->src->ln_out.push_back(_lastAddedLink);
    else            _lastAddedLink->src->ln_share.push_back(_lastAddedLink);

    if(type_dest==0)_lastAddedLink->dest->ln_in.push_back(_lastAddedLink);
    else            _lastAddedLink->dest->ln_add.push_back(_lastAddedLink);


    ImVec2 srcPos = NODE_src->getConnectionPos(type_src);
    ImVec2 destPos = NODE_dest->getConnectionPos(type_dest);

    // _lastAddedLink->Pos_src = ImVec2(NODE_src->pos[0]+srcPos.x, NODE_src->pos[1]+srcPos.y);
    // _lastAddedLink->Pos_dest = ImVec2(NODE_dest->pos[0]+destPos.x, NODE_dest->pos[1]+destPos.y);
    // ImVec2 pos_delta = ImVec2(_lastAddedLink->Pos_dest.x - _lastAddedLink->Pos_src.x, _lastAddedLink->Pos_dest.y - _lastAddedLink->Pos_src.y);
    // ImVec2 halfWayPoint = ImVec2(_lastAddedLink->Pos_src.x + pos_delta.x/2, _lastAddedLink->Pos_src.y + pos_delta.y/2);
    // _lastAddedLink->Pos_s1 = pos_interm_src;
    // _lastAddedLink->Pos_d1 = pos_interm_dest;
    // if(pos_interm_src.x==-1) _lastAddedLink->Pos_s1.x =  halfWayPoint.x - pos_delta.x/4;
    // if(pos_interm_src.y==-1) _lastAddedLink->Pos_s1.y =  halfWayPoint.y - pos_delta.y/4;
    // if(pos_interm_dest.x==-1) _lastAddedLink->Pos_d1.x =  halfWayPoint.x + pos_delta.x/4;
    // if(pos_interm_dest.y==-1) _lastAddedLink->Pos_d1.y =  halfWayPoint.y + pos_delta.y/4;
    // if(type_src==3 || type_src==5)  _lastAddedLink->Pos_s1 = _lastAddedLink->Pos_src;   //type_src is `share`
    // if(type_dest==2 || type_dest==4)_lastAddedLink->Pos_d1 = _lastAddedLink->Pos_dest;  //type_dest is `add`


    _lastAddedLink->move_link(
        ImVec2(NODE_src->pos[0]+srcPos.x,   NODE_src->pos[1]+srcPos.y),
        ImVec2(NODE_dest->pos[0]+destPos.x, NODE_dest->pos[1]+destPos.y),
        (type_src!=1?  ImVec2(NODE_src->pos[0]+srcPos.x,   NODE_src->pos[1]+srcPos.y)   : ImVec2(-1, -1)),
        (type_dest!=0? ImVec2(NODE_dest->pos[0]+destPos.x, NODE_dest->pos[1]+destPos.y) : ImVec2(-1, -1))
    );

    return this->_lastAddedLink;
}
gNC::gLINK* gNC::guiNodeChart::LINK_create_loose(
    ImVec2 loosePos,
    gNC::gNODE* _NODE,
    int type_NODE_connection,
    std::string label,
    std::string desc,
    ImVec2 pos_interm_NODE
) {
    std::list<gNC::gNODE>::const_iterator checkItr = this->_find_ptr_itr<gNC::gNODE>(this->_nodes, _NODE);

    if(checkItr==this->_nodes.end()) {
        std::runtime_error(
            "ERROR: "+this->_info_name+"LINK_create_loose(..):"+
            " arg for _NODE address is incorreect."
        );
    }
    if(searchVec<int>(std::vector<int>{1,2,3,4,5},type_NODE_connection)==-1) {
        std::runtime_error(
            "ERROR: "+this->_info_name+"LINK_create_loose(...):"+
            " arg for `type_NODE_connection`"
        );
    }

    int nType = 0; //`1`-src; `0`-dest
    if(searchVec<int>(std::vector<int>{1,3,5}, type_NODE_connection)!=-1) { //node is src (out, share)
        this->_links.push_back(gNC::gLINK(type_NODE_connection, 0, _NODE, nullptr, label, desc));
        nType = 1;
    }
    else { //node is dest (in, add)
        this->_links.push_back(gNC::gLINK(1, type_NODE_connection, nullptr, _NODE, label, desc));
        nType = 0;
    }

    this->_lastAddedLink = &(this->_links.back());
    this->_lastAddedLink->addr = ptrToStr<gNC::gLINK*>(this->_lastAddedLink);

    std::vector<gLINK*>& refVec = (
        type_NODE_connection==0?  _lastAddedLink->dest->ln_in :
        (type_NODE_connection==1? _lastAddedLink->src->ln_out :
        ((type_NODE_connection==2 || type_NODE_connection==4)? _lastAddedLink->dest->ln_add :
        _lastAddedLink->dest->ln_share))
    );
    refVec.push_back(_lastAddedLink);

    ImVec2 connecPos = _NODE->getConnectionPos(type_NODE_connection);

    if(nType==1)  {
        _lastAddedLink->Pos_src = ImVec2(_NODE->pos[0]+connecPos.x, _NODE->pos[1]+connecPos.y);
        _lastAddedLink->Pos_dest= loosePos;
        _lastAddedLink->Pos_s1  = pos_interm_NODE;
    }
    else {
        _lastAddedLink->Pos_dest= ImVec2(_NODE->pos[0]+connecPos.x, _NODE->pos[1]+connecPos.y);
        _lastAddedLink->Pos_src = loosePos;
        _lastAddedLink->Pos_d1  = pos_interm_NODE;
    }


    ImVec2 pos_delta = ImVec2(_lastAddedLink->Pos_dest.x - _lastAddedLink->Pos_src.x, _lastAddedLink->Pos_dest.y - _lastAddedLink->Pos_src.y);
    ImVec2 halfWayPoint = ImVec2(_lastAddedLink->Pos_src.x + pos_delta.x/2, _lastAddedLink->Pos_src.y + pos_delta.y/2);


    if(nType==1) {
        _lastAddedLink->Pos_d1  = ImVec2(halfWayPoint.x - pos_delta.x/4, halfWayPoint.y - pos_delta.y/4);

        if(pos_interm_NODE.x==-1) _lastAddedLink->Pos_s1.x =  halfWayPoint.x - pos_delta.x/4;
        if(pos_interm_NODE.y==-1) _lastAddedLink->Pos_s1.y =  halfWayPoint.y - pos_delta.y/4;
    }
    if(nType==0) {
        _lastAddedLink->Pos_s1  = ImVec2(halfWayPoint.x - pos_delta.x/4, halfWayPoint.y - pos_delta.y/4);

        if(pos_interm_NODE.x==-1) _lastAddedLink->Pos_d1.x =  halfWayPoint.x + pos_delta.x/4;
        if(pos_interm_NODE.y==-1) _lastAddedLink->Pos_d1.y =  halfWayPoint.y + pos_delta.y/4;
    }

    
    return this->_lastAddedLink;
}

int gNC::guiNodeChart::LINK_swapSrc(gNC::gLINK* toSwap, gNC::gNODE* newSrc, int srcType) {
    if(searchVec<int>(std::vector<int>{1,3,5}, srcType)==-1)
        std::runtime_error("ERROR: "+this->_info_name+"LINK_swapSrc(gNC::gLINK*, gNC::gNODE*, int): invalid `srcType` input");
    if(_find_ptr_itr<gNC::gNODE>(_nodes, newSrc)==_nodes.end())
        std::runtime_error("ERROR: "+this->_info_name+"LINK_swapSrc(gNC::gLINK*, gNC::gNODE*, int): arg for `newSrc` is not a valid `gNC::gNODE` address");
    else if(_find_ptr_itr<gNC::gLINK>(_links, toSwap)==_links.end())
        std::runtime_error("ERROR: "+this->_info_name+"LINK_swapSrc(gNC::gLINK*, gNC::gNODE*, int): arg for `toSwap` is not a valid `gNC::gLINK` address");

    if(toSwap->src==newSrc) return 1;

    if(toSwap->src!=nullptr) {
        //locate and erase this link for the src NODE to swap from
        std::vector<gNC::gLINK*>& eraseVec = (toSwap->type_src==1? toSwap->src->ln_out : toSwap->src->ln_share);
        std::vector<gNC::gLINK*>::const_iterator deleteItr = _vecfind_ptr_itr<gNC::gLINK*>(eraseVec, toSwap);
        eraseVec.erase(deleteItr);
    }

    toSwap->src = newSrc;

    if(srcType==1) newSrc->ln_out.push_back(toSwap);
    else           newSrc->ln_share.push_back(toSwap);

    ImVec2 srcPos = newSrc->getConnectionPos(srcType);
    toSwap->move_link(
        ImVec2(newSrc->pos[0]+srcPos.x, newSrc->pos[1]+srcPos.y),
        ImVec2(-2, -2), ImVec2(-1, -1), ImVec2(-1, -1)
    );

    return 0;
}
int gNC::guiNodeChart::LINK_swapDest(gNC::gLINK* toSwap, gNC::gNODE* newDest, int destType) {
    if(searchVec<int>(std::vector<int>{0,2,4}, destType)==-1)
        std::runtime_error("ERROR: "+this->_info_name+"LINK_swapDest(gNC::gLINK*, gNC::gNODE*, int): invalid `destType` input");
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
    else            newDest->ln_add.push_back(toSwap);


    ImVec2 destPos = newDest->getConnectionPos(destType);
    toSwap->move_link(
        ImVec2(-2, -2),
        ImVec2(newDest->pos[0]+destPos.x, newDest->pos[1]+destPos.y),
        ImVec2(-1, -1), ImVec2(-1, -1)
    );

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


    (*itr).draw_connection(std::vector<ImDrawList*>{local_drawList, win_draw_list}, nodePos);

    return result;
}


int _draw_NODEcheckConnects(
    std::list<gNC::gNODE>::iterator _node,
    ImVec2 nodePos
) {
    ImGuiIO& io = ImGui::GetIO();
    int result = -1;

    ImVec2 attachROI = (*_node).ROI_attach;

    for(int i=0; i<6; i++) {
        ImVec2 tempPos = (
            i==0? (*_node).pos_in :
            (i==1? (*_node).pos_out :
            (i==2? (*_node).pos_add_0 :
            (i==3? (*_node).pos_share_0 :
            (i==4? (*_node).pos_add_1 :
            (*_node).pos_share_1))))
        );
        if(inRegion(
            io.MousePos,
            ImVec2(nodePos.x + tempPos.x - attachROI.x/2, nodePos.y + tempPos.y - attachROI.y/2),
            ImVec2(nodePos.x + tempPos.x + attachROI.x/2, nodePos.y + tempPos.y + attachROI.y/2)
        )) {
            result = i;
            break;
        }
    }


    return result;
}

int gNC::guiNodeChart::draw() {
    static bool local_init = false;
    static std::vector<int>* pressed_keys;

    if(!local_init) pressed_keys = update_keys();


    ImGuiIO& io = ImGui::GetIO(); //(void)io;


    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    #if _DEBUG
    std::cout<< "{"<<std::setw(2)<< mouseAction<<","<<std::setw(3)<<mouseClick_left<<"} ";
    #endif



    for(auto itr=_links.begin(); itr!=this->_links.end(); ++itr) {

        if(
            ((*itr).Pos_src.x < 0 && (*itr).Pos_dest.x < 0) || ((*itr).Pos_src.y < 0 && (*itr).Pos_dest.y < 0) ||
            ((*itr).Pos_src.x > screen_dim[0] && (*itr).Pos_dest.x > screen_dim[0]) || ((*itr).Pos_src.y > screen_dim[1] && (*itr).Pos_dest.y > screen_dim[1])
        ) continue;

        (*itr).draw_link(std::vector<ImDrawList*>{draw_list}, ImVec2(screen_pos[0], screen_pos[1]));
    }
    for(auto itr=_nodes.begin(); itr!=this->_nodes.end(); ++itr) {
        ImVec2 nodePos = ImVec2((*itr).pos[0] + screen_pos[0], (*itr).pos[1] + screen_pos[1]);

        if(
            (nodePos.x + (*itr).width  < 0  || nodePos.x > screen_dim[0]) ||
            (nodePos.y + (*itr).height < 0  || nodePos.y > screen_dim[1])
        ) continue;


        ImGuiWindowFlags win_flags = 0;
        win_flags |= ImGuiWindowFlags_NoResize;
        win_flags |= ImGuiWindowFlags_NoCollapse;
        win_flags |= ImGuiWindowFlags_NoFocusOnAppearing;
        win_flags |= ImGuiWindowFlags_NoMove;
        

        int node_connect = _draw_NODEcheckConnects(itr, nodePos);

        if(isKeyPressed(655, pressed_keys)) {
            #if _DEBUG
            std::cout << "Mouse left pressed: ";
            std::cout << std::setw(2) << node_connect << " ";
            #endif
            if(node_connect==-1) {
                if(inRegion(
                    io.MousePos,
                    nodePos,
                    ImVec2(nodePos.x+(*itr).width, nodePos.y+(*itr).height)
                )) {
                    #if _DEBUG
                    std::cout << "in region ";
                    #endif
                    if(mouseAction==1 || mouseAction==-1) {
                        lockMove_node = false;
                        mouseAction = 1;
                        mouseClick_left = 100;
                        #if _DEBUG
                        std::cout << " [node] ";
                        #endif
                    }
                }
            }
            else if(node_connect!=-1) {
                if(mouseAction==2 || mouseAction==-1) {
                    (*itr).state_connections[node_connect] = 2;
                    #if _DEBUG
                    switch (node_connect) {
                    case 0: std::cout<<"["<<(*itr).addr<<" _in       ]"; break;
                    case 1: std::cout<<"["<<(*itr).addr<<" _out      ]"; break;
                    case 2: std::cout<<"["<<(*itr).addr<<" _add_0    ]"; break;
                    case 3: std::cout<<"["<<(*itr).addr<<" _share_0  ]"; break;
                    case 4: std::cout<<"["<<(*itr).addr<<" _add_1    ]"; break;
                    case 5: std::cout<<"["<<(*itr).addr<<" _share_1  ]"; break;
                    default:
                        break;
                    }
                    #endif
                    mouseAction = 2;
                    mouseClick_left = 100;
                }
            }
        }
        else {
            if(node_connect!=-1) {
                (*itr).state_connections[node_connect] = 1;
            }
            else if(mouseAction!=2) {
                for(int i=0; i<6; i++) if((*itr).state_connections[i]!=0) (*itr).state_connections[i]=0;
            }
        }


        ImGui::Begin((*itr).addr.c_str(), NULL, win_flags);
        ImGui::SetWindowSize(ImVec2(((*itr).width), (*itr).height));


        if(!(*itr).init) {
            ImGui::SetWindowPos(ImVec2((*itr).pos[0], (*itr).pos[1]));
            (*itr).init = true;
        }


        if(ImGui::IsWindowFocused()) {
            if(!lockMove_node && pressed_keys->size() > 0 && searchVec<int>(*pressed_keys, 655) != -1) {
                NODE_move(&(*itr), io.MouseDelta.x, io.MouseDelta.y, 1);
                ImGui::SetWindowPos(ImVec2((*itr).pos[0] + screen_pos[0], (*itr).pos[1] + screen_pos[1]));

            }
        }
        else {
            ImGui::SetWindowPos(nodePos);
        }
        _draw__node_cosmetics(itr, nodePos, draw_list);

        ImGui::End();
    }

    if((mouseAction==0 || mouseAction==-1) && isKeyPressed(655, pressed_keys)) {
        lockMove_screen = false;
        mouseAction = 0;
        mouseClick_left = 100;
        #if _DEBUG
        std::cout << " [screen]";
        #endif
    }
    if(mouseAction!=-1) {
        if(mouseClick_left>0) {
            if(!isKeyPressed(655, pressed_keys)) mouseClick_left -= mouseTimer_decay;
        }
        else {
            mouseClick_left = 0;
            mouseAction = -1;
        }
    }

    if(mouseAction!=0 || mouseAction==2) lockMove_screen  = true;
    if(mouseAction!=1 || mouseAction==2) lockMove_node    = true;


    if(!local_init) local_init = true;
    return 0;
}

int gNC::guiNodeChart::save(
    std::string filename,
    bool overwrite
) {


    return 0;
}

