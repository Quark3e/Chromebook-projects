
#include "jsonParser.hpp"


int main(int argc, char** argv) {
    JSON_P::jsonPair subPair(
        "p1",
        {
            JSON_P::jsonPair("sub0", true),
            JSON_P::jsonPair("sub1", std::string("sugondese"))
        },
        false
    );

    JSON_P::jsonPair subPair1 = subPair;
    // subPair1["sub0"] = std::vector<JSON_P::jsonPair>{
    //     JSON_P::jsonPair("subsub0", 69),
    //     JSON_P::jsonPair("subsub1", "420")
    // };
    subPair1["sub0"] = 420;
    subPair1.key = "amogus";
    subPair1["sub0"].key = "chungus0";

    JSON_P::jsonPair mainPair(
        "mainPair",
        {
            subPair,
            JSON_P::jsonPair("sub2", float(69.420)),
            subPair1
        },
        false
    );

    std::cout << mainPair.toStr() << std::endl;

    return 0;
}
