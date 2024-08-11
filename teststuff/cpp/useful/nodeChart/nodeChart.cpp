
#include "nodeChart.hpp"

template<typename storedType>
int NC::NodeChart::_find_ptr_idx(const std::list<storedType>& toCheck, storedType* ptr_toFind) {
    // std::list<storedType>::const_iterator itr;
    int found = -1;
    for(auto itr=toCheck.begin(); itr!=toCheck.end(); ++itr) {
        found++;
        if(&*itr==ptr_toFind) {
            return found;
        }
    }
    return found;
}
template<typename storedType>
auto NC::NodeChart::_find_ptr_itr(const std::list<storedType>& toCheck, storedType* ptr_toFind) {
    // std::list<storedType>::const_iterator itr;
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
int NC::NodeChart::_vecfind_ptr_idx(const std::vector<storedType>& toCheck, storedType toFind) {
    // std::vector<storedType>::const_iterator itr;
    int found = -1;
    for(auto itr=toCheck.begin(); itr!=toCheck.end(); ++itr) {
        found++;
        if((*itr)==toFind) {
            return found;
        }
    }
    return found;
}
template<typename storedType>
auto NC::NodeChart::_vecfind_ptr_itr(std::vector<storedType>& toCheck, storedType toFind) {
    // std::vector<storedType>::const_iterator itr;
    int found = -1;
    for(auto itr=toCheck.begin(); itr!=toCheck.end(); ++itr) {
        found++;
        if((*itr)==toFind) {
            return itr;
        }
    }
    return toCheck.end();
}



NC::NodeChart::NodeChart() {

}

/**
 * @brief return the size of the stored nodes or list depending on parameter.
 * 
 * @param whatList
 * - `0` size of stored NC::NODE
 * - `1` size of stored NC::LINK
 * @return size_t 
 */
size_t NC::NodeChart::size(int whatList) {
    if(whatList==0) return this->_nodes.size();
    else if(whatList==1) return this->_links.size();
    std::runtime_error("ERROR: "+this->_info_name+"size(int): input arguemnt for `whatList` does not match available options");
    return 0;
}

/**
 * @brief Return the address of the last added `NC::NODE` object
 * @note "last added" does not have to mean last element as nodes can be inserted at given pos.
 * @return `NC::NODE*` to the last added Node.
 */
NC::NODE* NC::NodeChart::lastAdded_NODE() {
    return this->_lastAddedNode;
}
/**
 * @brief Return the address of the last added `NC::LINK` object
 * @note "last added" does not have to mean last element as nodes can be inserted at given pos.
 * @return `NC::LINK*` to the last added Node.
 */
NC::LINK* NC::NodeChart::lastAdded_LINK() {
    return this->_lastAddedLink;
}


NC::NODE NC::NodeChart::operator[](size_t i) const {
    if(i>=this->_nodes.size()) std::runtime_error("ERROR: "+_info_name+"operator[](size_t) index out of size");

    auto itr = this->_nodes.begin();
    advance(itr, i);
    return const_cast<NC::NODE&>(*itr);
}

NC::NODE* NC::NodeChart::NODE_add(
    std::string label,
    std::string desc,
    std::string bodyText
) {
    // extend list of nodes with a new one
    this->_nodes.push_back(NC::NODE{label, desc, bodyText});

    // update `this->_lastAddedNode
    this->_lastAddedNode = &(this->_nodes.back());
    return this->_lastAddedNode;
}
NC::NODE* NC::NodeChart::NODE_insert(
    size_t insert_idx,
    std::string label,
    std::string desc,
    std::string bodyText
) {
    if(insert_idx > this->_nodes.size()) std::runtime_error(
        "ERROR: "+this->_info_name+"NODE_insert(size_t, std::string, std::string, std::string):"+
        " arg for `insert_idx` is bigger than available element size"
    );
    else if(insert_idx == this->_nodes.size()) std::runtime_error(
        "ERROR: "+this->_info_name+"NODE_insert(size_t, std::string, std::string, std::string):"+
        " arg for `insert_idx` is the same as the stored sizes. use \"NC::NodeChart::NODE_add\" instead"
    );

    auto itr = this->_nodes.begin();
    advance(itr, insert_idx);
    this->_nodes.insert(itr, NC::NODE{label, desc, bodyText});
    this->_lastAddedNode = &*itr;
    return this->_lastAddedNode;
}

int NC::NodeChart::NODE_delete(
    size_t NODE_idx,
    bool leaveFloating
) {
    if(NODE_idx >= this->_nodes.size()) std::runtime_error(
        "ERROR: "+this->_info_name+"NODE_delete(size_t, bool):"+
        " arg for `NODE_idx` is bigger than available element size"
    );

    NC::NODE* erasePtr;
    std::list<NC::NODE>::iterator itr = this->_nodes.begin();
    advance(itr, NODE_idx);
    return this->NODE_delete(&*itr, leaveFloating);
}
int NC::NodeChart::NODE_delete(
    NC::NODE* NODE_toDelete,
    bool leaveFloating
) {
    std::list<NC::NODE>::const_iterator eraseItr = this->_find_ptr_itr<NC::NODE>(this->_nodes, NODE_toDelete);
    if(eraseItr==this->_nodes.end()) std::runtime_error(
        "ERROR: "+this->_info_name+"NODE_delete(NC::NODE*, bool):"+
        " arg for node address does not exist in stored nodes"
    );

    if(!leaveFloating) {
        /**
         * @brief Erase all links that are shared with this Node that is to be deleted.
         *  - iterate through each NC::LINK vector in that Node
         *      - go to each link element in said vector(s) and go to the Node in either `src` or `dest` depending on if that Node isn't this Node
         *          - find (by using `type_{..}` in link) and erase the link, from previously, from that Node sourced from the link.
         * - after searching all the vectors in said NC::LINK, erase Node from this->_nodes.
         */
        
        for(size_t i=0; i<4; i++) {
            // go to each link vector
            const std::vector<NC::LINK*> linkVec = (i==0? NODE_toDelete->ln_in : (i==1? NODE_toDelete->ln_out : (i==2? NODE_toDelete->ln_add : NODE_toDelete->ln_share)));

            std::vector<std::list<NC::LINK>::const_iterator> itrToErase; //iterators of NC::LINK's to erase
            for(NC::LINK* plink: linkVec) {
                // go to each link in link vector
                if(NODE_toDelete==plink->src) {
                    std::vector<NC::LINK*>& plink_type = (plink->type_dest==0? plink->dest->ln_in : plink->dest->ln_add);
                    for(NC::LINK* pplink: plink_type) {
                        // iterate through stored links in opposing Node and find currently iterated link and erase it from the opposing Node
                        if(pplink==plink) {
                            //erase the link in the corresponding node linked to the node that's about to be deleted
                            plink_type.erase(this->_vecfind_ptr_itr<NC::LINK*>(plink_type, plink));
                        }
                    }
                }
                else if(NODE_toDelete==plink->dest) {
                    std::vector<NC::LINK*>& plink_type = (plink->type_src==0? plink->src->ln_out : plink->src->ln_share);
                    for(NC::LINK* pplink: plink_type) {
                        // iterate through stored links in opposing Node and find currently iterated link and erase it from the opposing Node
                        if(pplink==plink) plink_type.erase(this->_vecfind_ptr_itr<NC::LINK*>(plink_type, plink));
                    }
                }
                itrToErase.push_back(this->_find_ptr_itr<NC::LINK>(this->_links, plink));
                // this->_links.erase(this->_find_ptr_itr<NC::LINK>(this->_links, plink));
            }
            for(auto itrs: itrToErase) this->_links.erase(itrs);
        }
    }
    else if(leaveFloating) {
        /**
         * @brief Erase this node whilst setting this Node's address in its links to `nullptr`
         * 
         */
        
        for(size_t i=0; i<4; i++) {
            const std::vector<NC::LINK*> linkVec = (i==0? NODE_toDelete->ln_in : (i==1? NODE_toDelete->ln_out : (i==2? NODE_toDelete->ln_add : NODE_toDelete->ln_share)));

            for(NC::LINK* plink: linkVec) {
                // go to each link in this node and change this address to nullptr
                if(NODE_toDelete==plink->src)       plink->src  = nullptr;
                else if(NODE_toDelete==plink->dest) plink->dest = nullptr;
            }
        }
    }

    this->_nodes.erase(this->_find_ptr_itr<NC::NODE>(this->_nodes, NODE_toDelete));

    return 0;
}

NC::LINK* NC::NodeChart::LINK_add(
    size_t NODE_src_idx,
    size_t NODE_dest_idx,
    int type_src,
    int type_dest,
    std::string label,
    std::string desc
) {
    if(NODE_src_idx>=this->_nodes.size() || NODE_dest_idx>=this->_nodes.size())
        std::runtime_error(
            "ERROR: "+this->_info_name+"LINK_add(size_t, size_t, int, int, std::string, std::string):"+
            " arg for `{..}_idx` is bigger than available element size"
        );

    NC::NODE *ptrSrc, *ptrDest;
    std::list<NC::NODE>::iterator itr = this->_nodes.begin();
    advance(itr, NODE_src_idx);
    ptrSrc  = &*itr;
    itr = this->_nodes.begin();
    advance(itr, NODE_dest_idx);
    ptrDest = &*itr;

    return this->LINK_add(ptrSrc, ptrDest, type_src, type_dest, label, desc);
}
NC::LINK* NC::NodeChart::LINK_add(
    NC::NODE* NODE_src,
    NC::NODE* NODE_dest,
    int type_src,
    int type_dest,
    std::string label,
    std::string desc
) {
    std::list<NC::NODE>::const_iterator
        eraseItr_src    = this->_find_ptr_itr<NC::NODE>(this->_nodes, NODE_src),
        eraseItr_dest   = this->_find_ptr_itr<NC::NODE>(this->_nodes, NODE_dest);

    if(eraseItr_src==this->_nodes.end() || eraseItr_dest==this->_nodes.end())
        std::runtime_error(
            "ERROR: "+this->_info_name+"LINK_add(NC::NODE*, NC::NODE*, int, int, std::string, std::string):"+
            " arg(s) for node address(es) does not exist in stored nodes."
        );
    if(type_src!=1 && type_src!=3 && type_dest!=0 && type_dest!=2) 
        std::runtime_error(
            "ERROR: "+this->_info_name+"LINK_add(NC::NODE*, NC::NODE*, int, int, std::string, std::string):"+
            " arg(s) for type_{..} is/are not valid."
        );

    this->_links.push_back(NC::LINK(type_src,type_dest,NODE_src,NODE_dest,label,desc));
    this->_lastAddedLink = &(this->_links.back());
    return this->_lastAddedLink;
}

int NC::NodeChart::LINK_swapSrc(NC::LINK* toSwap, NC::NODE* newSrc, int srcType) {
    if(srcType!=1 && srcType!=3) std::runtime_error("ERROR: "+this->_info_name+"LINK_swapSrc(NC::LINK*, NC::NODE*, int): invalid `srcType` input");
    if(_find_ptr_itr<NC::NODE>(_nodes, newSrc)==_nodes.end())
        std::runtime_error("ERROR: "+this->_info_name+"LINK_swapSrc(NC::LINK*, NC::NODE*, int): arg for `newSrc` is not a valid `NC::NODE` address");
    else if(_find_ptr_itr<NC::LINK>(_links, toSwap)==_links.end())
        std::runtime_error("ERROR: "+this->_info_name+"LINK_swapSrc(NC::LINK*, NC::NODE*, int): arg for `toSwap` is not a valid `NC::LINK` address");


    if(toSwap->src==newSrc) return 1;
    
    if(toSwap->src!=nullptr) {
        // locate and erase the link address from the current src NODE
        std::vector<NC::LINK*>& eraseVec = (toSwap->type_src==1? toSwap->src->ln_out : toSwap->src->ln_share);
        std::vector<NC::LINK*>::const_iterator deleteItr = _vecfind_ptr_itr<NC::LINK*>(eraseVec, toSwap);
        eraseVec.erase(deleteItr);
    }
    // update src address in the link
    toSwap->src = newSrc;

    // add NC::LINK address to the new NODE in correct std::vector container
    if(srcType==1) newSrc->ln_out.push_back(toSwap);
    else if(srcType==3) newSrc->ln_share.push_back(toSwap);

    return 0;
}
int NC::NodeChart::LINK_swapDest(NC::LINK* toSwap, NC::NODE* newDest, int destType) {
    if(destType!=0 && destType!=2) std::runtime_error("ERROR: "+this->_info_name+"LINK_swapDest(NC::LINK*, NC::NODE*, int): invalid `destType` input");
    if(_find_ptr_itr<NC::NODE>(_nodes, newDest)==_nodes.end())
        std::runtime_error("ERROR: "+this->_info_name+"LINK_swapDest(NC::LINK*, NC::NODE*, int): arg for `newDest` is not a valid `NC::NODE` address");
    else if(_find_ptr_itr<NC::LINK>(_links, toSwap)==_links.end())
        std::runtime_error("ERROR: "+this->_info_name+"LINK_swapDest(NC::LINK*, NC::NODE*, int): arg for `toSwap` is not a valid `NC::LINK` address");

    if(toSwap->dest==newDest) return 1;

    if(toSwap->dest!=nullptr) {
        // locate and erase the link address from the current dest NODE
        std::vector<NC::LINK*>& eraseVec = (toSwap->type_dest==0? toSwap->dest->ln_in : toSwap->dest->ln_add);
        std::vector<NC::LINK*>::const_iterator deleteItr = _vecfind_ptr_itr<NC::LINK*>(eraseVec, toSwap);
        eraseVec.erase(deleteItr);
    }
    // update dest address in the link
    toSwap->dest = newDest;

    // add NC::LINK address to the new NODE in correct std::vector container
    if(destType==0) newDest->ln_in.push_back(toSwap);
    else if(destType==2) newDest->ln_add.push_back(toSwap);

    return 0;
}

int NC::NodeChart::LINK_delete(
    NC::LINK* LINK_toDelete
) {
    std::list<NC::LINK>::const_iterator linkItr = _find_ptr_itr<NC::LINK>(this->_links, LINK_toDelete);
    if(linkItr==this->_links.end()) std::runtime_error("ERROR: "+this->_info_name+"LINK_delete(NC::LINK*): invalid `NC::LINK` address to delete.");


    if(LINK_toDelete->src!=nullptr) {
        //locate and erase this link from LINK_toDelete src NODE.
        std::vector<NC::LINK*>& eraseVec = (LINK_toDelete->type_src==1? LINK_toDelete->src->ln_out : LINK_toDelete->src->ln_share);
        std::vector<NC::LINK*>::const_iterator deleteItr = _vecfind_ptr_itr<NC::LINK*>(eraseVec, LINK_toDelete);
        eraseVec.erase(deleteItr);
    }
    if(LINK_toDelete->dest!=nullptr) {
        //locate and erase this link from LINK_toDelete dest NODE.
        std::vector<NC::LINK*>& eraseVec = (LINK_toDelete->type_dest==0? LINK_toDelete->dest->ln_in : LINK_toDelete->dest->ln_add);
        std::vector<NC::LINK*>::const_iterator deleteItr = _vecfind_ptr_itr<NC::LINK*>(eraseVec, LINK_toDelete);
        eraseVec.erase(deleteItr);
    }

    this->_links.erase(linkItr);

    return 0;
}


int NC::NodeChart::save(std::string filename, bool overwrite) {
    std::ofstream saveFile;
    saveFile.open(filename);

    if(!saveFile.is_open()) std::runtime_error("ERROR: "+this->_info_name+"save(std::string, bool): could not open/create file.");





    return 0;
}
