
#pragma once
#ifndef HPP_HEXCLAW_SERVO_CONTROL
#define HPP_HEXCLAW_SERVO_CONTROL


#include "constants.hpp"
#include "global_variables.hpp"
#include "../IK_header.hpp"

#include <pos2d.hpp>

#if _MACHINE__RPI_MAIN
#include <PiPCA9685/PCA9685.h>
#endif

using namespace PiPCA9685;


/**Angle offset to apply to given angles to make sure angle-origo/0 is paralllel along the arm axis*/
extern servo_angles_6DOF offset_q;
// extern float offset_q[6];

/**Angles to send to the servo motors at startup to hold a pose*/
extern servo_angles_6DOF startup_q;
// extern float startup_q[6];


/// @brief Apply real-world servo motor offsets to input angles
/// @param angles float()[6] values to add defaults onto: NOTE: original var will be modified
void add_defaults(servo_angles_6DOF& angles);

// void add_defaults(float angles[6]);

/// @brief apply correction function values for each servos error rate
/// @param angles float array of rotation values to correct
/// @return nothing. Modifies parameter variable
void q_corrections(servo_angles_6DOF& angles);
// void q_corrections(float angles[6]);

/// @brief Check if any angle in {angles} is exceeding servo motors range
/// @param angles angles to check
/// @param printErrors whether to print out any exceedings onto the terminal
/// @return boolean of whether any angles have exceeded
bool exceedCheck(servo_angles_6DOF& angles, bool printErrors=true);
// bool exceedCheck(float angles[6], bool printErrors=true);


/// @brief Sends rotation commands to servo motors
/// @param pcaBoard is a PCA9685 class object pointer
/// @param new_rotation 6 element float() array of new rotations
/// @param old_rotation 6 element float() array of old/previous/current rotations
/// @param servoInitialize *true* if it's the first time this function is called; *false* otherwise
/// @param mode *0*-rotation values sent directly; *1*-sent linearly over *totalTime* duration; *2*-sent according to mp1 over *totalTime* duration
/// @param totalTime time it takes for mode [1, 2] to finish
/// @param useDefault *true*-include IK-offsets in angles BEFORE writing to servo motors; *false*-do not include IK-offsets
/// @param printErrors whether to print any errors
/// @param printResult whether to print any results/checkpoints/non-essential-info
/// @return `0`- if successful; `-1` otherwise
/// @note if `useDefault` is true then it'll modify `new_rotation` accordingly to default value addition
int sendToServo(
    PCA9685* pcaBoard,
    servo_angles_6DOF& new_rotation,
    servo_angles_6DOF& old_rotation,
    bool servoInitialize,
    int mode = 0,
    float totalTime = 0,
    bool useDefault = true,
    bool printErrors= true,
    bool printResult= false
);
// int sendToServo(
//     PCA9685* pcaBoard,
//     float new_rotation[6],
//     float old_rotation[6],
//     bool servoInitialize,
//     int mode = 0,
//     float totalTime = 0,
//     bool useDefault = true,
//     bool printErrors = true,
//     bool printResult = false
// );


/**
 * @brief solve/get the three axis coordinates from webcam feed
 * @param mode 
 * different modes/alternatives for how to get the three coordinate values
 * `0` - {cam0.x, cam0.y, cam0.cntArea*scal}
 * `1` - {cam0.x, cam0.y, cam1.y}
 * `2` - {camTri.x, camTri.solvedZ, camTri.y}
 * @return `0`- if successful; `1`- if an error occured
*/


/**
 * @brief solve/get the three axis coordinates from webcam feed
 * @param cam0_pos
 * @param cam1_pos
 * @param mode different modes/alternatives for how to get the three coordinate values
 * `0` - {cam0.x, cam0.y, cam0.cntArea*scal}
 * `1` - {cam0.x, cam0.y, cam1.y}
 * `2` - {camTri.x, camTri.solvedZ, camTri.y}
 * @param camDim
 * @return `0`- if successful; `1`- if an error occured
 */
int getCoordinates(
    pos2d<float> cam0_pos,
    pos2d<float> cam1_pos,
    int mode,
    pos2d<float> camDim = pos2d<float>(DEFAULT_PREF_WIDTH, DEFAULT_PREF_HEIGHT)
);



#endif
