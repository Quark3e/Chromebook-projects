
#include "jsonParser.hpp"


int main(int argc, char** argv) {
    JSON_P::jsonPair subPair(
        "singlePair",
        {
            JSON_P::jsonPair("sub0", true),
            JSON_P::jsonPair("sub1", std::string("sugondese"))
        },
        true
    );

    JSON_P::jsonPair mainPair(
        "mainPair",
        {
            subPair,
            JSON_P::jsonPair("sub2", float(69.420))
        },
        false
    );

    std::cout << mainPair.toStr(0) << std::endl;

    return 0;
}
