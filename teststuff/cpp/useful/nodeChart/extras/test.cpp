

#include <iostream>
#include "../nodeChart.hpp"



int main(int argc, char** argv) {
    NC::NodeChart chart0;

    auto node0ptr = chart0.NODE_add("testLabel", "testDesc", "testBody");
    auto node1ptr = chart0.NODE_add("test2", "desc2", "body2");
    auto link01ptr = chart0.LINK_add(node0ptr, node1ptr, 1, 0, "linklabel", "linkdesc");



    return 0;

}
