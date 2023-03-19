#include <unistd.h>
#include <PiPCA9685/PCA9685.h>
#include <iostream>
#include <math.h>

using namespace std;
using namespace PiPCA9685;

int main() {
  //Max 530
  PCA9685 pca{};

  int sm_pin = 2;
  pca.set_pwm_freq(50.0);

  pca.set_pwm(1, 0, 280);

  usleep(1'000'000);

  for(int i=0;i<=180;i+=45) {
    cout << i << endl;
    pca.set_pwm(sm_pin, 0, round(400*(float(i)/180))+100);
    usleep(1'000'000);
  }
  for(int i=180;i>=0;i-=45) {
    cout << i << endl;
    pca.set_pwm(sm_pin, 0, round(400*(float(i)/180))+100);
    usleep(1'000'000);
  }
  for(int i=0;i<=180;i+=1) {
    cout << i << endl;
    pca.set_pwm(sm_pin, 0, round(400*(float(i)/180))+100);
    usleep(10'000);
  }
  for(int i=180;i>=0;i-=1) {
    cout << i << endl;
    pca.set_pwm(sm_pin, 0, round(400*(float(i)/180))+100);
    usleep(10'000);
  }
  
}
