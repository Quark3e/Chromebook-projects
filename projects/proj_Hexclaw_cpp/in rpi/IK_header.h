
#pragma once
#include <unistd.h>
#include <iostream>
#include <string>
#include <math.h>

#include "HW_headers/motion_control/motion_profiles.hpp"
// #include "hexclaw_global.hpp"

using namespace std;


float arm_link[6] = {145, 130, 75, 50, 25, 25}; //mm
float sLoadWeight[6] = {0, 0.130, 0.085, 0, 0, 0}; //kg
float offset_q[6] = {90, 0, 135, 90, 90, 90};
float startup_q[6] = {0, 115, -90, 0, -25, 00}; //offset not added

float solved_q[6] = {0, 0, 0, 0, 0, 0}; //new rotation angles solved.



/// @brief Apply real-world servo motor offsets to input angles
/// @param angles float()[6] values to add defaults onto: NOTE: original var will be modified
void add_defaults(float angles[6]) {
    angles[0] = offset_q[0] + angles[0];
    angles[1] = offset_q[1] + angles[1];
    angles[2] = 180 - (offset_q[2] + angles[2]);
    angles[3] = offset_q[3] + angles[3];
    angles[4] = 180 - (offset_q[4] + angles[4]);
    angles[5] = offset_q[5] + angles[5];
}

/// @brief apply correction function values for each servos error rate
/// @param angles float array of rotation values to correct
/// @return nothing. Modifies parameter variable
void q_corrections(float angles[6]) {
    float error_Consts[6] = {
        0.802139037433155,
        0.7538,
        0.8772,
        1.0345,
        1,
        1
    };
    angles[0] = angles[0] * error_Consts[0];
    // angles[1] = angles[1] * error_Consts[1];
    angles[1] =
        1.751 * pow(10, -9) * pow(angles[1], 5)
        -7.693 * pow(10, -7) * pow(angles[1], 4)
        +0.000117 * pow(angles[1], 3)
        -0.006447 * pow(angles[1], 2)
        +0.71 * angles[1]
        +25.66;
    angles[2] = 
        4.605 * pow(10, -9) * pow(angles[2], 5)
        -2.178 * pow(10, -6) * pow(angles[2], 4)
        +0.0003815 * pow(angles[2],3)
        -0.02938 * pow(angles[2],2)
        +1.776 * angles[2]
        +4.114;
    angles[3] = angles[3] * error_Consts[3];
    angles[4] = angles[4] * error_Consts[4];
    angles[5] = angles[5] * error_Consts[5];
}

/// @brief Check if any angle in {angles} is exceeding servo motors range
/// @param angles angles to check
/// @param printErrors whether to print out any exceedings onto the terminal
/// @return boolean of whether any angles have exceeded
bool exceedCheck(float angles[6], bool printErrors=true) {
    bool exceeded = false;
    bool whichExceeded[6] = {false, false, false, false, false, false};
    string typeOfExceeded[6] = {"null", "null", "null", "null", "null", "null"};

    for(int i=0; i<5; i++) {
        if(int(angles[i])<0) {
            exceeded=true;
            whichExceeded[i]=true;
            typeOfExceeded[i]="under";
            angles[i]=0; 
        }
        if(int(angles[i])>180) {
            exceeded=true;
            whichExceeded[i]=true;
            typeOfExceeded[i]="over";
            angles[i]=180;
        }
    }
    if(exceeded && printErrors) {
        printf("\n");
        for(int i=0; i<6; i++) { if(whichExceeded[i]) printf("\t-servo q[%d] exceeded: %s\n",i, typeOfExceeded[i].c_str()); }
    }
    // printf("\n");
    if(exceeded) return true;
    else return false;   
}



// Inverse Kinematics - specific functions:
/// @brief Calculate Inverse Kinematic angles from given PP and orientation
/// @param q 6 element float() array linked via pointers to original source
/// @param PP XYZ 3 element float() array
/// @param a orient[0] [radians]
/// @param b orient[1] [radians]
/// @param Y orient[2] [radians]
/// @param mode 0-q return in radians; 1-q return in degrees
/// @param posOption "+" positive IK-setup; "-" negative IK_setup
/// @param length_scalar scaling variable for each robot arm/length
/// @param coord_scalar scaling variable for each coordinate axis in given PP
/// @param printText whether to print non-essential-info
/// @param printErrors whether to print essential-info/error-info
/// @return returns true if position is reachable with given setups; false otherwise
bool getAngles(
	float q[6],
	float PP[3],
	float a, float b, float Y,
	int mode = 0,
	char posOption = '-',
	float length_scalar = 1, float coord_scalar = 1,
	bool printText = false, bool printErrors = false
) {
	bool isReachable = true;
    float a1_exceed = 0, b1_exceed = 0;
	float frame1X=0, frame1Y=0, frame1Z = 0;

	float P5[3];
	for(int i=0; i<6; i++) {
		if(i<3) PP[i]=PP[i]*coord_scalar;
		arm_link[i]=arm_link[i]*length_scalar;
	}
	float l = (arm_link[4]+arm_link[5]) * cos(b);
	P5[0] = PP[0] - l * sin(a);
	P5[1] = PP[1] - l * cos(a);
	P5[2] = PP[2] - (arm_link[4]+arm_link[5]) * sin(b);

	if(printText) printf(" P5 coords: %f %f %f\n", int(round(P5[0])), int(round(P5[1])), int(round(P5[2])));

	if(P5[1]<0) P5[1] = 0;
	else if(P5[1]==0) {
		if(P5[0]>0) q[0] = toRadians(90);
		else if(P5[0] < 0) q[0] = toRadians(-90);
		else if(P5[0] == 0) q[0] = toRadians(0);
	}
	else q[0] = atan(-P5[0]/P5[1]);
	a = 0-a;

	if(posOption=='+') q[2] = acos((pow(P5[0], 2) + pow(P5[1], 2) + pow(P5[2] - arm_link[0], 2) - pow(arm_link[1], 2) - pow(arm_link[2] + arm_link[3], 2)) /(2 * arm_link[1] * (arm_link[2] + arm_link[3])));
	else if(posOption=='-') q[2] = acos((pow(P5[0], 2) + pow(P5[1], 2) + pow(P5[2] - arm_link[0], 2) - pow(arm_link[1], 2) - pow(arm_link[2] + arm_link[3], 2)) /(2 * arm_link[1] * (arm_link[2] + arm_link[3])));
    if(isnan(q[2])) isReachable = false;


	float lambdaVar=0, muVar=0;

	if(!sqrt(pow(P5[0], 2) + pow(P5[1], 2))==0) lambdaVar = atan((P5[2]-arm_link[0]) / sqrt(pow(P5[0], 2) + pow(P5[1], 2)));
	else isReachable = false;
	muVar = atan(((arm_link[2] + arm_link[3]) * sin(q[2])) /(arm_link[1] + (arm_link[2] + arm_link[3]) * cos(q[2])));

	if(printText) printf("lambda:%d mu:%d\n", int(round(toDegrees(lambdaVar))), int(round(toDegrees(muVar))));
	if(posOption=='+') q[1] = lambdaVar - muVar;
	else if(posOption=='-') {
		if(lambdaVar+muVar>0) q[1] = lambdaVar+muVar;
		else {
			if(printErrors) printf("q[1] error occured\n");
			isReachable = false;
		}
		q[2] = 0 - q[2];
	}

	float a1 = a - q[0];
	float b1 = b - (q[1] + q[2]);
    a1 = a1*cos(b); //debug: True
	
	if(true) {a1 = a1*cos(b);}

	if(printText) printf("a1:%d b1:%d\n",int(toDegrees(a1)),int(toDegrees(b1)));
	
	if (toDegrees(a1)>90) a1_exceed = 1;
    else if(toDegrees(a1)<-90) a1_exceed = -1;
    if(toDegrees(b1)>90) b1_exceed = 1;
    else if(toDegrees(b1)<-90) b1_exceed = -1;
	if(b1_exceed!=0 || a1_exceed!=0) {
		if(printErrors) {
			if(a1_exceed!=0) printf(" a1 exceeded beyond %d", a1_exceed*90);
			if(b1_exceed!=0) printf(" b1 exceeded beyond %d", b1_exceed*90);
			printf("\n");
		}
		isReachable = false;
	}

	frame1X = (arm_link[4]+arm_link[5])*cos(b1)*sin(a1);
    frame1X*=cos(b); //debug: True
	//frame1X = frame1X*cos(b);
	frame1Y = (arm_link[4]+arm_link[5])*cos(b1)*cos(a1);
	frame1Z = (arm_link[4]+arm_link[5])*sin(b1);
	if(printText) printf(" frame1_x:%d frame1_y:%d frame1_z:%d\n",int(frame1X),int(frame1Y),int(frame1Z));

	if(b1==0) {
		if(frame1X>0) q[3]=toRadians(90);
		else if(frame1X<0) q[3]=toRadians(-90);
		else if(frame1X==0) q[3]=toRadians(0);
		if(printText) printf("b1=0  =>  q4 was adjusted\n");
	}
	else if(b1<0 || b1>0) q[3]=atan(frame1X/frame1Z);

    if(b1>0) q[3]=0-a1;
    else q[3]=a1;

    if(b<0) q[3] = 0-a1; //debug: True
    else q[3] = a1; //debug: True
    if(b1<0) q[3] = 0-a1; //debug: True
    else q[3] = a1; //debug: True

	if(isnan(asin(sqrt(pow(frame1X, 2) + pow(frame1Z, 2)) / (arm_link[4]+arm_link[5])))) {
		if(printErrors) printf("q5 error: can't solve it\n");
		isReachable = false;
	}
	else q[4]=asin(sqrt(pow(frame1X, 2) + pow(frame1Z, 2)) / (arm_link[4]+arm_link[5]));

	if(true) {
		q[4]=asin(sqrt(pow(frame1X,2) + pow(frame1Z,2)) / (arm_link[4]+arm_link[5]));
		if(frame1Z<0) {
			q[4]=0-q[4];
			if(printText) printf("frame1_z<0  =>  q5=0-q5\n");
		}
		if(b<=M_PI/2 && b>=0-M_PI/2) q[5]=Y-q[3];
		else if(b>=M_PI/2 || b<=0-M_PI/2) q[5]=M_PI-(Y-q[3]);
	}
	for(int i=0; i<6; i++) {
		if(printText) printf(" %d",int(round(toDegrees(q[i]))));
		if(isnan(q[i])) {
			isReachable = false;
			if(printErrors) printf(" q[%d] is Not a Number\n",i);;
		}	
	}

	if(mode==0) {}
	else if(mode==1) {
		for(int i=0; i<6; i++) {
			q[i] = toDegrees(q[i]);
            solved_q[i] = q[i];
		}
	}	
	

	return isReachable;
}

/// @brief find a valid yaw/pitch orient for given 3-axis coordinate values
/// @param PP 3-axis coordinate values. {`x`, `y`, `z`}
/// @param currOrient current tilt orient
/// @param retOrient pointer to array container for result orients.
/// @param qAngles servo motor angles to find valid orient for
/// @return whether the function managed to find a valid orient
bool findValidOrient(float PP[3], float currOrient[3], float retOrient[2], float qAngles[6]) {
    int B_dir = 0;
    int orientAcc[2] = {10, 10};

    retOrient[0] = currOrient[0];
    retOrient[1] = currOrient[1];

    if(currOrient[1]<=0) B_dir = -1;
    else B_dir = 1;
    for(int alpha=0; alpha<=180; alpha+=orientAcc[0]) {
        for(int beta=0; beta<=180; beta+=orientAcc[1]) {
            
            if(
                (currOrient[0]+alpha) < 
                getAngles(qAngles, PP, toRadians(currOrient[0]+alpha),toRadians(currOrient[1]+beta*B_dir),toRadians(currOrient[3]), 1)) {
                retOrient[0] = currOrient[0] + alpha;
                retOrient[1] = currOrient[1] + beta*B_dir;
                return true;
            }
            if(getAngles(qAngles, PP, toRadians(currOrient[0]-alpha),toRadians(currOrient[1]+beta*B_dir),toRadians(currOrient[3]), 1)) {
                retOrient[0] = currOrient[0] - alpha;
                retOrient[1] = currOrient[1] + beta*B_dir;
                return true;
            }
        }
        for(int beta=0; beta<=180; beta+=orientAcc[1]) {
            if(getAngles(qAngles, PP, toRadians(currOrient[0]+alpha),toRadians(currOrient[1]+beta*(-B_dir)),toRadians(currOrient[3]), 1)) {
                retOrient[0] = currOrient[0] + alpha;
                retOrient[1] = currOrient[1] + beta*(-B_dir);
                return true;
            }
            if(getAngles(qAngles, PP, toRadians(currOrient[0]-alpha),toRadians(currOrient[1]+beta*(-B_dir)),toRadians(currOrient[3]), 1)) {
                retOrient[0] = currOrient[0] - alpha;
                retOrient[1] = currOrient[1] + beta*(-B_dir);
                return true;
            }
        }
    }
    return false;
}