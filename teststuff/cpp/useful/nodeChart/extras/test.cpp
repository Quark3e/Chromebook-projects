

#include <iostream>
#include "../nodeChart.hpp"



int main(int argc, char** argv) {
    std::cout << "--START--" << std::endl;
    NC::NodeChart chart0;

    auto node0ptr = chart0.NODE_add("testLabel", "testDesc", "testBody");
    auto node1ptr = chart0.NODE_add("test2", "desc2", "body2");
    auto link01ptr = chart0.LINK_add(node0ptr, node1ptr, 1, 0, "linklabel", "linkdesc");
    std::cout << "--INIT--" << std::endl;

    chart0.NODE_delete(node1ptr, true);
    std::cout << "--DELETE--" << std::endl;



    std::cout << "--END--" << std::endl;
    return 0;
}
