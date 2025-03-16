
#include "includes.hpp"

#include "variables_data.hpp"
#include "functions_data.hpp"

#include "variables_gui.hpp"
#include "functions_gui.hpp"

#include "processes.hpp"

int main(int argc, char** argv) {
    mutex_cout("init", mtx__cout);
    std::thread thread_calc(process_calc);
    try {
        process_gui();
        
    }
    catch(const std::exception& e) {
        std::cerr << e.what() << '\n';
        exit(1);
    }
    mutex_cout("gui ended", mtx__cout);

    thread_calc.join();

    return 0;
}

