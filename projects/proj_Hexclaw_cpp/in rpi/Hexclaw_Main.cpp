#include <unistd.h>
#include <PiPCA9685/PCA9685.h>
#include <iostream>
#include <math.h>

#include "IK_header.h"

using namespace std;

int main() {
	PiPCA9685::PCA9685 pca{};
	pca.set_pwm_freq(50.0);

	float current_q[6] = {0,0,0,0,0,0}; //old_rotation
	float new_q[6] = {0,0,0,0,0,0};
	float PP[3] = {0, 150, 150};

	sendToServo(&pca, new_q, current_q, true, 0);

	sleep(2);
	if(getAngles(new_q,PP,0,0,0,1)) {	
		printf("angles: ");
		for(int i=0; i<6; i++) { printf(" %f",new_q[i]); }
		printf("\n");
		sendToServo(&pca, new_q, current_q, false, 2, 2, true, true, false);
	}
	else printf("position not reachable\n");
	cout << endl;

	return 0;
}
