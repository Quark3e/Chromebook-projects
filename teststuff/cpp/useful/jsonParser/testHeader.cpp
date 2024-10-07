
#include "jsonParser.hpp"


void _recur(JSON_P::jsonPair& cor, std::string _start="") {

    switch (cor.type()) {
        case 2:
        case 3:
            std::cout << _start << cor.type()<<": "<<cor.key << std::endl;
            for(size_t i=0; i<cor.size(); i++) {
                _recur(cor[i], _start+std::string(4, ' '));
            }
            break;
    default:
        std::cout << _start << cor.type()<<"- "<<cor.key<<": " << std::endl;
        break;
    }

}

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
    subPair1["sub0"] = std::vector<JSON_P::jsonPair>{
        JSON_P::jsonPair("subsub0", 69, true),
        JSON_P::jsonPair("subsub1", "420", true)
    };
    // subPair1["sub0"] = 420;
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

    std::cout << "-------------" << std::endl;

    // JSON_P::Parser _parsed("/home/berkhme/github_repo/Chromebook-projects/teststuff/cpp/useful/nodeChart/saveFiles/_TEST_project 0.json");
    JSON_P::Parser _parsed("testJSON.json");

    std::cout << "\n\n";
    std::cout << "all types:" << std::endl;
    _recur(_parsed._json);
    std::cout << "\n\n";
    std::cout << _parsed._json.toStr() << std::endl;


    return 0;
}
