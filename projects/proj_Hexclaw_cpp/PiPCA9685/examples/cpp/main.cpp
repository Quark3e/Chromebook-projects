#include <unistd.h>
#include <PiPCA9685/PCA9685.h>
#include <iostream>
#include <math.h>

using namespace std;
// using namespace PiPCA9685;

// PCA9685 pca{};

void to_sendToServo(
  PiPCA9685::PCA9685* pcaBoard,
  // float new_rotation[6],
  int servoToMove,
  int angle
) {
  (*pcaBoard).set_pwm(servoToMove, 0, round(400*(float(angle)/180))+100);
  printf("angle: %d\n", angle);
}


int main() {
  //Max 530
  PiPCA9685::PCA9685 pca{};
  pca.set_pwm_freq(50.0);

  int sm_pin = 3;
  string input;
  while (true) {
    cout << "enter servo pin to test: ";
    cin >> input;
    if(input=="exit") break;
    else sm_pin = stoi(input); 
    
    for(int i=0; i<=180; i++) {
      sendToServo(&pca, sm_pin, i);
      usleep(10'000);
    }
    usleep(1'000'000);
    for(int i=180; i>=0; i--) {
      sendToServo(&pca, sm_pin, i);
      usleep(10'000);
    }

    // pca.set_pwm(1, 0, 280);

    // usleep(1'000'000);

    // for(int i=0;i<=180;i+=45) {
    //   cout << i << endl;
    //   pca.set_pwm(sm_pin, 0, round(400*(float(i)/180))+100);
    //   usleep(1'000'000);
    // }
    // for(int i=180;i>=0;i-=45) {
    //   cout << i << endl;
    //   pca.set_pwm(sm_pin, 0, round(400*(float(i)/180))+100);
    //   usleep(1'000'000);
    // }
    // for(int i=0;i<=180;i+=1) {
    //   cout << i << endl;
    //   pca.set_pwm(sm_pin, 0, round(400*(float(i)/180))+100);
    //   usleep(10'000);
    // }
    // for(int i=180;i>=0;i-=1) {
    //   cout << i << endl;
    //   pca.set_pwm(sm_pin, 0, round(400*(float(i)/180))+100);
    //   usleep(10'000);
    // }
    usleep(500'000);
    to_sendToServo(&pca, sm_pin, 90);
  }
}
