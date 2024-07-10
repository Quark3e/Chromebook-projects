
#include <nodeChart.hpp>


NC::NodeChart::NodeChart() {

}

NC::NODE NC::NodeChart::operator[](size_t i) const {
    if(i>=this->_nodes.size()) std::runtime_error("ERROR: "+_info_name+"operator[](size_t) index out of size");

    auto itr = this->_nodes.begin();
    advance(itr, i);
    return const_cast<NC::NODE&>(*itr);
}

int NC::NodeChart::NODE_add(
    std::string label,
    std::string desc,
    std::string bodyText
) {
    
}
int NC::NodeChart::NODE_insert(
    size_t insert_idx,
    std::string label,
    std::string desc,
    std::string bodyText
) {

}
int NC::NodeChart::NODE_delete(
    size_t NODE_idx
) {

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
