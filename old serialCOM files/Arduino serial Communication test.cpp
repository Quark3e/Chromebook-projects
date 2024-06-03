
#include <sfml/Graphics.hpp>
#include <sfml/Audio.hpp>
#include <opencv2/opencv.hpp>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/objdetect.hpp"
#include <string>
#include <stdlib.h>
#include <math.h>
#include <cmath>
#include <sstream>
#include <fstream>
#include <thread>
#include <vector>
#include <stdlib.h>
#include "SerialPort.h"
#include <iostream>

# define PI           3.14159265358979323846  /* pi */

char output[MAX_DATA_LENGTH];
char incomingData[MAX_DATA_LENGTH];

using namespace std;
using namespace cv;
using namespace sf;


//arm length definitions
float d1 = 140; //axial "roll"
float d2 = 135; //axial "pitch"
float d3 = 70; //axial "pitch"
float d4 = 80; //axial "roll"
float d5 = 45; //axial "pitch
float d6 = 30; //axial "roll" (?)

//char temp;
char* port;

float valScalar = 0.8888;

void addNULL_char(string data, char* charArray) {
	//char* charArray = new char[data.size() + 2];

	copy(data.begin(), data.end(), charArray);
	charArray[data.size()] = '\n';
	//charArray[data.size()] = ';';
	//cout << charArray << endl;
}

#include "functions.h"
#include "inverse_kinematics.h"
#include "7DOF_math.h"
#include "options.h"

int main() {
	
	string input;

	int COM_num;
	string temporaryCOM = "\\\\.\\COM";

	cout << "Enter COM port: ";
	getline(cin, input);
	if (input == "exit")
		return 0;
	COM_num = stoi(input);

	temporaryCOM += to_string(COM_num);
	char temp[9];
	for (int i = 0; i < 9; i++)
		temp[i] = temporaryCOM[i];

	port = temp;

	cout << port << endl;


	while (true) {

		cout << "\n0.Give position to 7 DOF robot arm\n\n";
		cout << " 1. Send direct degrees(3 DOF)\n";
		cout << " 2. Send xyz coords (3 DOF)\n";
		cout << " 3. Use coords from accelerometer (3DOF robot arm)\n";
		cout << " 4. Send rotational degrees directly(7DOF)\n";
		cout << " 5. Send xyz coords (7 DOF)\n";
		cout << " 6. Change a, b, Y offset angles at a given position\n";
		cout << " 7. Use coords from accelerometer (with 7 DOF robot arm)\n";
		cout << " 8. hexclaw project arm workspace stuff\n";
		cout << " 9. Run 7DOF robot arm with opencv\n";
		cout << " 10.openCV acc arm acc test: Run robot arm from .dat file with given coords and determine the position\n";
		cout << " 11.Run servo rotation commands from .txt. file\n";

		cout << "input: ";
		cin >> input;
		if (input == "exit") { return 0; }

		if (stoi(input) == 0) { coord_to_7DOF(); }
		else if (stoi(input) == 1) { sendDegrees(); }
		else if (stoi(input) == 2) { sendCoords(); }
		else if (stoi(input) == 3) { useAccCommmands_3DOF(); }
		else if (stoi(input) == 4) { sendDegrees_7DOF(); }
		else if (stoi(input) == 5) { sendCoords_7DOF(); }
		else if (stoi(input) == 6) { testOffsetAngles_7DOF(); }
		else if (stoi(input) == 7) { useAccCommmands_7DOF(); }
		else if (stoi(input) == 8) { hexclaw_ws_stuff(); }
		else if (stoi(input) == 9) { Robotarm_CV_control(); }
		else if (stoi(input) == 10) { openCV_armAccuracy_test(); }
		else if (stoi(input) == 11) { runServoDegrees_fromFile(); }

	}

	return 0;
}