
#pragma once
#include <unistd.h>
#include <PiPCA9685/PCA9685.h>
#include <iostream>
#include <string>
#include <math.h>

using namespace std;
using namespace PiPCA9685;


float arm_link[6] = {145, 130, 75, 50, 25, 25};
float sLoadWeight[6] = {0, 0.130, 0.085, 0, 0, 0}; //kg
float offset_q[6] = {90, 0, 135, 90, 90, 90};
float startup_q[6] = {0, 115, -90, 0, -25, 00};

float toDegrees(float radians) { return (radians*180)/M_PI; }
float toRadians(float degrees) { return (degrees*M_PI)/180; }

float get3dDistance(float p1[3], float p2[3]) { return sqrt(pow(p2[0]-p1[0],2) + pow(p2[1]-p1[1],2) + pow(p2[2]-p1[2],2)); }

int PoN(float var) {
	if(var>0) return 1;
	else if(var<0) return -1;
	else {
		cout << "PoN(): \"" << var << "\" is not a number\n";
		return 0;
	}
}

void add_defaults(float angles[6]) {
    angles[0] = offset_q[0] + angles[0];
    angles[1] = offset_q[1] + angles[1];
    angles[2] = 180 - (offset_q[2] + angles[2]);
    angles[3] = offset_q[3] + angles[3];
    angles[4] = 180 - (offset_q[4] + angles[4]);
    angles[5] = offset_q[5] + angles[5];
}

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
    angles[1] = angles[1] * error_Consts[1] + 15;
    angles[2] = angles[2] * error_Consts[2];
    angles[3] = angles[3] * error_Consts[3];
    angles[4] = angles[4] * error_Consts[4];
    angles[5] = angles[5] * error_Consts[5];
}

bool exceedCheck(float angles[6], bool printErrors=false) {
    bool exceeded = false;
    bool whichExceeded[6] = {false, false, false, false, false, false};
    string typeOfExceeded[6] = {"null", "null", "null", "null", "null", "null"};

    for(int i=0; i<5; i++) {
        if(angles[i]<0) { exceeded=true; whichExceeded[i]=true; typeOfExceeded[i]="under"; angles[i]=0; }
        if(angles[i]>180) { exceeded=true; whichExceeded[i]=true; typeOfExceeded[i]="over"; angles[i]=180;}
    }
    if(exceeded && printErrors) {
        for(int i=0; i<6; i++) { if(whichExceeded[i]) printf(" servo %d exceeded by: %s\n",i, typeOfExceeded[i]); }
    }
    if(exceeded) return true;
    else return false;   
}

float findVal(float arrToCheck[], int arrLen, int mode=0) {
    /*
    mode:
    - 0 - biggest, value
    - 1 - smallest, value
    - 2 - biggest, index
    - 3 - smallest, index
    */
    float val = arrToCheck[0];
    int index = 0;
    for(int i=0; i<arrLen; i++) {
        if(mode==0 || mode==2) if(arrToCheck[i]>val) { val=arrToCheck[i]; index=i; }
        else if(mode==1 || mode==3) if(arrToCheck[i]<val) {val=arrToCheck[i]; index=i; }
    }
    if(mode==0 || mode==1) return val;
    else if(mode==2 || mode==3) return index;
    else return -1;
}


float mp1(float x) {
    float y=0, V_max=2, t3=1;
    float Pt1=0.5;
    float Pt2=Pt1;
    float t2=t3*Pt2;
    float t1=t3*Pt1;
    float a3 = (0-V_max)/(t3-t2);
    float a1 = (V_max-0)/(t1-0);
    if(x>0 && x<=t1) y=(a1*pow(x,2))/2;
    else if(x>t1 && x<t2) y=a1*t1*x-(a1*pow(t1,2))/2;
    else if(x>=t2 && x<=t3) y=(a3*pow(x-t2,2))/2+V_max*x+a1*(t1*t2-pow(t1,2)/2)-V_max*t2;
    return y;
}

int sendToServo(
    PCA9685* pcaBoard,
    float new_rotation[6],
    float old_rotation[6],
    bool servoInitialize,
    int mode = 0,
    float totalTime = 0,
    bool useDefault = true,
    bool printErrors = true,
    bool printResult = false
    ){/*
        When the function is ran for the first time and servoInitialize is one, an empty array
        needs to be entered that will be used in *EVERY* call of sendToServo
        */
        printResult = false;
	   	int returnCode = 0;

        if(servoInitialize) {
            for(int i=0; i<6; i++) {new_rotation[i] = startup_q[i];}
        }

        if(useDefault) add_defaults(new_rotation);

        q_corrections(new_rotation);
        if(exceedCheck(new_rotation, printErrors)) {}//return -1;
        if(printResult) {
            printf("sent: ");
            for(int i=0; i<6; i++) { printf("%f ", new_rotation[i]); }
            printf("\n");
        }

        int total_iteration = 135;
        if(mode==0) {
            for(int q=0; q<6; q++) {
                pcaBoard->set_pwm(q, 0, round(400*(float(new_rotation[q])/180))+100);
                old_rotation[q] = new_rotation[q];
            }
        }
        if(mode==1 || mode==2) {
            float s_diff[6];
            float s_temp[6];
            for(int i=0; i<6; i++) {
                if(!servoInitialize) {
                    s_diff[i] = new_rotation[i] - old_rotation[i];
                    s_temp[i] = old_rotation[i];
                }
                else if(servoInitialize) {
                    old_rotation[i] = startup_q[i];
                    s_diff[i] = new_rotation[i]-old_rotation[i];
                    s_temp[i] = old_rotation[i];
                }
                if(printResult) { printf("s_diff[%d]: %f\n",i, s_diff[i]); }
            }
            
            total_iteration = int(findVal(s_diff, sizeof(s_diff)/sizeof(int),0));
            if(printResult) printf("total_iteration:%d\n",total_iteration);
            for(int count=0; count<total_iteration-1; count++) {
                float val;
                for(int q=0; q<6; q++) {
                    if(mode==1) {
                        val = s_diff[q]/total_iteration;
                        s_temp[q]+=val;
                        pcaBoard->set_pwm(q, 0, round(400*(s_temp[q]/180))+100);
                    }
                    else if(mode==2) { 
                        val = s_temp[q] + s_diff[q]*mp1(float(count)/total_iteration);
                        pcaBoard->set_pwm(q, 0, round(400*(val/180))+100);
                    }
                    old_rotation[q] = val;
                    if(printResult) printf("val:%f\n",val);
                }
                if(totalTime>0.1) usleep(int(totalTime/total_iteration*1'000'000));
            }
        }
        if(printResult) {
            printf("received: ");
            for(int i=0; i<6; i++) {printf("%f ",old_rotation[i]);}
            printf("\n");
        }
        return returnCode;
    }


// Inverse Kinematics - specific functions:
bool getAngles(
	float q[6],
	float PP[3],
	float a, float b, float Y,
	// bool* positionIsReachable,
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

	if(true) {
		if(b1>0) q[3]=0-a1;
		else q[3]=a1;
	}
	

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
			if(printErrors) printf(" q%d is Not a Number\n",i+1);;
		}	
	}

	if(mode==0) {}
	else if(mode==1) {
		for(int i=0; i<6; i++) {
			q[i] = toDegrees(q[i]);
		}
	}	
	
	return isReachable;
}


