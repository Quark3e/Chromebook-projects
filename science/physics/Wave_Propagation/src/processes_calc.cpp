
#include "processes.hpp"


std::atomic<size_t> maxThreadNum(0);

void process_calc() {
    maxThreadNum = std::thread::hardware_concurrency();

    

}
