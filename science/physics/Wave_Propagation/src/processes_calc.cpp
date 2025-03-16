
#include "processes.hpp"


void process_calc() {
    maxThreadNum = std::thread::hardware_concurrency();

    std::unique_lock<std::mutex> u_lck__cout(mtx__cout, std::defer_lock);
    std::unique_lock<std::mutex> u_lck__ptrBMPsystem(mtx__ptrBMPsystem_Switch, std::defer_lock);
    std::unique_lock<std::mutex> u_lck__abs_cam_pixelPos(mtx__abs_cam_pixelPos_Access, std::defer_lock);
    std::unique_lock<std::mutex> u_lck__system_waves(mtx__system_waves_Access, std::defer_lock);
    std::unique_lock<std::mutex> u_lck__meter_per_px(mtx__meter_per_px_Access, std::defer_lock);

    mutex_cout("thread-calc: init", mtx__cout);
    while(atm__running_process_calc.load()) {
        // u_lck__abs_cam_pixelPos.lock();
        // u_lck__system_waves.lock();
        // u_lck__meter_per_px.lock();
        
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
                
                bitArrIdx+=1;
            }
            
        }

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
