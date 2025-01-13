
#include <iostream>
#include <string>
#include <vector>

#include <thread>
#include <mutex>
#include <atomic>
#include <chrono>

#include <math.h>


std::atomic<size_t> highest_prime = 0;
std::atomic<bool>   race_won = false;

std::mutex mtx_cout;


class threadClass {
    private:
    std::string _info_name="threadClass";
    
    std::thread _threadObj;
    bool _initCalled = false;

    static void solve_prime(size_t idx, size_t _search_min, size_t _search_max);
    public:
    size_t object_idx;

    size_t search_min;
    size_t search_max;

    threadClass *thisPtr = nullptr;

    threadClass(threadClass&& other) = default;
    threadClass(
        size_t objIdx,
        size_t numSearch_min,
        size_t numSearch_max,
        bool call_init=true
    ): object_idx(objIdx), search_min(numSearch_min), search_max(numSearch_max), thisPtr(this) {
        if(call_init) {
            int initCode=0;
            if((initCode=this->init())) throw std::runtime_error(_info_name+"threadClass(bool) --> this->init() failed: "+std::to_string(initCode));
        }
    }
    ~threadClass() {
        if(_threadObj.joinable()) {
            race_won = true;
            _threadObj.join();
        }
    }

    int init() {
        if(_initCalled) throw std::runtime_error(_info_name+"::init() idx:"+std::to_string(object_idx)+" this function has been called again.");
        _threadObj = std::thread(solve_prime, object_idx, search_min, search_max);
        _initCalled = true;
        return 0;
    }

    void join() {
        _threadObj.join();
    }
};


int main(int argc, char** argv) {
    std::vector<threadClass> participants;
    std::unique_lock<std::mutex> u_lck_cout(mtx_cout, std::defer_lock);

    u_lck_cout.lock();
    std::cout << "creating instance" << std::endl;
    u_lck_cout.unlock();
    size_t splitSize = 1;
    size_t findMax = 1'000'000;
    if(argc>1) splitSize = std::stoi(argv[1]);
    for(size_t p=0; p<splitSize; p++) {
        participants.emplace_back(p, findMax*(float(p)/float(splitSize)), findMax*(float(p+1)/float(splitSize)), false);
    }
    u_lck_cout.lock();
    std::cout << "initialising instances." << std::endl;
    u_lck_cout.unlock();
    
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    auto _startTime = std::chrono::steady_clock::now();
    for(size_t p=0; p<participants.size(); p++) {
        participants.at(p).init();
    }

    u_lck_cout.lock();
    std::cout << "waiting to join.." << std::endl;
    u_lck_cout.unlock();
    for(size_t p=0; p<participants.size(); p++) {
        participants.at(p).join();
    }
    std::cout << "\nHighest prime number: "<<highest_prime << std::endl;
    std::cout << "Total time: " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now()-_startTime).count() << "ms";

    return 0;
}


void threadClass::solve_prime(size_t idx, size_t _search_min, size_t _search_max) {
    std::unique_lock<std::mutex> u_lck_cout(mtx_cout, std::defer_lock);
    for(size_t i=_search_min; i<_search_max; i++) {
        if(race_won.load()) {
            return;
        }
        bool divisor_found = false;
        for(size_t ii=2; ii<sqrt(i); ii++) {
            if(i%ii==0) {
                //divisor found
                divisor_found = true;
            }
        }
        if(divisor_found) continue;
        else {
            //is prime
            if(i>highest_prime.load()) {
                highest_prime = i;
            }
        }
    }
    if(race_won.load()) return;
    // race_won = true;
    // u_lck_cout.lock();
    // std::cout << "thread: "<< idx << " won." <<std::endl;
    // u_lck_cout.unlock();
}

