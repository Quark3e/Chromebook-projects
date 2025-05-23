
#pragma once
#ifndef HPP_HEXCLAW_CONSTANTS
#define HPP_HEXCLAW_CONSTANTS

#define _XSTRINGLIT(x) STRINGLIT(x)
#define STRINGLIT(x) #x

/**
 * Whether the project is built on the main rpi board.
 * If this is `false` then the project is being built on remote machines without gpio methods.
 */
#define _MACHINE__RPI_MAIN  (_WIN32? false : true)

/**
 * Absolute path of the program directory.
 * Will be used as a frame of reference for accessing files such as data and e.t.c so it'll be defined by the compiler definitions
 * during build.
 * 
 */
// #define _PROGRAM_PATH ""

// #define _PROGRAM_PATH__EXE ""

// #define _PROGRAM_PATH__DATA ""


/// Whether to print checkpoints
#define threadDebug		false

#define SHLEEP(sleep_ms) (std::this_thread::sleep_for(std::chrono::milliseconds(sleep_ms)))

#define GET_PRGRSS(STARTVAL, ENDVAL, PRGRS_DECIM) (STARTVAL+(ENDVAL-STARTVAL)*PRGRS_DECIM)


/**
 * Whether to setup and handle performances of the program.
 * That includes:
 *  - measure delays of different checkpoints/sections 
 *  - measure overall delays
 *  - display fps
*/
#define takePerf		true
/// Whether to record opencv frame/image/`cv::Mat` variables received from `cv::VideoCapture` object(s)
#define recordFrames	false


#define DEFAULT_PREF_WIDTH   320 //640
#define DEFAULT_PREF_HEIGHT  240 //480


#if !_MACHINE__RPI_MAIN
#include <stdint.h>

/// Mockup/Replica version of the pca library with empty replica functions just for the sake of cross-platform compatibility without the use of the original class
namespace PiPCA9685 {
    class PCA9685 {
        public:
        explicit PCA9685(bool init) {
            if(init) this->init();
        }
        ~PCA9685() {

        }
    
        bool init() {
            /// Because this is a replica copy it doesn't serve a "real"/GPIO-controlling purpose.
            return true;
        }

        void set_pwm_freq(const double freq_hz) {

        }
        
        void set_pwm(const int channel, const uint16_t on, const uint16_t off) {

        }

        void set_all_pwm(const uint16_t on, const uint16_t off) {

        }

        void set_pwm_ms(const int channel, const double ms) {

        }

        private:

        bool _init;
        double frequency = 200.0;
    };
}
#endif // #if !_MACHINE__RPI_MAIN

#endif // HPP_HEXCLAW_CONSTANTS
