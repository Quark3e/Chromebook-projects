
#pragma once
#ifndef HPP_HEXCLAW_CONSTANTS
#define HPP_HEXCLAW_CONSTANTS


/// Whether to use threads
#define useThreads		true
/// Whether to print checkpoints
#define threadDebug		false

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



#endif
