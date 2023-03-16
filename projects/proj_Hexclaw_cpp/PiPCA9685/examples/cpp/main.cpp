#include <unistd.h>
#include <PiPCA9685/PCA9685.h>
#include <iostream>
#include <math.h>

using namespace std;

int main() {
  //Max 530
  PiPCA9685::PCA9685 pca{};

  int sm_pin = 2;
  pca.set_pwm_freq(50.0);

  // pca.set_pwm(sm_pin, 0, 300);
  // usleep(1'000'000);
  // pca.set_pwm(sm_pin, 0, 100);
  // usleep(1'000'000);

  pca.set_pwm(1, 0, 280);

  // for(int i=0;i<=100;i+=10) {
  //   cout << i << endl;
  //   pca.set_pwm(1, 0, i);
  //   usleep(1'000'000);
  // }

  // for(int i=100;i<500;i++) {
  //   pca.set_pwm(sm_pin, 0, i);
  //   if (i>500 || i<100) {
  //     usleep(1'000'000);
  //     i+=9;
  //   }
  //   else if (i==300)
  //     usleep(2'000'000);
  //   else
  //     usleep(10'000);
  //   cout << i << endl;
  // }
  // usleep(1'000'000);
  // for(int i=500;i>100;i--) {
  //   pca.set_pwm(sm_pin, 0, i);
  //   if (i>500 || i<100) {
  //     usleep(1'000'000);
  //     i+=9;
  //   }
  //   else if (i==300)
  //     usleep(2'000'000);
  //   else
  //     usleep(10'000);
  //   cout << i << endl;
  // }  

  // pca.set_pwm(sm_pin, 0, 100);
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
