
#pragma once
#ifndef HPP_NODECHART
#define HPP_NODECHART

#include <string>
#include <iomanip>
#include <vector>
#include <list>


namespace NC {    

    struct LINK {
        std::string label   = ""; //optional
        std::string desc    = ""; //optional

        int type_src;
        int type_dest;

        NC::NODE* src;
        NC::NODE* dest;
    

    };
    
    struct NODE {
        std::string label   = ""; //optional
        std::string desc    = ""; //optional
        std::string bodyText= ""; //optional

        std::vector<NC::LINK*> ln_in;     // type 0 //optional
        std::vector<NC::LINK*> ln_out;    // type 1 //optional
        std::vector<NC::LINK*> ln_add;    // type 2 //optional
        std::vector<NC::LINK*> ln_share;  // type 3 //optional
        NODE(
            std::string par_label="",std::string par_desc="", std::string par_bodyText="",
            std::vector<NC::LINK*> par_ln_in=std::vector<NC::LINK*>(),  std::vector<NC::LINK*> par_ln_out=std::vector<NC::LINK*>(),
            std::vector<NC::LINK*> par_ln_add=std::vector<NC::LINK*>(), std::vector<NC::LINK*> par_ln_share=std::vector<NC::LINK*>()
        ): label{par_label}, desc{par_desc}, bodyText{par_bodyText}, ln_in{par_ln_in}, ln_out{par_ln_out}, ln_add{par_ln_add}, ln_share{par_ln_share} {}

    };


    class NodeChart {
        private:
        std::string _info_name = "NC::NodeChart::";
        /**
         * Main storage for the different nodes. The nodes will be differentiated by their element
         * index or element address in the std::list container `_nodes`.
         */
        std::list<NC::NODE> _nodes;
        /**
         * Main storage for the different links between nodes. The links will be differentiated
         * by their element address in the std::list container `_links`
         */
        std::list<NC::LINK> _links;

        public:
        NodeChart(/* args */);

        NC::NODE operator[](size_t i) const;
        

        int NODE_add(
            std::string label   = "",
            std::string desc    = "",
            std::string bodyText= ""
        );
        int NODE_insert(
            size_t insert_idx,
            std::string label   = "",
            std::string desc    = "",
            std::string bodyText= ""
        );
        int NODE_delete(size_t NODE_idx);
        int NODE_delete(NC::NODE* NODE_toDelete);

        int LINK_add(
            size_t NODE_src_idx,
            size_t NODE_dest_idx,
            std::string label   = "",
            std::string desc    = ""
        );
        int LINK_add(
            NC::NODE* NODE_src,
            NC::NODE* NODE_dest,
            std::string label   = "",
            std::string desc    = ""
        );
        int LINK_delete(NC::LINK* LINK_toDelete);


    };
    

} // namespace NODE_CHART


#endif