
#include <iostream>
#include <string>   //std::stof
#include <pos2d.hpp>
#include <useful.hpp>
#include <line_intersect2D.hpp>


int main(int argc, char **argv) {
    if(argc==2 && (!strcmp(argv[1], "-h") || !strcmp(argv[1], "--help"))) {
        std::cout << std::endl << "[program]   {lineA_0} {lineA_1} {lineB_0} {lineB_1}" << std::endl;
        return 0;
    }
    if(argc < 3) throw std::invalid_argument("main: invalid number of program command line arguments.");
    
    std::cout << "main: received argv[]: \"";
    for(size_t i=0; i<argc; i++) std::cout << argv[i] << " ";
    std::string _totalRaw = "";
    for(size_t i=1; i<argc; i++) {
        // std::cout << argv[i] << " ";
        _totalRaw += std::string(argv[i]);
    }
    std::cout << std::endl;
    std::vector<pos2d<float>> line_points = parse__2D_coordinates(_totalRaw, ",", "[]", {' '});
    if(line_points.size()!=4) throw std::invalid_argument("invalid number of line_points parsed from given string. Got "+formatNumber(line_points.size(),0,0)+" num points instead of 4.");

    // std::vector<pos2d<float>> line_A{parse__2D_coord(argv[1],",","[]",{' '}), parse__2D_coord(argv[2],",","[]",{' '})};
    // std::vector<pos2d<float>> line_B{parse__2D_coord(argv[3],",","[]",{' '}), parse__2D_coord(argv[4],",","[]",{' '})};
    std::vector<pos2d<float>> line_A{line_points[0], line_points[1]};
    std::vector<pos2d<float>> line_B{line_points[2], line_points[3]};


    std::cout << " A0: "<< line_A[0] << std::endl;
    std::cout << " A1: "<< line_A[1] << std::endl;
    std::cout << " B0: "<< line_B[0] << std::endl;
    std::cout << " B1: "<< line_B[1] << std::endl;

    pos2d<float> intersect;
    try {
        intersect = getLineIntersect_2D(line_A[0], line_A[1], line_B[0], line_B[1], true, 2, 0.01);
        
    }
    catch(const std::exception& e) {
        std::cerr << e.what() << '\n';
        exit(1);
    }
    
    std::cout << "\n found line intersection at: " << intersect << std::endl;


    return 0;
}