
#include "processes.hpp"


void subProcess_calc(size_t processIdx, size_t idx_start, size_t idx_end);

/**
 * Number of sub/parallel processess/threads that the job will be split into. If it's >1 then it'll use spawn more threads.
 * 
 */
size_t numSubProcesses = 1;

#if takePerformance_processCalc
/**
 * Used as a container for the individual `numSubProcesses` parallel threads/processes delay measurements to then be grouped.
 * 
 */
std::vector<std::vector<double>> subProcess_split__delays;
#endif //takePerformance_processCalc

void process_calc() {
    maxThreadNum = std::thread::hardware_concurrency();

    switch (maxThreadNum) {
        case 0: throw std::runtime_error("ERROR: maxThreadNum cannot be 0. Something is wrong"); break;
        case 1: throw std::runtime_error("ERROR: maxThreadNum == 1. Can't run this program. Need to think of another solution");
        case 2: numSubProcesses = maxThreadNum-1;
        default:
        numSubProcesses = maxThreadNum-2;
    }
    #if takePerformance_processCalc
    numSubProcesses = 1;
    
    subProcess_split__delays = std::vector<std::vector<double>>(numSubProcesses, std::vector<double>(ptr_delays__process_calc_calc->size()-1, 0));
    #endif

    size_t maxArrLen = (system_dim.x*system_dim.y)/(pixelSpacing*pixelSpacing);
    std::vector<pos2d<size_t>> idxRange;
    for(size_t i=1; i<=numSubProcesses; i++) {
        idxRange.push_back(pos2d<size_t>((double(i-1)/numSubProcesses)*maxArrLen, (double(i)/numSubProcesses)*maxArrLen));
    }

    mutex_cout("thread-calc: idxRanges:"+formatVector(idxRange, 0), mtx__cout);

    mutex_cout("thread-calc: maxThreadNum: "+formatNumber(maxThreadNum.load(),0,0), mtx__cout);

    std::unique_lock<std::mutex> u_lck__cout(mtx__cout, std::defer_lock);
    std::unique_lock<std::mutex> u_lck__ptrBMPsystem(mtx__ptrBMPsystem_Switch, std::defer_lock);
    std::unique_lock<std::mutex> u_lck__abs_cam_pixelPos(mtx__abs_cam_pixelPos_Access, std::defer_lock);
    std::unique_lock<std::mutex> u_lck__system_waves(mtx__system_waves_Access, std::defer_lock);
    std::unique_lock<std::mutex> u_lck__meter_per_px(mtx__meter_per_px_Access, std::defer_lock);
    #if takePerformance_processCalc
    std::unique_lock<std::mutex> u_lck__delays_processCalc(mtx__delays_processCalc_switch, std::defer_lock);
    #endif

    mutex_cout("thread-calc: init", mtx__cout);
    while(atm__running_process_calc.load()) {
        // u_lck__abs_cam_pixelPos.lock();
        // u_lck__system_waves.lock();
        // u_lck__meter_per_px.lock();

        auto timePoint = std::chrono::steady_clock::now();
        try {
            
            std::vector<std::thread> threadObjects;
            for(size_t n=1; n<numSubProcesses; n++) {
                threadObjects.emplace_back(
                    [idxRange, n] {subProcess_calc(n, idxRange[n].x, idxRange[n].y);}
                );
            }
            subProcess_calc(0, idxRange[0].x, idxRange[0].y);
            // mutex_cout("thread-calc: waiting for sub calc threads.", mtx__cout);

            for(size_t i=0; i<threadObjects.size(); i++) {
                if(threadObjects.at(i).joinable()) threadObjects.at(i).join();
            }
            // mutex_cout("thread-calc: joined and finished all threads..", mtx__cout);
        }
        catch(const std::exception& e) {
            std::cerr << "ERROR: " << e.what() << '\n';
            exit(1);
        }

        
        #if takePerformance_processCalc
        /// Clear the existing data
        for(size_t i=0; i<ptr_delays__process_calc_calc->size(); i++) {
                ptr_delays__process_calc_calc->operator[](i) = 0;
        }

        for(size_t i=0; i<ptr_delays__process_calc_calc->size()-1; i++) {
            for(size_t n=0; n<numSubProcesses; n++) {
                ptr_delays__process_calc_calc->operator[](i) += subProcess_split__delays[n][i];
            }
            ptr_delays__process_calc_calc->operator[](i) /= (((system_dim.x*system_dim.y)/(pixelSpacing*pixelSpacing))*numSubProcesses);

        }
        ptr_delays__process_calc_calc->get("Total abs time") = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now()-timePoint).count();

        u_lck__delays_processCalc.lock();
        auto temp_delaysPtr = ptr_delays__process_calc_calc;
        ptr_delays__process_calc_calc = ptr_delays__process_calc_gui;
        ptr_delays__process_calc_gui = temp_delaysPtr;
        u_lck__delays_processCalc.unlock();
        #endif

        /*
        size_t bitArrIdx = 0;

        pos2d<double> realPos(0, 0);
        std::vector<double> indiv_amplitude;
        std::vector<pos2d<double>> indiv_phaseVec;
        double sum_amplitude = 0;
        pos2d<double> sum_phaseVec(0, 0);
        
        std::vector<double> amplitudeLim{-double(system_waves.size()*1.0), double(system_waves.size()*1.0)};
        for(size_t y=0; y<system_dim.y; y++) {
            if(!atm__running_process_calc.load()) break;
            for(size_t x=0; x<system_dim.x; x++) {
                bitArrIdx = y*system_dim.x + x;
                realPos.x = (double(x) + double(abs_cam_pixelPos.x)) * meter_per_px;
                realPos.y = (double(y) + double(abs_cam_pixelPos.y)) * meter_per_px;
                indiv_amplitude.clear();
                indiv_phaseVec.clear();
                
                sum_amplitude   = getWavePhaseAmplitudeSum(realPos, system_waves, &indiv_amplitude);
                sum_phaseVec    = getWaveVectorSum(realPos, system_waves, &indiv_phaseVec);
                
                ptr_BMP_system_calc->at_pos(x, y)           = realPos;
                ptr_BMP_system_calc->at_indiv_ampl(x, y)    = indiv_amplitude;
                ptr_BMP_system_calc->at_indiv_phaseVec(x, y)= indiv_phaseVec;
                ptr_BMP_system_calc->at_sum_ampl(x, y)      = sum_amplitude;
                ptr_BMP_system_calc->at_sum_phaseVec(x, y)  = sum_phaseVec;
                
                std::vector<uint8_t> RGB_col;
                switch(scalMethod) {
                    case 0: { // sine wave amplitude sum
                        RGB_col = graph_colour_scale(sum_amplitude/amplitudeLim[1]);
                    }
                    break;
                    case 1: { // phase shift vector sum sq
                        RGB_col = graph_colour_scale(pow(sum_phaseVec.hypotenuse(), 2) / pow(amplitudeLim[1], 2));
                    }
                    break;
                    default: throw std::invalid_argument("invalid scalMethod arg.");
                }
                ptr_BMP_system_calc->get_BMP_arr()[bitArrIdx] = RGB_col[0];
                
                // bitArrIdx+=1;
            }
            
        }
        */

        u_lck__ptrBMPsystem.lock();
        auto temp = ptr_BMP_system_calc;
        ptr_BMP_system_calc = ptr_BMP_system_gui;
        ptr_BMP_system_gui = temp;
        u_lck__ptrBMPsystem.unlock();

        // u_lck__meter_per_px.unlock();
        // u_lck__system_waves.unlock();
        // u_lck__abs_cam_pixelPos.unlock();
    }

}

void subProcess_calc(size_t processIdx, size_t idx_start, size_t idx_end) {
    if(idx_start == idx_end) throw std::invalid_argument("subProcess_calc(size_t, size_t): invalid range values. Are not allowed to be the same");

    auto getPos = [](size_t _absIdx) {
        return pos2d<size_t>(
            (_absIdx) % (system_dim.x/pixelSpacing),
            floor(double(_absIdx)/double(system_dim.y/pixelSpacing))
        );
    };
    

    pos2d<double> realPos(0, 0);
    std::vector<double> indiv_amplitude;
    std::vector<pos2d<double>> indiv_phaseVec;
    double sum_amplitude = 0;
    pos2d<double> sum_phaseVec(0, 0);

    std::vector<double> amplitudeLim{-double(system_waves.size()*1.0), double(system_waves.size()*1.0)};

    #if takePerformance_processCalc
    auto timePoint = std::chrono::steady_clock::now();

    for(size_t i=0; i<ptr_delays__process_calc_calc->size()-1; i++) {
        subProcess_split__delays.at(processIdx).at(i) = 0;
        // ptr_delays__process_calc_calc->operator[](i) = 0;
    }
    #endif
    for(size_t i=idx_start; i<idx_end; i++) {
        if(!atm__running_process_calc.load()) break;
        pos2d<size_t> pixelPos = getPos(i);
        
        realPos.x = (double(pixelPos.x*pixelSpacing) + double(abs_cam_pixelPos.x)) * meter_per_px;
        realPos.y = (double(pixelPos.y*pixelSpacing) + double(abs_cam_pixelPos.y)) * meter_per_px;
        indiv_amplitude.clear();
        indiv_phaseVec.clear();
        
        #if takePerformance_processCalc
        subProcess_split__delays[processIdx][0] += std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now()-timePoint).count();
        timePoint = std::chrono::steady_clock::now();
        #endif
        
        sum_amplitude   = getWavePhaseAmplitudeSum(realPos, system_waves, &indiv_amplitude);
        #if takePerformance_processCalc
        subProcess_split__delays[processIdx][1] += std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now()-timePoint).count();
        timePoint = std::chrono::steady_clock::now();
        #endif
        sum_phaseVec    = getWaveVectorSum(realPos, system_waves, &indiv_phaseVec);
        
        #if takePerformance_processCalc
        subProcess_split__delays[processIdx][2] += std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now()-timePoint).count();
        timePoint = std::chrono::steady_clock::now();
        #endif
        
        // if(pixelPos.x>=system_dim.x/pixelSpacing) {
            //     pixelPos.x--;
            //     // throw std::runtime_error("ERROR: In subProcess_calc the pixelPos.x is indexing a position outside the available region: "+formatContainer1(pixelPos,2,0,0));
            // }
            // if(pixelPos.y>=system_dim.y/pixelSpacing) {
                //     pixelPos.y--;
                //     // throw std::runtime_error("ERROR: In subProcess_calc the pixelPos.y is indexing a position outside the available region: "+formatContainer1(pixelPos,2,0,0));
                // }
        if(pixelPos.x>=system_dim.x/pixelSpacing) {
            throw std::runtime_error("ERROR: In subProcess_calc the pixelPos.x is indexing a position outside the available region: "+formatContainer1(pixelPos,2,0,0));
        }
        if(pixelPos.y>=system_dim.y/pixelSpacing) {
            throw std::runtime_error("ERROR: In subProcess_calc the pixelPos.y is indexing a position outside the available region: "+formatContainer1(pixelPos,2,0,0));
        }
        
        ptr_BMP_system_calc->at_pos(pixelPos.x, pixelPos.y)           = realPos;
        ptr_BMP_system_calc->at_indiv_ampl(pixelPos.x, pixelPos.y)    = indiv_amplitude;
        ptr_BMP_system_calc->at_indiv_phaseVec(pixelPos.x, pixelPos.y)= indiv_phaseVec;
        ptr_BMP_system_calc->at_sum_ampl(pixelPos.x, pixelPos.y)      = sum_amplitude;
        ptr_BMP_system_calc->at_sum_phaseVec(pixelPos.x, pixelPos.y)  = sum_phaseVec;
        
        #if takePerformance_processCalc
        subProcess_split__delays[processIdx][3] += std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now()-timePoint).count();
        timePoint = std::chrono::steady_clock::now();
        #endif
        
        std::vector<uint8_t> RGB_col;
        switch(scalMethod) {
            case 0: { // sine wave amplitude sum
                RGB_col = graph_colour_scale(sum_amplitude/amplitudeLim[1]);
            }
            break;
            case 1: { // phase shift vector sum sq
                RGB_col = graph_colour_scale(pow(sum_phaseVec.hypotenuse(), 2) / pow(amplitudeLim[1], 2));
            }
            break;
            default: throw std::invalid_argument("invalid scalMethod arg.");
        }
        
        #if takePerformance_processCalc
        subProcess_split__delays[processIdx][4] += std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now()-timePoint).count();
        timePoint = std::chrono::steady_clock::now();
        #endif
        ptr_BMP_system_calc->get_BMP_arr()[i] = RGB_col[0];
        
        #if takePerformance_processCalc
        subProcess_split__delays[processIdx][5] += std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now()-timePoint).count();
        timePoint = std::chrono::steady_clock::now();
        #endif
    }

}
