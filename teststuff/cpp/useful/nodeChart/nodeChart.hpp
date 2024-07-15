
#pragma once
#ifndef HPP_NODECHART
#define HPP_NODECHART

#include <string>
#include <iomanip>
#include <vector>
#include <list>


namespace NC {    

    struct NODE;

    struct LINK {
        std::string label   = ""; //optional
        std::string desc    = ""; //optional

        /**
         * Integer code for source type: Whether it's "out" or "share":
         * - {`out`: `1`, `share`: `3`}
         */
        int type_src;
        /**
         * Integer code for dest type: WHether it's "in" our "add"
         * - {`in`: `0`, `add`: `2`}
         */
        int type_dest;

        NC::NODE* src;  // link source
        NC::NODE* dest; // link destination


        LINK(
            int par_type_src, int par_type_dest,
            NC::NODE* par_src, NC::NODE* par_dest,
            std::string par_label="",std::string par_desc=""
        ) {
            if(par_type_src!=1 && par_type_src!=3 && par_type_dest!=0 && par_type_dest!=2) std::runtime_error("ERROR: NC::LINK() constructor par_type_{..} is invalid");

            this->type_src  = par_type_src;
            this->type_dest = par_type_dest;
            this->src  = par_src;
            this->dest = par_dest;
            this->label = par_label;
            this->desc  = par_desc;
        }

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

        //Address of the last `NC::NODE` element in the list
        NC::NODE* _lastAddedNode = nullptr;
        //Address of the last `NC::LINK` element in the list
        NC::LINK* _lastAddedLink = nullptr;

        template<typename storedType>
        int _find_ptr_idx(const std::list<storedType>& toCheck, storedType* ptr_toFind);
        template<typename storedType>
        auto _find_ptr_itr(const std::list<storedType>& toCheck, storedType* ptr_toFind);

        template<typename storedType>
        int _vecfind_ptr_idx(const std::vector<storedType>& toCheck, storedType toFind);
        template<typename storedType>
        auto _vecfind_ptr_itr( std::vector<storedType>& toCheck, storedType toFind);


        public:
        NodeChart(/* args */);
        size_t size(int whatList=0);

        NC::NODE* lastAdded_NODE();
        NC::LINK* lastAdded_LINK();

        NC::NODE operator[](size_t i) const;
        

        NC::NODE* NODE_add(
            std::string label   = "",
            std::string desc    = "",
            std::string bodyText= ""
        );
        NC::NODE* NODE_insert(
            size_t insert_idx,
            std::string label   = "",
            std::string desc    = "",
            std::string bodyText= ""
        );
        int NODE_delete(size_t NODE_idx, bool leaveFloating=false);
        int NODE_delete(NC::NODE* NODE_toDelete, bool leaveFloating=false);

        NC::LINK* LINK_add(
            size_t NODE_src_idx,
            size_t NODE_dest_idx,
            int type_src,
            int type_dest,
            std::string label   = "",
            std::string desc    = ""
        );
        NC::LINK* LINK_add(
            NC::NODE* NODE_src,
            NC::NODE* NODE_dest,
            int type_src,
            int type_dest,
            std::string label   = "",
            std::string desc    = ""
        );
        int LINK_swapSrc(NC::LINK* toSwap, NC::NODE* newSrc, int srcType);
        int LINK_swapDest(NC::LINK* toSwap, NC::NODE* newDest, int destType);
        int LINK_delete(NC::LINK* LINK_toDelete);

    };
    

} // namespace NODE_CHART


#endif