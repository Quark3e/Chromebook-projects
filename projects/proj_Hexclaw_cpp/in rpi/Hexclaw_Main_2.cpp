#include <unistd.h>
#include <PiPCA9685/PCA9685.h>
#include <iostream>
#include <math.h>

#include "IK_header.h"

using namespace std;
using namespace PiPCA9685;


int main() {
	PiPCA9685 pca();
	float current_q[6] = {0,0,0,0,0,0};
	float new_q[6];
	float PP[3] = {0, 150, 150};
	getAngles(new_q,PP,0,0,0,1);
	
	printf("angles: ");
	for(int i=0; i<6; i++) {
		printf(" %f",new_q[i]);
	}

	return 0;
}
