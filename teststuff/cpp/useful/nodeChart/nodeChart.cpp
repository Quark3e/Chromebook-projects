
#include <nodeChart.hpp>


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
}

/**
 * @brief Return the address of the last added Node
 * @note "last added" does not have to mean last element as nodes can be inserted at given pos.
 * @return `NC::NODE*` to the last added Node.
 */
NC::NODE* NC::NodeChart::lastAdded_NODE() {
    return this->_lastAddedNode;
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
    size_t NODE_idx
) {
    if(NODE_idx >= this->_nodes.size()) std::runtime_error(
        "ERROR: "+this->_info_name+"NODE_delete(size_t):"+
        " arg for `NODE_idx` is bigger than available element size"
    );

}
int NC::NodeChart::NODE_delete(
    NC::NODE* NODE_toDelete
) {

}

int NC::NodeChart::LINK_add(
    size_t NODE_src_idx,
    size_t NODE_dest_idx,
    std::string label,
    std::string desc
) {

}
int NC::NodeChart::LINK_add(
    NC::NODE* NODE_src,
    NC::NODE* NODE_dest,
    std::string label,
    std::string desc
) {

}
int NC::NodeChart::LINK_delete(
    NC::LINK* LINK_toDelete
) {

}
