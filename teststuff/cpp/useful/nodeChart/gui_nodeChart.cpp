
#include "gui_nodeChart.hpp"

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_allegro5.h>



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
    float width,
    float height,
    std::string label,
    std::string desc,
    std::string bodyText
) {

    this->_nodes.push_back(gNC::gNODE(
        pos_x, pos_y, label, desc, bodyText
    ));

    this->_lastAddedNode = &(this->_nodes.back());
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



int gNC::guiNodeChart::draw() {
    

    return 0;
}

int gNC::guiNodeChart::save(
    std::string filename,
    bool overwrite
) {

}
