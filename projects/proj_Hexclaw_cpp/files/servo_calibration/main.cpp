
#include <iostream>
#include <math.h>
#include <stdio.h>
#include <string>
#include <unistd.h>
#include <fstream>
#include <cstdlib>
#include <chrono>
#include <ctime>

#include "PiPCA9685/PCA9685.h"

using namespace std;
using namespace PiPCA9685;

void sendToServo(
    PCA9685* pcaBoard,
    int servoToMove,
    int angle
) {
    pcaBoard->set_pwm(servoToMove, 0, round(400*(float(angle)/180))+100);
}

float readAngles[6][18];


int main(int argc, char** argv) {
    ofstream delayFile;
    delayFile.open("readDelays.dat", ios_base::app);

    for(int i=0; i<6; i++) {for(int j=0; j<18; j++) {readAngles[i][j]=0;}}

    PCA9685 pca{};
    pca.set_pwm_freq(50.0);

    int angleDefaults[6] = {90, 90, 45, 90, 90, 90};
    float readValues[18];
    int servoToMove=0;
    string input;
    while(true) {
        for(int i=0; i<6; i++) sendToServo(&pca, i, angleDefaults[i]);
        cout << "--- Servo motor calibration ---\n";
        cout << "enter servo to test [0-5]: ";
        cin >> input;
        if(input=="exit") break;
        servoToMove = stoi(input);
        cin.clear();
        cin.ignore();

        printf("\nprelim. angles sent:\t");
        for(int angle=0; angle<=180; angle+=10) {
            printf(" %d", angle);
            sendToServo(&pca, servoToMove, angle);
            usleep(750'000);
        }
        usleep(1'000'000);
        printf("\ntrack. angles:\n");
        for(int i=0; i<=18; i+=1) {
            printf("sent: %d read: ", i*10);
            sendToServo(&pca, servoToMove, i*10);
            cin >> input;
            if(input=="exit") return 0;
            readValues[i] = stof(input);
            readAngles[servoToMove][i] = readValues[i];
            cin.clear();
            cin.ignore();
        }

        printf("\n\nresults:\nsent: ");
        for(int i=0; i<18; i++) { printf(" %d", i*10); }

        printf("\nread: ");
        for(int i=0; i<18; i++) { printf(" %d", int(round(readValues[i]))); }
        printf("\npaused: ");
        cin.get();
        cin.clear();
        cin.ignore();
        system("clear");
        sendToServo(&pca, servoToMove, angleDefaults[servoToMove]);
    }

    auto currentTime = chrono::system_clock::now();
    time_t currentDate = chrono::system_clock::to_time_t(currentTime);

    delayFile << "date:\"" + ctime(&currentDate) + "\";\n";
    for(int q=0; q<6; q++) {
        delayFile << to_string(q) + ":" + to_string(readAngles[q][0]);
        for(int i=1; i<18; i++) delayFile << "," + to_string(readAngles[q][i]);
        delayFile << ";\n";
    }

    delayFile.close();

    return 0;
}