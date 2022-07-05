#pragma once

void sendDegrees() {

	SerialPort arduino(port);
	if (arduino.isConnected()) {
		cout << "Connection established" << endl << endl;
	}
	else {
		cout << "Error in port name" << endl << endl;
	}
	while (arduino.isConnected()) {

		//for (int i = 0; i < 180; i += 10) {

		//}

		while (true) {
			cout << "Enter your command/degrees by separating with ':' (in degrees 0 to 180 in integers): " << endl;
			string data;
			cin >> data;
			if (data == "exit")
				return;


			//copy(data.begin(), data.end(), charArray);
			//charArray[data.size()] = '\n';
			//charArray[data.size() + 1] = '\0';
			//cout << "char array: " << charArray << endl;

			char* charArray = new char[data.size() + 1];
			addNULL_char(data, charArray);

			arduino.writeSerialPort(charArray, data.size() + 1);
			delete[] charArray;

			Sleep(10);
			arduino.readSerialPort(output, MAX_DATA_LENGTH);

			cout << ">> " << output << endl;
			cout << "----------\n" << endl;

		}
	}
}

void sendCoords() {

	SerialPort arduino(port);
	if (arduino.isConnected()) {
		cout << "Connection established" << endl << endl;
	}
	else {
		cout << "Error in port name" << endl << endl;
	}
	while (arduino.isConnected()) {

		//for (int i = 0; i < 180; i += 10) {

		//}

		while (true) {
			float scalar = 1;
			float arm1 = 100, arm2 = 101;
			string input;
			//cout << "Enter scalar: ";
			//cin >> input;
			//if (input == "exit")
				//return;
			//scalar = stof(input);
			//cout << "\nEnter length of arm1 (joint 1 to joint 2): ";
			//cin >> arm1;
			//cout << "\nEnter length of arm2 (joint 2 to tip): ";
			//cin >> arm2;
			cout << "\nEnter coords (x(left,right), y(forward, backward) and z(up and down)) separeted with a space symbol inbetween: " << endl;
			string x, y, z;
			cin >> x >> y >> z;

			float xDefault = 0, yDefault = 0, zDefault = 0;
			float c;
			float q_1, q_2, q_yaw; //joint angles 1 and 2, and angle for yaw base.
			int q1_default = 0, q2_default = 135, qYaw_default = 90;;

			q_yaw = yawServo(stof(x), stof(y), &c, scalar);
			cout << "q_yaw: " << q_yaw << endl;
			getJoint_2_angle(
				arm1,
				arm2,
				c,
				stof(z),
				&q_2,
				"negative",
				scalar
			);
			getJoint_1_angle_negative(
				arm1,
				arm2,
				c,
				stof(z),
				q_2,
				&q_1,
				scalar
			);

			cout << "q1: " << q_1 << endl;
			string data;
			data = to_string(int(round(qYaw_default + q_yaw))) + ":" + to_string(int(round(q1_default + q_1))) + ":" + to_string(int(round(q2_default + q_2)));
			cout << "q2: " << q_2 << endl;
			//copy(data.begin(), data.end(), charArray);
			//charArray[data.size()] = '\n';
			//charArray[data.size() + 1] = '\0';
			//cout << "char array: " << charArray << endl;
			cout << "Degrees: " << data << endl;

			char* charArray = new char[data.size() + 1];
			addNULL_char(data, charArray);

			arduino.writeSerialPort(charArray, data.size() + 1);
			delete[] charArray;

			Sleep(10);
			arduino.readSerialPort(output, MAX_DATA_LENGTH);

			cout << ">> " << output << endl;
			cout << "----------\n" << endl;
			//cout << "Size of \"output\": " << sizeof(output) / sizeof(output[0]) << endl;

		}
	}
}

void useAccCommmands_3DOF() {
	system("cls");
	
	SerialPort arduino(port);

	if (arduino.isConnected()) {
		cout << "Connection established" << endl << endl;
	}
	else {
		cout << "Error in port name and/or unable to connect to port" << endl << endl;
		return;
	}
	
	float scalar = 1;
	float arm1 = 100, arm2 = 101;
	string x, y, z;
	//float pitch, yaw, 
	Vector3f coordChange; //change in coords
	Vector2f tiltChange; //x - pitch, y - roll

	Vector3f coordChange_factor = Vector3f(1, 1, 1);


	//cout << "Enter start coordinates (in X Y Z format): ";
	//cin >> x >> y >> z;
	//cout << endl;

	//setting base coords
	x = "0";
	y = to_string(int(round((arm1 + arm2) * 0.3)));
	z = to_string(int(round((arm1 + arm2) * 0.3)));

	Vector3f XYZ = Vector3f(stof(x), stof(y), stof(z));

	while (arduino.isConnected()) {
		while (true) {

			string input;
			//cout << "Enter scalar: ";
			//cin >> input;
			//if (input == "exit")
				//return;
			//scalar = stof(input);
			//cout << "\nEnter length of arm1 (joint 1 to joint 2): ";
			//cin >> arm1;
			//cout << "\nEnter length of arm2 (joint 2 to tip): ";
			//cin >> arm2;

			float xDefault = 0, yDefault = 0, zDefault = 0;
			float c;
			float q_1, q_2, q_yaw; //joint angles 1 and 2, and angle for yaw base.
			int q1_default = 0, q2_default = 135, qYaw_default = 90;;

			q_yaw = yawServo(XYZ.x, XYZ.y, &c, scalar);
			cout << "q_yaw: " << q_yaw << endl;
			getJoint_2_angle(
				arm1,
				arm2,
				c,
				XYZ.z,
				&q_2,
				"negative",
				scalar
			);
			getJoint_1_angle_negative(
				arm1,
				arm2,
				c,
				XYZ.z,
				q_2,
				&q_1,
				scalar
			);

			cout << "q1: " << q_1 << endl;
			string data;
			data = to_string(int(round(qYaw_default + q_yaw))) + ":" + to_string(int(round(q1_default + q_1))) + ":" + to_string(int(round(q2_default + q_2)));
			cout << "q2: " << q_2 << endl;
			//copy(data.begin(), data.end(), charArray);
			//charArray[data.size()] = '\n';
			//charArray[data.size() + 1] = '\0';
			//cout << "char array: " << charArray << endl;
			cout << "Degrees: " << data << endl;

			char* charArray = new char[data.size() + 1];
			addNULL_char(data, charArray);

			arduino.writeSerialPort(charArray, data.size() + 1);
			delete[] charArray;

			Sleep(10);
			arduino.readSerialPort(output, MAX_DATA_LENGTH);

			updateArduino_output(&coordChange, &tiltChange);

			XYZ.x += coordChange.x * coordChange_factor.x + sin(toRadians(tiltChange.y)) * coordChange.z * coordChange_factor.z;
			XYZ.y += coordChange.y * coordChange_factor.y + sin(toRadians(tiltChange.x)) * coordChange.z * coordChange_factor.z;
			XYZ.z += coordChange.z * coordChange_factor.z + sin(toRadians(tiltChange.x)) * coordChange.y * coordChange_factor.y + sin(toRadians(tiltChange.y)) * coordChange.x * coordChange_factor.x;
			
			//cout << "x:" << XYZ.x << " y:" << XYZ.y << " z:" << XYZ.z << "\n";
			printf("x:%f y:%f z:%f\n", XYZ.x, XYZ.y, XYZ.z);

			//cout << ">> " << output << endl;
			//cout << "----------\n" << endl;

			//cout << "Size of \"output\": " << sizeof(output) / sizeof(output[0]) << endl;

		}
	}
}

void sendDegrees_7DOF() {
	SerialPort arduino(port);

	if (arduino.isConnected()) {
		cout << "Connection established" << endl << endl;
		Sleep(500);
	}
	else {
		cout << "Error in port name" << endl << endl;
		system("pause");
	}
	system("cls");
	string data;
	int q1_default = 90, //in degrees
		q2_default = 0, //in degrees
		q3_default = 135, //in degrees
		q4_default = 90, //in degrees
		q5_default = 90, //in degrees
		q6_default = 1; //in degrees
	int q1 = 0, q2 = 0, q3 = 0, q4 = 0, q5 = 0, q6 = 0;

	int servoSpeed = 100;
	string useTimer = "true";

	while (true) {
		printf("Enter rotational commands of each servo by using spaces between each number\n\ninput (q1 q2 q3 q4 q5 q6): ");
		cin >> q1 >> q2 >> q3 >> q4 >> q5 >> q6;
		printf("\nenter servo speed in for() loop iteration (integer): ");
		cin >> servoSpeed;

		data = (
			to_string(q1) + ":" +
			to_string(q2) + ":" +
			to_string(q3) + ":" +
			to_string(q4) + ":" + //note: it's set to be "reverse" because the playing field is in "reverse"
			to_string(q5) + ":" +
			to_string(q6) + "s" +
			to_string(servoSpeed) + "t" +
			useTimer
			);
		//cout << "---- sent:" << data << " with a:" << toDegrees(a) << " b:" << toDegrees(b) << " Y:" << toDegrees(Y) << endl;
		printf("--- sent:%s\n", data.c_str());
		char* charArray = new char[data.size() + 1];
		addNULL_char(data, charArray);

		arduino.writeSerialPort(charArray, data.size() + 1);
		delete[] charArray;
		arduino.readSerialPort(output, MAX_DATA_LENGTH);
		cout << ">> " << output << endl;
		cout << "----------\n" << endl;

	}
}

void sendCoords_7DOF_sendCoords(Vector3f PP, char posOption, SerialPort* arduino, float a, float b, float Y, bool printText = true, int servoSpeed = 100, string useTimer = "false") {
	//Note: port must be connected
	float scalar = 1;
	float l;
	float a1, b1;
	Vector3f P2; //PP is coords entered; P2 is coords to cutoff
	float xDefault = 0, yDefault = 0, zDefault = 0;
	//servo default declaration and definition
	int q1_default = 90, //in degrees
		q2_default = 0, //in degrees
		q3_default = 135, //in degrees
		q4_default = 90, //in degrees
		q5_default = 90, //in degrees
		q6_default = 90; //in degrees

	float q1 = q1_default,
		q2 = toRadians(q2_default),
		q3 = toRadians(q3_default),
		q4 = toRadians(q4_default),
		q5 = toRadians(q5_default),
		q6 = toRadians(q6_default); //in radians
	float q1_holder, q2_holder, q3_holder, q4_holder, q5_holder, q6_holder; //in radians


	float coord_scalar = 1;
	float length_scalar = 1;

	{
		//PP.x = coord_scalar * (PP.x + xDefault);
		//PP.y = coord_scalar * (PP.y + yDefault);
		//PP.z = coord_scalar * (PP.z + zDefault);

		//d1 = d1 * length_scalar;
		//d2 = d2 * length_scalar;
		//d3 = d3 * length_scalar;
		//d4 = d4 * length_scalar;
		//d5 = d5 * length_scalar;
		//d6 = d6 * length_scalar;

		////l = d4 * cos(b);
		//l = d5 * cos(b);
		//P2.x = PP.x - l * sin(a);
		//P2.y = PP.y - l * cos(a);
		////P2.z = PP.z - d4 * sin(b);
		//P2.z = PP.z - d5 * sin(b);
		//q1_holder = atan(P2.x / P2.y);
		//if (P2.y < 0) { q1_holder += PI; }

		//if (!isnan(q1_holder))
		//	q1 = q1_holder;
		//if (posOption == '-') {
		//	q3_holder = q3_negative(P2, d1, d2, d3);
		//	if (!isnan(q3_holder))
		//		q3 = q3_holder;
		//	q2_holder = q2_negative(P2, d1, d2, d3, q3, &q2);
		//	if (!isnan(q2_holder))
		//		q2 = q2_holder;
		//}
		//else if (posOption == '+') {
		//	q3_holder = q3_positive(P2, d1, d2, d3);
		//	if (!isnan(q3_holder))
		//		q3 = q3_holder;
		//	q2_holder = q2_positive(P2, d1, d2, d3, q3, &q2);
		//	if (!isnan(q2_holder))
		//		q2 = q2_holder;
		//}
		//a1 = get_a1(a, q1);
		//b1 = get_b1(b, q2, q3);

		////q4_holder = q4_func(get_d4ry(d4, b1, a1), get_d4rz(d4, b1));
		//q4_holder = q4_func(get_d5x(d5, b1, a1), get_d5z(d5, b1));
		//if (!isnan(q4_holder))
		//	q4 = q4_holder;
		////q5_holder = q5_func(get_d4ry(d4, b1, a1), get_d4rz(d4, b1), b1, a1);
		//q5_holder = q5_func(get_d5x(d5, b1, a1), get_d5z(d5, b1), b1, a1);

		//if (!isnan(q5_holder))
		//	q5 = q5_holder;
		//q6_holder = q6_func(Y, q4, b);
		//if (!isnan(q6_holder))
		//	q6 = q6_holder;
	}
	get_Angles(
		PP,
		&q1,
		&q2,
		&q3,
		&q4,
		&q5,
		&q6,
		&a1,
		&b1,
		&P2,
		a,
		b,
		Y,
		posOption,
		length_scalar,
		coord_scalar,
		printText
	);
	//cout << "-------------------q5:" << toDegrees(q5) << endl;
	
	if (q1_default - int(round(toDegrees(q1))) < 0) { q1 = toRadians(0 + q1_default); }
	if (q2_default + int(round(toDegrees(q2))) < 0) { q2 = toRadians(0 - q2_default); }
	if (q3_default + int(round(toDegrees(q3))) < 0) { q3 = toRadians(0 - q3_default); }
	if (q4_default + int(round(toDegrees(q4))) < 0) { q4 = toRadians(0 - q4_default); }
	if (q5_default + int(round(toDegrees(q5))) < 0) { q5 = toRadians(0 - q5_default); }
	if (q6_default + int(round(toDegrees(q6))) < 0) { q6 = toRadians(0 - q6_default); }
	if (q1_default - int(round(toDegrees(q1))) > 180) { q1 = toRadians(0 - q1_default); }
	if (q2_default + int(round(toDegrees(q2))) > 180) { q2 = toRadians(180 - q2_default); }
	if (q3_default + int(round(toDegrees(q3))) > 180) { q3 = toRadians(180 - q3_default); }
	if (q4_default + int(round(toDegrees(q4))) > 180) { q4 = toRadians(180 - q4_default); }
	if (q5_default + int(round(toDegrees(q5))) > 180) { q5 = toRadians(180 - q5_default); }
	if (q6_default + int(round(toDegrees(q6))) > 180) { q6 = toRadians(180 - q6_default); }

	string data;
	data = (
		to_string(q1_default - int(round(toDegrees(q1)))) + ":" +
		to_string(q2_default + int(round(toDegrees(q2)))) + ":" +
		to_string(q3_default + int(round(toDegrees(q3)))) + ":" +
		to_string(q4_default + int(round(toDegrees(q4)))) + ":" + //note: it's set to be "reverse" because the playing field is in "reverse"
		to_string(q5_default + int(round(toDegrees(q5)))) + ":" +
		to_string(q6_default + int(round(toDegrees(q6)))) + "s" +
		to_string(servoSpeed) + "t" +
		useTimer
		);
	//cout << "---- sent:" << data << " with a:" << toDegrees(a) << " b:" << toDegrees(b) << " Y:" << toDegrees(Y) << endl;
	if(printText)
		printf("--- sent:%s\n", data.c_str());

	char* charArray = new char[data.size() + 1];
	addNULL_char(data, charArray);
	(*arduino).writeSerialPort(charArray, data.size() + 1);
	delete[] charArray;
	(*arduino).readSerialPort(output, MAX_DATA_LENGTH);
	//cout << "data: " << data << endl;
}

bool areAngles_NaN(Vector3f PP, float a, float b, char posOption) {
	//in radians
	bool ans = false;
	Vector3f P2;
	float q1 = 0, q2 = 0, q3 = 0, q4 = 0, q5 = 0, q6 = 0;
	float a1, b1, Y = 0; //in radians

	get_Angles(
		PP,
		&q1,
		&q2,
		&q3,
		&q4,
		&q5,
		&q6,
		&a1,
		&b1,
		&P2,
		a,
		b,
		Y,
		posOption,
		1,
		1,
		false
	);

	if (isnan(q1) ||
		isnan(q2) ||
		isnan(q3) ||
		isnan(q4) ||
		isnan(q5) ||
		isnan(q6)
		) {
		ans = true;
	}

	return ans;
} //if the ans is true then the workspace coord binary will be 0.

void solveNaN(Vector3f PP, float* a, float* b, bool* solvedNaN, bool posOption) {
	(*solvedNaN) = false;
	float a_origin = 1, b_origin = -90;

	for (int beta = b_origin; beta <= 90; beta++) {
		for (int alpha = -90; alpha <= 90; alpha++) {
			if (!areAngles_NaN(PP, toRadians(float(alpha)), toRadians(float(beta)), posOption)) {
				(*a) = toRadians(alpha);
				(*b) = toRadians(beta);
				beta = 91;
				alpha = 91;
				//printf("---alternative found\n");
				printf("alpha:%d beta:%d", alpha, beta);
				(*solvedNaN) = true;
				return;
			}
		}
	}

	for (int beta = b_origin; beta >= -90; beta--) {
		for (int alpha = -90; alpha <= 90; alpha++) {
			if (!areAngles_NaN(PP, toRadians(float(alpha)), toRadians(float(beta)), posOption)) {
				(*a) = toRadians(alpha);
				(*b) = toRadians(beta);
				beta = 0 - 91;
				alpha = 91;
				//printf("---alternative found\n");
				printf("alpha:%d beta:%d", alpha, beta);
				(*solvedNaN) = true;
				return;
			}
		}
	} 

} //don't initiate without having areAngles_NaN() return true

void robotArm_coordsToArduino(SerialPort* arduino, Vector3f PP = Vector3f(0, 100, 150), bool useAutoOrientation = true) {

	//variable declarations
	float scalar = 1;
	float axisTest_timer = 70;
	int axisTest_loopIncrement = 2;
	float runOffsetTest_timer = 40;
	float l;
	float a, b, Y;
	float a_origin = 1, b_origin = -45;
	a = toRadians(a_origin)/*yaw*/, b = toRadians(b_origin)/*pitch*/, Y = toRadians(0)/*roll*/;
	//float a1, b1;
	Vector3f P2; //PP is coords entered; P2 is coords to cutoff
	float xDefault = 0, yDefault = 0, zDefault = 0;
	float c;
	int servoSpeed;
	float q1 = toRadians(90),
		q2 = toRadians(0),
		q3 = toRadians(135),
		q4 = toRadians(90),
		q5 = toRadians(90),
		q6 = toRadians(1); //in radians
	float q1_holder, q2_holder, q3_holder, q4_holder, q5_holder, q6_holder;
	char posOption = '-'; //whether it be '+' or '-' for the type of angle q2 and q3 is. + is lore robot, - is typical gripper
	bool breakVal = false;
	bool reachable = true;

	a = toRadians(a_origin)/*yaw*/, b = toRadians(b_origin)/*pitch*/, Y = toRadians(0)/*roll*/;

	if (areAngles_NaN(PP, a, b, posOption)) {
		//printf("orientationvariables are incorrect. brute forcing run\n");
		if (useAutoOrientation) {
			breakVal = false;
			reachable = false;
			for (int beta = b_origin; beta <= 90; beta++) {
				for (int alpha = 0 - 90; alpha <= 90; alpha++) {
					if (!areAngles_NaN(PP, toRadians(float(alpha)), toRadians(float(beta)), posOption)) {
						a = toRadians(alpha);
						b = toRadians(beta);
						beta = 91;
						alpha = 91;
						//printf("---alternative found\n");
						printf("alpha:%d beta:%d", alpha, beta);
						breakVal = true;
						reachable = true;
					}
					if (breakVal) { break; }
				}
				if (breakVal) { break; }
			}

			for (int beta = b_origin; beta >= -90; beta--) {
				for (int alpha = 0 - 90; alpha <= 90; alpha++) {
					if (!areAngles_NaN(PP, toRadians(float(alpha)), toRadians(float(beta)), posOption)) {
						a = toRadians(alpha);
						b = toRadians(beta);
						beta = 0 - 91;
						alpha = 91;
						//printf("---alternative found\n");
						printf("alpha:%d beta:%d", alpha, beta);
						breakVal = true;
						reachable = true;
					}
					if (breakVal) { break; }
				}
				if (breakVal) { break; }
			}
		}
	}
	else {}
	if (reachable) {
		float coord_scalar = 1;
		PP.x = coord_scalar * (PP.x + xDefault);
		PP.y = coord_scalar * (PP.y + yDefault);
		PP.z = coord_scalar * (PP.z + zDefault);
		sendCoords_7DOF_sendCoords(PP, posOption, arduino, a, b, Y, false, 5, "true");
		Sleep(10);
		(*arduino).readSerialPort(output, MAX_DATA_LENGTH);
	}
	else {
		cout << "that coord is unreachable\n";
	}
}


void sendCoords_7DOF() {
	SerialPort arduino(port);

	if (arduino.isConnected()) {
		cout << "Connection established" << endl << endl;
	}
	else {
		cout << "Error in port name" << endl << endl;
	}
	//while (arduino.isConnected()) {
		//for (int i = 0; i < 180; i += 10) {
		//}

	//variable declarations
	float scalar = 1;
	float axisTest_timer = 70;
	int axisTest_loopIncrement = 2;

	float runOffsetTest_timer = 40;
	float l;
	float a, b, Y;
	a = toRadians(45)/*yaw*/, b = toRadians(90)/*pitch*/, Y = toRadians(0)/*roll*/;

	float a1, b1;
	Vector3f PP, P2; //PP is coords entered; P2 is coords to cutoff
	float xDefault = 0, yDefault = 0, zDefault = 0;
	float c;

	int servoSpeed;

	//servo default declaration and definition
	int q1_default = 90, //in degrees
		q2_default = 0, //in degrees
		q3_default = 135, //in degrees
		q4_default = 90, //in degrees
		q5_default = 90, //in degrees
		q6_default = 1; //in degrees

	float q1 = q1_default,
		q2 = toRadians(q2_default),
		q3 = toRadians(q3_default),
		q4 = toRadians(q4_default),
		q5 = toRadians(q5_default),
		q6 = toRadians(q6_default); //in radians
	float q1_holder, q2_holder, q3_holder, q4_holder, q5_holder, q6_holder;


	//custom options:
	bool runSquareBox = false;
	bool runAxisTest1 = false;
	bool runAxisTest2 = false;
	bool runOffsetTest1 = false;
	bool runOffsetTest2 = false;

	Vector3f customBox1_coords[16];
	customBox1_coords[0] = Vector3f(-100, 100, 100);
	customBox1_coords[1] = Vector3f(-100, 200, 100);
	customBox1_coords[2] = Vector3f(100, 200, 100);
	customBox1_coords[3] = Vector3f(100, 100, 100);
	customBox1_coords[4] = Vector3f(-100, 100, 200);
	customBox1_coords[5] = Vector3f(-100, 200, 200);
	customBox1_coords[6] = Vector3f(100, 200, 200);
	customBox1_coords[7] = Vector3f(100, 100, 200);

	customBox1_coords[8] = Vector3f(0, 100, 200);
	customBox1_coords[9] = Vector3f(50, 100, 200);
	customBox1_coords[10] = Vector3f(50, 65, 200);
	customBox1_coords[11] = Vector3f(0, 65, 200);
	customBox1_coords[12] = Vector3f(0, 65, 100);
	customBox1_coords[13] = Vector3f(0, 100, 100);
	customBox1_coords[14] = Vector3f(50, 100, 100);
	customBox1_coords[15] = Vector3f(50, 65, 100);

	Vector3f customBox2_coords[9];
	customBox2_coords[0] = Vector3f(-50, 50, 150);
	customBox2_coords[1] = Vector3f(-50, 150, 150);
	customBox2_coords[2] = Vector3f(50, 150, 150);
	customBox2_coords[3] = Vector3f(50, 50, 150);
	customBox2_coords[4] = Vector3f(50, 50, 50);
	customBox2_coords[5] = Vector3f(-50, 50, 50);
	customBox2_coords[6] = Vector3f(-50, 150, 50);
	customBox2_coords[7] = Vector3f(50, 150, 50);
	customBox2_coords[8] = Vector3f(50, 50, 50);



	char posOption; //whether it be '+' or '-' for the type of angle q2 and q3 is. + is lore robot, - is typical gripper
	while (true) {
		if (
			!runSquareBox &&
			!runAxisTest1 &&
			!runAxisTest2 &&
			!runOffsetTest1 &&
			!runOffsetTest2
			) {
			float a_temp, b_temp, Y_temp;

			while (true) {
				string strX, strY, strZ;
				while (true) {
					cout << "\nEnter coords (x(left,right), y(forward, backward) and z(up and down)) separeted with a space symbol inbetween:" << endl;
					cin >> strX >> strY >> strZ;
					if (strX == "run" && strY == "square" && strZ == "box") {
						runSquareBox = true;
						cout << "Running box test\n";
						break;
					}
					else if (strX == "run" && strY == "axis" && strZ == "test1") {
						runAxisTest1 = true;
						break;
					}
					else if (strX == "run" && strY == "axis" && strZ == "test2") {
						runAxisTest2 = true;
						break;
					}
					else if (strX == "run" && strY == "offset" && strZ == "test1") {
						runOffsetTest1 = true;
						break;
					}
					else if (strX == "run" && strY == "offset" && strZ == "test2") {
						runOffsetTest2 = true;
						break;
					}

					cout << "\nEnter angles a, b and Y (in degrees):";
					cin >> a_temp >> b_temp >> Y_temp;


					cout << "\n\nuse '+' or '-' cosine?\ninput: ";
					cin >> posOption;
					if (posOption != '+' && posOption != '-')
						cout << "incorrect sign input. Try again";
					else if (posOption == '+' || posOption == '-')
						break;

				}
				if (runSquareBox || runAxisTest1 || runAxisTest2 || runOffsetTest1 || runOffsetTest2)
					break;

				a = toRadians(a_temp);
				b = toRadians(b_temp);
				Y = toRadians(Y_temp);

				PP.x = stof(strX);
				PP.y = stof(strY);
				PP.z = stof(strZ);

				float coord_scalar = 1;

				PP.x = coord_scalar * (PP.x + xDefault);
				PP.y = coord_scalar * (PP.y + yDefault);
				PP.z = coord_scalar * (PP.z + zDefault);

				{//cout << "Coords: x:" << PP.x << " y:" << PP.y << " z:" << PP.z << endl;

				//float length_scalar = 1;

				////lengths of the arms (in px but it's arbitrary here) [mm]

				//d1 = d1 * length_scalar;
				//d2 = d2 * length_scalar;
				//d3 = d3 * length_scalar;
				//d4 = d4 * length_scalar;
				//d5 = d5 * length_scalar;
				//d6 = d6 * length_scalar;


				//l = d4 * cos(b);
				//P2.x = PP.x - l * cos(a);
				//P2.y = PP.y - l * sin(a);
				//P2.z = PP.z - d4 * sin(b);

				//if (P2.x == 0)
				//	P2.x = 0.00000001;
				//if (P2.y == 0)
				//	P2.y = 0.00000001;
				//if (P2.z == 0)
				//	P2.z = 0.00000001;

				//q1_holder = atan(P2.x / P2.y);

				//if (!isnan(q1_holder))
				//	q1 = q1_holder;


				//if (posOption == '-') {
				//	q3_holder = q3_negative(P2, d1, d2, d3);
				//	if (!isnan(q3_holder))
				//		q3 = q3_holder;
				//	q2_holder = q2_negative(P2, d1, d2, d3, q3, &q2);
				//	if (!isnan(q2_holder))
				//		q2 = q2_holder;
				//}
				//else if (posOption == '+') {
				//	q3_holder = q3_positive(P2, d1, d2, d3);
				//	if (!isnan(q3_holder))
				//		q3 = q3_holder;
				//	q2_holder = q2_positive(P2, d1, d2, d3, q3, &q2);
				//	if (!isnan(q2_holder))
				//		q2 = q2_holder;
				//}
				//cout << "used: " << posOption << " for q3 and q2" << endl;
				//a1 = get_modified_a1(a, q1);
				//b1 = get_b1(b, q2, q3);

				//q4_holder = q4_func(get_d4ry(d4, b1, a1), get_d4rz(d4, b1));
				//if (!isnan(q4_holder))
				//	q4 = q4_holder;
				//q5_holder = q5_func(get_d4ry(d4, b1, a1), get_d4rz(d4, b1), b1, a1);
				//if (!isnan(q5_holder))
				//	q5 = q5_holder;
				//q6_holder = q6_func(Y, q4, b);
				//if (!isnan(q6_holder))
				//	q6 = q6_holder;


				//cout << "\n---calculated rotation: q1:" << toDegrees(q1) << " q2:" << toDegrees(q2) << " q3:" <<
				//	toDegrees(q3) << " q4:" << toDegrees(q4) << " q5:" << toDegrees(q5) << " q6:" << toDegrees(q6) << "---" << endl;

				//if (isnan(q1_holder)) { cout << "q1 is NaN" << endl; }
				//if (isnan(q2_holder)) { cout << "q2 is NaN" << endl; }
				//if (isnan(q3_holder)) { cout << "q3 is NaN" << endl; }
				//if (isnan(q4_holder)) { cout << "q4 is NaN" << endl; }
				//if (isnan(q5_holder)) { cout << "q5 is NaN" << endl; }
				//if (isnan(q6_holder)) { cout << "q6 is NaN" << endl; }


				//string data;

				//data = (
				//	to_string(q1_default - int(round(toDegrees(q1)))) + ":" +
				//	to_string(q2_default + int(round(toDegrees(q2)))) + ":" +
				//	to_string(q3_default + int(round(toDegrees(q3)))) + ":" +
				//	to_string(q4_default + int(round(toDegrees(q4)))) + ":" +
				//	to_string(q5_default + int(round(toDegrees(q5)))) + ":" +
				//	to_string(q6_default + int(round(toDegrees(q6))))
				//	);

				////copy(data.begin(), data.end(), charArray);
				////charArray[data.size()] = '\n';
				////charArray[data.size() + 1] = '\0';
				////cout << "char array: " << charArray << endl;

				//cout << "---- sent:" << data << endl;

				//char* charArray = new char[data.size() + 1];
				//addNULL_char(data, charArray);

				//arduino.writeSerialPort(charArray, data.size() + 1);
				//delete[] charArray;
				}

				sendCoords_7DOF_sendCoords(PP, posOption, &arduino, a, b, Y, true, 5, "true");

				Sleep(10);
				arduino.readSerialPort(output, MAX_DATA_LENGTH);
				cout << endl;
				cout << ">> " << output << endl;
				cout << "----------\n" << endl;
			}
		}

		if (runSquareBox) {
			posOption = '-';
			for (int i = 0; i < 8; i++) {
				cout << "point: " << i << endl;
				sendCoords_7DOF_sendCoords(customBox1_coords[i], '-', &arduino, toRadians(float(-45) * ifPositiveOrNegative(customBox1_coords[i].x)),
					toRadians(-45), 0, true, 4, "true");
				Sleep(2000);
				if (Keyboard::isKeyPressed(Keyboard::Space)) { while (!Keyboard::isKeyPressed(Keyboard::Escape)); }
				if (Keyboard::isKeyPressed(Keyboard::LShift) && Keyboard::isKeyPressed(Keyboard::Space)) { break; }
			}
			Sleep(2000);
			string data = to_string(90) + ":"
				+ to_string(45) + ":"
				+ to_string(180) + ":"
				+ to_string(90) + ":"
				+ to_string(45) + ":"
				+ to_string(90);
			char* charArray = new char[data.size() + 1];
			addNULL_char(data, charArray);
			arduino.writeSerialPort(charArray, data.size() + 1);
			delete[] charArray;
			runSquareBox = false;
		}
		if (runAxisTest1) {
			cout << "Running axis test no.1\n";
			servoSpeed = 1000;
			string toUseTimer = "false";
			posOption = '-';
			b = toRadians(90);
			sendCoords_7DOF_sendCoords(Vector3f(-150, 100, 200), posOption, &arduino, 0, toRadians(-45), Y, true, 2, "true");
			Sleep(5000);
			for (int i = -150; i <= 150; i += axisTest_loopIncrement) {
				cout << "x: " << i << endl;
				sendCoords_7DOF_sendCoords(Vector3f(i, 100, 200), posOption, &arduino, toRadians(0 - float(i) * 0.3)/*from 45 to -45 degrees*/, toRadians(-45), Y, true, servoSpeed, toUseTimer);
				Sleep(axisTest_timer);
			}
			for (int i = 150; i > -150; i -= axisTest_loopIncrement) {
				cout << "x: " << i << endl;
				sendCoords_7DOF_sendCoords(Vector3f(i, 100, 200), posOption, &arduino, toRadians(0 - float(i) * 0.3)/*from -45 to 45 degrees*/, toRadians(-45), Y, true, servoSpeed, toUseTimer);
				Sleep(axisTest_timer);
			}
			Sleep(2000);
			for (int i = 100; i < 250; i += axisTest_loopIncrement) {
				cout << "y: " << i << endl;
				sendCoords_7DOF_sendCoords(Vector3f(-150, i, 200), posOption, &arduino, toRadians(float(0 - (i - 250)) * 0.3)/*from 45 to 0*/, toRadians(0 - float(100 - (i - 150)) * 0.45) /*from -45 to 1 degrees*/, Y, true, servoSpeed, toUseTimer);
				Sleep(axisTest_timer);
				//while(Keyboard::isKeyPressed(Keyboard::Space)) {}
			}
			for (int i = 250; i > 100; i -= axisTest_loopIncrement) {
				cout << "y: " << i << endl;
				sendCoords_7DOF_sendCoords(Vector3f(-150, i, 200), posOption, &arduino, toRadians(float(0 - (i - 250)) * 0.3)/*from 0 to 45*/, toRadians(0 - float(100 - (i - 150)) * 0.45) /*from 1 to -45 degrees*/, Y, true, servoSpeed, toUseTimer);
				Sleep(axisTest_timer);
			}
			Sleep(2000);
			for (int i = 200; i < 350; i += axisTest_loopIncrement) {
				cout << "z: " << i << endl;
				sendCoords_7DOF_sendCoords(Vector3f(-150, 100, i), posOption, &arduino, toRadians(45), toRadians(float(i - 250) * 0.9) /*from -45 to 45*/, Y, true, servoSpeed, toUseTimer);
				Sleep(axisTest_timer);
			}
			for (int i = 350; i >= 200; i -= axisTest_loopIncrement) {
				cout << "z: " << i << endl;
				sendCoords_7DOF_sendCoords(Vector3f(-150, 100, i), posOption, &arduino, toRadians(45), toRadians(float(i - 250) * 0.9), Y, true, servoSpeed, toUseTimer);
				Sleep(axisTest_timer);
			}
			Sleep(2000);
			sendCoords_7DOF_sendCoords(Vector3f(-75, 150, 300), '-', &arduino, 0, toRadians(-45), Y, true, 1, "true");
			cout << "test finished\n";
			runAxisTest1 = false;
		}
		if (runAxisTest2) {
			cout << "Running axis test no.2\n";
			posOption = '-';
			b = toRadians(90);
			sendCoords_7DOF_sendCoords(Vector3f(0, 10, 160), posOption, &arduino, a, b, Y);
			posOption = '+';
			b = toRadians(90);
			Sleep(5000);
			for (int i = 50; i >= -50; i -= 2) {
				cout << "x: " << i << endl;
				sendCoords_7DOF_sendCoords(Vector3f(i, 50, 350), posOption, &arduino, a, b, Y);
				Sleep(axisTest_timer);
			}
			for (int i = -50; i <= 50; i += 2) {
				cout << "x: " << i << endl;
				sendCoords_7DOF_sendCoords(Vector3f(i, 50, 350), posOption, &arduino, a, b, Y);
				Sleep(axisTest_timer);
			}

			for (int i = 50; i <= 150; i += 2) {
				cout << "y: " << i << endl;
				sendCoords_7DOF_sendCoords(Vector3f(50, i, 350), posOption, &arduino, a, b, Y);
				Sleep(axisTest_timer);
			}
			for (int i = 150; i >= 50; i -= 2) {
				cout << "y: " << i << endl;
				sendCoords_7DOF_sendCoords(Vector3f(50, i, 350), posOption, &arduino, a, b, Y);
				Sleep(axisTest_timer);
			}

			for (int i = 350; i >= 150; i -= 2) {
				cout << "z: " << i << endl;
				sendCoords_7DOF_sendCoords(Vector3f(50, 50, i), posOption, &arduino, a, b, Y);
				Sleep(axisTest_timer);
			}
			for (int i = 150; i <= 350; i += 2) {
				cout << "z: " << i << endl;
				sendCoords_7DOF_sendCoords(Vector3f(50, 50, i), posOption, &arduino, a, b, Y);
				Sleep(axisTest_timer);
			}
			Sleep(2000);
			sendCoords_7DOF_sendCoords(Vector3f(-50, 50, 150), '-', &arduino, a, PI / 2, Y);
			cout << "test finished\n";
			runAxisTest2= false;
		}
		if (runOffsetTest1) {
			PP = Vector3f(0, 150, 325);
			for (int i = 0; i <= 360; i++) {
				printf("i: %i\n", i);
				sendCoords_7DOF_sendCoords(PP, '-', &arduino, toRadians((0 - sin(toRadians(i))) * 45), toRadians((0 - cos(toRadians(i))) * 45), 0, false, 100, "false");
				Sleep(runOffsetTest_timer);
				while(Keyboard::isKeyPressed(Keyboard::Space)) {}
				if (Keyboard::isKeyPressed(Keyboard::LShift) && Keyboard::isKeyPressed(Keyboard::Space)) { break; }
				Sleep(50);
			}
			runOffsetTest1 = false;
		}
		if (runOffsetTest2) {
			PP = Vector3f(0, 110, 300);
			for (int i = 0; i <= 360; i++) {
				printf("i: %i\n", i);
				sendCoords_7DOF_sendCoords(PP, '-', &arduino, toRadians((0 - sin(toRadians(i))) * 45), toRadians((0 - cos(toRadians(i))) * 45), 0, false, 90, "false");
				Sleep(runOffsetTest_timer);
				while (Keyboard::isKeyPressed(Keyboard::Space)) {}
				if (Keyboard::isKeyPressed(Keyboard::LShift) && Keyboard::isKeyPressed(Keyboard::Space)) { break; }
				Sleep(50);
			}
			runOffsetTest2 = false;
		}
	}
	//}
}

void testOffsetAngles_7DOF() {
	SerialPort arduino(port);
	system("cls");

	if (arduino.isConnected()) {
		cout << "Connection established" << endl << endl;
	}
	else {
		cout << "Error in port name" << endl << endl;
	}

	string input;

	Vector3f PP;
	char posOption = '-';
	float a, b, Y; //in degrees

	int scaleVal = 2;

	cout << "Enter coords (with spaces): ";
	cin >> PP.x >> PP.y >> PP.z;

	cin.clear();
	cin.ignore();

	cout << "---start coords: x:" << PP.x << " y:" << PP.y << " z:" << PP.z << endl;

	cout << "Enter start a, b and Y values (with spaces, in degrees): ";
	cin >> a >> b >> Y;


	while (true) {

		printf("a:%d b:%d Y:%d", int(a), int(b), int(Y));
		if (Keyboard::isKeyPressed(Keyboard::Escape)) {
			if (Keyboard::isKeyPressed(Keyboard::LShift))
				return;
			cout << "Enter coords (with spaces between xyz): ";
			cin.ignore();
			getline(cin, input);
			if (input == "exit") {
				break;
			}
			PP.x = stof(input.substr(0, input.find(" ")));
			input.erase(0, input.find(" ") + 1);
			PP.y = stof(input.substr(0, input.find(" ")));
			input.erase(0, input.find(" ") + 1);
			PP.z = stof(input.substr(0, input.find(" ")));
		}
		if (Keyboard::isKeyPressed(Keyboard::Q))
			posOption = '-';
		else if (Keyboard::isKeyPressed(Keyboard::W))
			posOption = '+';

		if (Keyboard::isKeyPressed(Keyboard::A)) {
			if (Keyboard::isKeyPressed(Keyboard::LShift))
				a -= scaleVal;
			else {
				a += scaleVal;
			}
		}
		if (Keyboard::isKeyPressed(Keyboard::B)) {
			if (Keyboard::isKeyPressed(Keyboard::LShift))
				b -= scaleVal;
			else {
				b += scaleVal;
			}
		}
		if (Keyboard::isKeyPressed(Keyboard::Y)) {
			if (Keyboard::isKeyPressed(Keyboard::LShift))
				Y -= scaleVal;
			else {
				Y += scaleVal;
			}
		}
			

		sendCoords_7DOF_sendCoords(PP, posOption, &arduino, toRadians(a), toRadians(b), toRadians(Y), true, 100);

		printf("received:%s\n", output);

		cout << endl;
		Sleep(100);
	}
}

void useAccCommmands_7DOF() {
	system("cls");

	SerialPort arduino(port);

	if (arduino.isConnected()) {
		cout << "Connection established" << endl << endl;
	}
	else {
		cout << "Error in port name and/or unable to connect to port" << endl << endl;
		return;
	}

	//variable declarations
	float scalar = 1;
	float axisTest_timer = 60;
	float l;
	float a, b, Y;
	a = toRadians(0)/*yaw*/, b = toRadians(-45)/*pitch*/, Y = toRadians(0)/*roll*/; //in radians

	float a1, b1;
	Vector3f PP, P2; //PP is coords entered; P2 is coords to cutoff
	float xDefault = 0, yDefault = 0, zDefault = 0;
	float c;

	//float pitch, yaw, 
	Vector3f coordChange; //change in coords
	Vector2f tiltChange; //x - pitch, y - roll //in radians

	Vector3f coordChange_factor = Vector3f(1, 1, 1);

	char posOption = '-';
	
	PP = Vector3f(0, d5 + d6 + 127, 200);

	bool startup = true;

	int sleepTimer = 80;

	while (true) {
		if (startup) {
			sendCoords_7DOF_sendCoords(Vector3f(0, 150, 150) , posOption, &arduino, toRadians(0), toRadians(-45), Y);
			startup = false;
			Sleep(1000);
		}

		if (Keyboard::isKeyPressed(Keyboard::Escape)) {
			break;
		}
		if (Keyboard::isKeyPressed(Keyboard::W)) {
			posOption = '+';
		}
		if (Keyboard::isKeyPressed(Keyboard::S)) {
			posOption = '-';
		}

		float coord_scalar = 1;

		PP.x = coord_scalar * (PP.x + xDefault);
		PP.y = coord_scalar * (PP.y + yDefault);
		PP.z = coord_scalar * (PP.z + zDefault);

		bool bPos = false;
		if (tiltChange.x <= float(90) && tiltChange.x >= -90) {
			b = toRadians(tiltChange.x * 0.9 + toDegrees(b) * 0.1);
			if(b < 0) { bPos = false; }
			if (b > 0) { bPos = true; }
		}
		else {}
		if (tiltChange.y <= float(90) && tiltChange.y >= -90) { 
			if (!bPos) {
				a = toRadians(0 - (tiltChange.y * 0.9 + toDegrees(a) * 0.1));
			}
			else if (bPos) {
				a = toRadians((tiltChange.y * 0.9 + toDegrees(a) * 0.1));
			}
		}
		else {}

		/*if (toDegrees(tiltChange.y) <= 45 && toDegrees(tiltChange.y) >= -45) {
			a = toRadians(tiltChange.y * 0.1 + toDegrees(a) * 0.9);
		}
		else {}*/
		printf("\na:%f b:%f\n", toDegrees(a), toDegrees(b));
		if (areAngles_NaN(PP, a, b, posOption)) {}
		else {
			sendCoords_7DOF_sendCoords(PP, posOption, &arduino, a, b, Y, true, 1, "false");
			//Sleep(10);
			arduino.readSerialPort(output, MAX_DATA_LENGTH);
			updateArduino_output(&coordChange, &tiltChange);
		}

		/*PP.x += coordChange.x * coordChange_factor.x + sin(toRadians(tiltChange.y)) * coordChange.z * coordChange_factor.z;
		PP.y += coordChange.y * coordChange_factor.y + sin(toRadians(tiltChange.x)) * coordChange.z * coordChange_factor.z;
		PP.z += coordChange.z * coordChange_factor.z + sin(toRadians(tiltChange.x)) * coordChange.y * coordChange_factor.y +
			sin(toRadians(tiltChange.y)) * coordChange.x * coordChange_factor.x;*/

		printf("\nx:%f y:%f z:%f\n", PP.x, PP.y, PP.z);

		Sleep(1);
		while (Keyboard::isKeyPressed(Keyboard::Space)) {}
		if (Keyboard::isKeyPressed(Keyboard::LShift) && Keyboard::isKeyPressed(Keyboard::Space)) {
			Sleep(100);
			while(!Keyboard::isKeyPressed(Keyboard::O)) {}
		}
		//}
		Sleep(sleepTimer);
	}
}

bool tempArray[100][100 / 2][100]; //variable to hold possible positions (i.e workspace), true if it's not NaN [hols a boolean value: true if can reach, false if unreachable]
Vector2f possibleAngle[100][100 / 2][100]; //variable to hold said possible hangles for each ws-position (so it doesn't have to be re-calculated later) [holds both .x for alpha and .y beta variables in degrees]
const int res = 100; //note: the resolution for the y axis is half because the circle is half.

void createListof_ws(Vector2i xRange, bool specify_ab = false) {
	system("cls");
	Vector3f XYZ_max = Vector3f(
		d2 + d3 + d4 + d5 + d6,
		d2 + d3 + d4 + d5 + d6,
		d1 + d2 + d3 + d4 + d5 + d6
	); //the x and z variables hold only one side of the workspace
	Vector3f PP;


	//bool tempArray[res][res / 2][res]; //true if it's not NaN

	float angle_max = 180; //in degrees of the total angle

	float a, b, Y = 0; //in radians
	float a_input = 0, b_input = 0;
	
	if (specify_ab) {
		cout << "\nenter alpha and beta value [in degrees, with spaces]: ";
		cin >> a_input >> b_input;
	}

	float x_gap = 2 * XYZ_max.x / res, y_gap = XYZ_max.y / (res / 2), z_gap = 2 * XYZ_max.z / res;

	//float spacing;

	char posOption = '-';
	bool breakVal = false;

	Vector3f minVal = Vector3f(-XYZ_max.x, 0, XYZ_max.z);  //holds the minimum value to start going "up" from with the for() loop.
	
	//int xMin = 0; //this will be the different value for each thread




	Sleep(1000);
	printf(" | X:%f\n", minVal.x);
	printf(" | Y:%f\n", minVal.y);
	printf(" | Z:%f\n", minVal.z);


	for (int z = 0; z < res; z++) { //this goes from up to down
		printf("--z val:%d\n", z);
		for (int y = 0; y < float(res)/2; y++) { //this goes from back to forward
			printf("y val:%d\n", y);
			for (int x = xRange.x; x < xRange.y; x++) { //this goes from left to right; it'll always iterate 25 times so no need for special variable
				PP = Vector3f(minVal.x + x * x_gap, minVal.y + y * y_gap, minVal.z - z * z_gap); //the z-range is special
				if (!specify_ab) {
					a = atan(PP.x / PP.y);
					b = atan(PP.z / PP.y);
				}
				else {
					a = toRadians(a_input);
					b = toRadians(b_input);
				}
				if(areAngles_NaN(
					PP,
					a,
					b,
					posOption
				)) {
					//printf("x:%d y:%d z:%d\n", x, y, z);

					//a second check where all possible a, b values are tested (this part will drastically slow it all down)
					breakVal = false;
					if (!specify_ab) {
						for (int alpha = 0 - 90; alpha <= 90; alpha++) {
							for (int beta = 0 - 90; beta <= 90; beta++) {
								//printf("alpha:%d beta:%d\n", alpha, beta);
								if (!areAngles_NaN(PP, toRadians(float(alpha)), toRadians(float(beta)), posOption)) {
									possibleAngle[x][y][z].x = alpha;
									possibleAngle[x][y][z].y = beta;

									tempArray[x][y][z] = true;
									breakVal = true;
									beta = 91;
									alpha = 91;
									printf("---alternative found\n");
								}
							}
						}
					}
					if (!breakVal) { tempArray[x][y][z] = false; }
				}
				else {
					tempArray[x][y][z] = true;
					possibleAngle[x][y][z].x = toDegrees(a);
					possibleAngle[x][y][z].y = toDegrees(b);
				}
			}
		}
	}

	cout << "\n\n-----finished calculating-----\n";


} //.x is min, .y is max

string endlist[10000]; //the finished list of data points that will be used to compare new var's for. 10k because there's maximum 
int rowsOfData;

void filterForHighestZVal() {
	//load the ws-list text into 
	system("cls");

	static string var[101122]; //holds all the old data points
	string toCompare; //the biggest value
	string currentXY; //the xy value of the current one being compared
	
	size_t found;

	string nameof_fileToFilter;
	string nameof_fileToFilter_to;

	/*cout << "\nEnter file name of the .dat file to search for [note: include .dat and the relative address (from working directory)]:\n- ";
	cin >> nameof_fileToFilter;
	cout << "\nEnter name of the new file with the filtered data [name and address]:\n- ";
	cin >> nameof_fileToFilter_to;*/

	//"ws-calc/ws-list.dat"
	ifstream myfile("ws-calc/ws - full list - alphaBeta/ws-list_res10.dat");

	getline(myfile, var[0]);

	for (int i = 0; i < 101122; i++) { getline(myfile, var[i]); }
	myfile.close();

	toCompare = var[0];
	var[0] = "-1";
	found = toCompare.find(" ");



	int list_rowCount = 213; //the rows of coordinates (remember to subtract for the initial comment)
	int maxPossibleAnswers = pow(10, 2);
	//this is an example of how i imagine the code to look
	//unless there're any major flaws then no need to look through it.

	int k_count = 0; //keeps track so that the loops end the moment all elements of the old function var[] is =-1
	rowsOfData = 0; //the complete number of elements in endlist[] that actually holds data and isn't empty.

	string temp[2]; /*temporary string holders because of the erase() function*/
	for (int k = 0; k < maxPossibleAnswers; k++) {
		printf("k: %d\n", k);
		k_count = 0;
		for (int i = 1; i < list_rowCount; i++) {
			if (stoi(var[i].substr(0, var[0].find(" ", 0))) != -1) {
				size_t found = var[i].find(" ", 0); /*//this is just put here to showcase and not give a seizure to whoever tries to look at the next if statement*/
				if (
					toCompare.substr(0, toCompare.find(" ", toCompare.find(" ", 0) + 1)) == 
					var[i].substr(0, var[i].find(" ", var[i].find(" ", 0) + 1))
					) {
					temp[0] = toCompare;
					temp[1] = var[i];
					if (aIsBigger(
						stof(var[i].erase(0, (var[i].erase(0, found + 1)).find(" ") + 1)),
						stof(toCompare.erase(0, (toCompare.erase(0, found + 1)).find(" ") + 1))
					))
					{
						toCompare = temp[1];
						printf("it's higher\n");
						system("pause");
					} //because the data is sorted from the highest z-value to the lowest this part never is active.
					else { toCompare = temp[0]; }
					var[i] = "-1";
				}
				/*cout << toCompare.substr(0, toCompare.find(" ", toCompare.find(" ", 0) + 1)) << "------" <<
					var[i].substr(0, var[i].find(" ", var[i].find(" ", 0) + 1)) << endl;*/
			}
			else { k_count++; /*printf("+1 added to k_count\n");*/ }
			/*inside i loop*/
		}
		if (k_count >= list_rowCount-1) {
			cout << "break initialised\n";
			for (int n = 0; n < list_rowCount; n++) {
				if (stoi(var[n].substr(0, var[n].find(" ", 0))) != -1) {
					toCompare = var[n];
					var[n] = "-1";
					break;
				}
			}
			//rowsOfData+=2;
			break;
		}
		printf("added to endlist[%d]\n", k);
		endlist[k] = toCompare;
		rowsOfData++;

		for (int n = 0; n < list_rowCount; n++) { //to pick a new XY value to check the z-value for
			if (stoi(var[n].substr(0, var[n].find(" ", 0))) != -1) {
				toCompare = var[n];
				var[n] = "-1";
				break;
			}
		}
		/*inside k loop*/
	}
	//"ws-calc/ws-list_Zvals.dat"
	ofstream output("ws-calc/ws - top z-value - alphaBeta/ws-list_res10-z_val.dat");
	for (int i = 0; i < rowsOfData; i++) {
		output << (endlist[i] + "\n");
		printf("point: %d\n", i);
	}
	output.close();
	printf("---FINISHED:---\nThe total number of elements in the end: %d\n", rowsOfData);
	system("pause");

}

void hexclaw_ws_stuff() {
	system("cls");

	//const int res = 10; //note: the resolution for the y axis is half because the circle is half.

	Vector3f XYZ_max = Vector3f(
		d2 + d3 + d4 + d5 + d6,
		d2 + d3 + d4 + d5 + d6,
		d1 + d2 + d3 + d4 + d5 + d6
	); //the x and z variables hold only one side of the workspace
	float x_gap = 2 * XYZ_max.x / res, y_gap = XYZ_max.y / (res / 2), z_gap = 2 * XYZ_max.z / res;


	string input;
	cout << "1. generate list of workspace limits with free alpa/beta choice (it'll create a dot wherever any of 180^2 alpha/beta combinations will not return NaN) [uses 4 threads]\n";
	cout << "2. load and filter the list of ws points from 1. for the ones with the highest z-val [uses 1 thread; slow]\n";
	cout << "3. generate list of workspace limits with a given alpha/beta input\n";
	cout << "4. generate list of workspace limits with a range of alpha/beta input\n";

	cin >> input;
	if (input == "exit") { return; }
	if (stoi(input) == 1) {
		
		createListof_ws(Vector2i(0, 10));
		//thread t1(createListof_ws, Vector2i(0, round(float(res)/4))); //the .y variable is never reached, it just counts up to that point
		//thread t2(createListof_ws, Vector2i(round(float(res) * 0.25), round(float(res) * 0.5)));
		//thread t3(createListof_ws, Vector2i(round(float(res) * 0.5), round(float(res) * 0.75)));
		//thread t4(createListof_ws, Vector2i(round(float(res) * 0.75), res));
		//t1.join();
		//t2.join();
		//t3.join();
		//t4.join();

		string coord;

		string fileName = "ws-list_res" + to_string(res) + ".dat";
		ofstream myfile("ws-calc/ws - full list - alphaBeta/" + fileName);
		myfile << "#x y z\n";

		for (int z = 0; z < float(res); z++) {
			for (int y = 0; y < float(res)*0.5; y++) {
				for (int x = 0; x < float(res); x++) {
					printf("x:%d y:%d z:%d\n", x, y, z);
					if (tempArray[x][y][z]) {
						coord = to_string((0 - XYZ_max.x) + x * x_gap) + " " + to_string(y * y_gap) + " " + to_string(XYZ_max.z - z * z_gap) + " " + to_string(possibleAngle[x][y][z].x) + " " + to_string(possibleAngle[x][y][z].y) + "\n";
						myfile << coord;
					}
					else {
						//coord = to_string((0 - XYZ_max.x) + x * x_gap) + " " + to_string(y * y_gap) + " " + "0" + " 0" + "\n";
					}
				}
			}
		}
		myfile.close();
		printf("FINISHED\n");
	}
	if (stoi(input) == 2) {
		filterForHighestZVal();

	}
	if (stoi(input) == 3) {
		
		createListof_ws(Vector2i(0, 100), true);

		string coord;


		ofstream myfile("ws-calc/ws - full list - alphaBeta/0.-90.dat");
		myfile << "#x y z\n";

		for (int z = 0; z < res; z++) {
			for (int y = 0; y < float(res)*0.5; y++) {
				for (int x = 0; x < res; x++) {
					printf("x:%d y:%d z:%d\n", x, y, z);
					if (tempArray[x][y][z]) {
						coord = to_string((0 - XYZ_max.x) + x * x_gap) + " " + to_string(y * y_gap) + " " + to_string(XYZ_max.z - z * z_gap) + " " + to_string(possibleAngle[x][y][z].x) + " " + to_string(possibleAngle[x][y][z].y) + "\n";
						myfile << coord;
					}
					else {
						//coord = to_string((0 - XYZ_max.x) + x * x_gap) + " " + to_string(y * y_gap) + " " + "0" + " 0" + "\n";
					}
				}
			}
		}
		myfile.close();
		printf("FINISHED\n");
	}
	if (stoi(input) == 4) {

	}


}

void coord_to_7DOF() {
	system("cls");
	SerialPort arduino(port);

	if (arduino.isConnected()) {
		cout << "Connection established" << endl << endl;
	}
	else {
		cout << "Error in port name" << endl << endl;
	}

	//variable declarations
	float scalar = 1;
	float axisTest_timer = 70;
	int axisTest_loopIncrement = 2;

	float runOffsetTest_timer = 40;
	float l;
	float a, b, Y;
	float a_origin = 1, b_origin = -90;
	a = toRadians(a_origin)/*yaw*/, b = toRadians(b_origin)/*pitch*/, Y = toRadians(0)/*roll*/;

	//float a1, b1;
	Vector3f PP, P2; //PP is coords entered; P2 is coords to cutoff
	float xDefault = 0, yDefault = 0, zDefault = 0;
	float c;

	int servoSpeed;


	float q1 = toRadians(90),
		q2 = toRadians(0),
		q3 = toRadians(135),
		q4 = toRadians(90),
		q5 = toRadians(90),
		q6 = toRadians(1); //in radians
	float q1_holder, q2_holder, q3_holder, q4_holder, q5_holder, q6_holder;


	char posOption = '-'; //whether it be '+' or '-' for the type of angle q2 and q3 is. + is lore robot, - is typical gripper
	bool breakVal = false;

	while (true) {
		while (true) {
			a = toRadians(a_origin)/*yaw*/, b = toRadians(b_origin)/*pitch*/, Y = toRadians(0)/*roll*/;
			string strX, strY, strZ;
			cout << "\nEnter coords (x(left,right), y(forward, backward) and z(up and down)) separeted with a space symbol inbetween:" << endl;
			cin >> strX;
			if (strX == "exit") { return; }
			cin >> strY >> strZ;

			PP.x = stof(strX);
			PP.y = stof(strY);
			PP.z = stof(strZ);

			if (areAngles_NaN(PP, a, b, posOption)) {
				//printf("orientationvariables are incorrect. brute forcing run\n");
				breakVal = false;
				for (int beta = b_origin; beta <= 90; beta++) {
					for (int alpha = 0 - 90; alpha <= 90; alpha++) {
						if (!areAngles_NaN(PP, toRadians(float(alpha)), toRadians(float(beta)), posOption)) {
							a = toRadians(alpha);
							b = toRadians(beta);
							beta = 91;
							alpha = 91;
							//printf("---alternative found\n");
							printf("alpha:%d beta:%d", alpha, beta);
							breakVal = true;
						}
						if (breakVal) { break; }
					}
					if (breakVal) { break; }
				}
				if (breakVal) { break; }

				for (int beta = b_origin; beta >= -90; beta--) {
					for (int alpha = 0 - 90; alpha <= 90; alpha++) {
						if (!areAngles_NaN(PP, toRadians(float(alpha)), toRadians(float(beta)), posOption)) {
							a = toRadians(alpha);
							b = toRadians(beta);
							beta = 0-91;
							alpha = 91;
							//printf("---alternative found\n");
							printf("alpha:%d beta:%d", alpha, beta);
							breakVal = true;
						}
						if (breakVal) { break; }
					}
					if (breakVal) { break; }
				}
				if (breakVal) { break; }
			}
			else {
				break;
			}
			cout << "that coord is unreachable\n";
		}



		float coord_scalar = 1;

		PP.x = coord_scalar * (PP.x + xDefault);
		PP.y = coord_scalar * (PP.y + yDefault);
		PP.z = coord_scalar * (PP.z + zDefault);


		sendCoords_7DOF_sendCoords(PP, posOption, &arduino, a, b, Y, true, 1, "true");

		Sleep(10);
		arduino.readSerialPort(output, MAX_DATA_LENGTH);
		cout << endl;
		cout << ">> " << output << endl;
		cout << "----------\n" << endl;
	}
}

void Robotarm_CV_control() {
	system("cls");
	SerialPort arduino(port);

	if (arduino.isConnected()) {
		cout << "Connection established" << endl << endl;
	}
	else {
		cout << "Error in port name" << endl << endl;
	}

	VideoCapture TempCap(1); //capture the video from web cam
	if (!TempCap.isOpened()) {// if not success, exit program
		cout << "Cannot open the web cam" << endl;
		return;
	}
	//

	namedWindow("Control", WINDOW_AUTOSIZE); //create a window called "Control"
	int iLowH = 0;
	int iHighH = 0;
	int iLowS = 0;
	int iHighS = 0;
	int iLowV = 255;
	int iHighV = 255;

	long int pauseVal = -1;
	bool normalPath = true;
	//Create trackbars in "Control" window
	createTrackbar("LowH", "Control", &iLowH, 179); //Hue (0 - 179)
	createTrackbar("HighH", "Control", &iHighH, 179);
	createTrackbar("LowS", "Control", &iLowS, 255); //Saturation (0 - 255)
	createTrackbar("HighS", "Control", &iHighS, 255);
	createTrackbar("LowV", "Control", &iLowV, 255); //Value (0 - 255)
	createTrackbar("HighV", "Control", &iHighV, 255);
	Mat imgTemp;

	while (true) {
		pauseVal = waitKey(5);
		if (pauseVal == 32) { normalPath = false; }
		else if (pauseVal == 8) { normalPath = true; }
		if (normalPath) {
			bool bSuccess = TempCap.read(imgTemp); // read a new frame from video
			if (!bSuccess) //if not success, break loop
			{
				cout << "Cannot read a frame from video stream" << endl;
				break;
			}
		}
		Mat imgTempHSV;
		cvtColor(imgTemp, imgTempHSV, COLOR_BGR2HSV); //Convert the captured frame from BGR to HSV
		Mat imgTempThresholded;
		inRange(imgTempHSV, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), imgTempThresholded); //Threshold the image
		//morphological opening (remove small objects from the foreground)
		erode(imgTempThresholded, imgTempThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
		dilate(imgTempThresholded, imgTempThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
		//morphological closing (fill small holes in the foreground)
		dilate(imgTempThresholded, imgTempThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
		erode(imgTempThresholded, imgTempThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));

		imshow("Thresholded Image", imgTempThresholded); //show the thresholded image
		imshow("Original", imgTemp); //show the temporary image

		if (waitKey(30) == 27) {//wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
			cout << "esc key is pressed by user" << endl;
			destroyAllWindows();
			return;
		}
		if (waitKey(30) == 18) { //wait for 'ctrl + r'
			cout << "Starting Tracking..." << endl;
			destroyAllWindows();
			break;
		}
	}


	int loopDelayTimer = 0;


	//video tracking
	int trackLimit = 20000;//min area to start tracking in
	VideoCapture cap(1); //capture the video from webcam
	if (!cap.isOpened()) { cout << "Cannot open the web cam" << endl; return; }
	namedWindow("Control"); //create a window called "Control"
	int TrackBarToggle = 0;
	int TrackingSquare = 100;
	int TrackerScaling = 5;
	int Y_offVal = 50;
	int X_offVal = 50;
	double trackerScaledouble = 0.0001;
	//Create trackbars in "Control" window
	createTrackbar("LowH", "Control", &iLowH, 179); //Hue (0 - 179)
	createTrackbar("HighH", "Control", &iHighH, 179);
	createTrackbar("LowS", "Control", &iLowS, 255); //Saturation (0 - 255)
	createTrackbar("HighS", "Control", &iHighS, 255);
	createTrackbar("LowV", "Control", &iLowV, 255);//Value (0 - 255)
	createTrackbar("HighV", "Control", &iHighV, 255);
	createTrackbar("Toggle", "Control", &TrackBarToggle, 2);
	createTrackbar("Track size", "Control", &TrackerScaling, 10);
	createTrackbar("X-off val", "Control", &X_offVal, 100);
	createTrackbar("Y-off val", "Control", &Y_offVal, 100);
	int iLastX = -1;
	int iLastY = -1;
	int fakePosX, fakePosY;
	//Capture a temporary image from the camera
	Mat imgTmp;
	cap.read(imgTmp);
	//Create a black image with the size as the camera output
	Mat imgLines = Mat::zeros(imgTmp.size(), CV_8UC3);
	Mat imgTrackSquare = Mat::zeros(imgTmp.size(), CV_8UC3);
	Mat imgGrid = Mat::zeros(imgTmp.size(), CV_8UC3); //the grid on the background of the displayed image
	Vector2f camRes = Vector2f(640, 480); //the resolution of the camera output in xy
	Vector3f posFromImage = Vector3f(0, 0, 140); //the positions from the camera needs to reverted
	float zVal_default = 50;
	Vector3f coordScalar = Vector3f(10, 2, 1);
	
	//thread toArduino(robotArm_coordsToArduino, &arduino, Vector3f(0, 100, 150));

	while (true) { //camera tracking loop
		Mat imgOriginal;
		trackerScaledouble = 0.00001;
		if (TrackerScaling > 0) { trackerScaledouble = (TrackerScaling)*trackerScaledouble; }
		else { trackerScaledouble = 0.1 * trackerScaledouble; }
		bool bSuccess = cap.read(imgOriginal); // read a new frame from video
		if (!bSuccess) //if not success, break loop
		{
			cout << "Cannot read a frame from video stream" << endl;
			return;
		}
		Mat imgHSV;
		cvtColor(imgOriginal, imgHSV, COLOR_BGR2HSV); //Convert the captured frame from BGR to HSV
		Mat imgThresholded;
		inRange(imgHSV, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), imgThresholded); //Threshold the image
		//morphological opening (removes small objects from the foreground)
		erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
		dilate(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
		//morphological closing (removes small holes from the foreground)
		dilate(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
		erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
		//Calculate the moments of the thresholded image
		Moments oMoments = moments(imgThresholded);
		double dM01 = oMoments.m01;
		double dM10 = oMoments.m10;
		double dArea = oMoments.m00;
		cvtColor(imgOriginal, imgHSV, COLOR_BGR2HSV);
		imgTrackSquare = Mat::zeros(imgTmp.size(), CV_8UC3);
		line(imgGrid, Point(320, 0), Point(320, 480), Scalar(255, 255, 255), 1);
		imgOriginal = imgOriginal + imgGrid;
		imgGrid = Mat::zeros(imgTmp.size(), CV_8UC3);

		float graphScalar = 0.5;

		for (int i = 1; i <= 3; i++) {
			line(imgGrid, Point(320 + i * (50 / graphScalar), 0), Point(320 + i * (50 / graphScalar), 480), Scalar(250, 250, 250), 1); //positive x grid lines
			imgOriginal = imgOriginal + imgGrid;
			line(imgGrid, Point(320 - i * (50 / graphScalar), 0), Point(320 - i * (50 / graphScalar), 480), Scalar(250, 250, 250), 1); //negative x grid lines
			imgOriginal = imgOriginal + imgGrid;
		}
		for (int i = 0; i <= int(floor(480 / (50 / graphScalar))); i++) {
			line(imgGrid,
				Point(0, floor(480 - (50 / graphScalar) * floor(float(480) / (50 / graphScalar))) + i * (50 / graphScalar)),
				Point(640, floor(480 - (50 / graphScalar) * floor(float(480) / (50 / graphScalar))) + i * (50 / graphScalar)),
				Scalar(250, 250, 250), 1); //y grid lines
			imgOriginal = imgOriginal + imgGrid;
		}
		//printf("------------%f\n", float(480) / (50 / graphScalar));
		circle(imgGrid, Point(320, 480), 50/ graphScalar, Scalar(255, 160, 100), 2);
		imgOriginal = imgOriginal + imgGrid;
		circle(imgGrid, Point(320, 480), 100 / graphScalar, Scalar(255, 160, 100), 2);
		imgOriginal = imgOriginal + imgGrid;
		circle(imgGrid, Point(320, 480), 150 / graphScalar, Scalar(255, 160, 100), 2);
		imgOriginal = imgOriginal + imgGrid;
		if (dArea > trackLimit) { //creating track square and setting the coordinates for PP
			int posX = dM10 / dArea;
			int posY = dM01 / dArea;
			TrackingSquare = trackerScaledouble * dArea;
			posFromImage.z = zVal_default + 1.5*(100 - TrackingSquare);
			if (iLastX >= 0 && iLastY >= 0 && posX >= 0 && posY >= 0) {
				imgLines = Mat::zeros(imgTmp.size(), CV_8UC3);
				fakePosY = posY + (Y_offVal - 50);
				fakePosX = posX + (X_offVal - 50);
				line(imgTrackSquare, Point(fakePosX - (TrackingSquare / 2), fakePosY + (TrackingSquare / 2)), Point(fakePosX + (TrackingSquare / 2), fakePosY + (TrackingSquare / 2)), Scalar(0, 0, 255), 1);
				imgOriginal = imgOriginal + imgTrackSquare;
				line(imgTrackSquare, Point(fakePosX - (TrackingSquare / 2), fakePosY + (TrackingSquare / 2)), Point(fakePosX - (TrackingSquare / 2), fakePosY - (TrackingSquare / 2)), Scalar(0, 0, 255), 1);
				imgOriginal = imgOriginal + imgTrackSquare;
				line(imgTrackSquare, Point(fakePosX - (TrackingSquare / 2), fakePosY - (TrackingSquare / 2)), Point(fakePosX + (TrackingSquare / 2), fakePosY - (TrackingSquare / 2)), Scalar(0, 0, 255), 1);
				imgOriginal = imgOriginal + imgTrackSquare;
				line(imgTrackSquare, Point(fakePosX + (TrackingSquare / 2), fakePosY + (TrackingSquare / 2)), Point(fakePosX + (TrackingSquare / 2), fakePosY - (TrackingSquare / 2)), Scalar(0, 0, 255), 1);
				//printf("square coords: trackingSquare:%d x:%d y:%d\n", TrackingSquare, fakePosX, fakePosY);
				imgOriginal = imgOriginal + imgTrackSquare;
				line(imgTrackSquare, Point(fakePosX, fakePosY - 25), Point(fakePosX, fakePosY + 25), Scalar(0, 0, 255), 3);
				imgOriginal = imgOriginal + imgTrackSquare;
				line(imgTrackSquare, Point(fakePosX + 25, fakePosY), Point(fakePosX - 25, fakePosY), Scalar(0, 0, 255), 3);
				imgOriginal = imgOriginal + imgTrackSquare;
			}
			iLastX = posX;
			iLastY = posY;
			//sets x and y for the end-effector pos
			posFromImage = Vector3f(
				0.8 * posFromImage.x + 0.2 * (camRes.x / 2 - iLastX) * graphScalar/*this part sets x=0 as the center, meaning x ranges from [-320:320]*/,
				0.8 * posFromImage.y + 0.2 * (camRes.y - iLastY) * graphScalar,
				0.8 * posFromImage.z + 0.2 * posFromImage.z
			);
		}
		////////
		Mat flippedThreshold;
		flip(imgThresholded, flippedThreshold, 1);
		imshow("threshold image", flippedThreshold);
		Mat flippedImage;
		flip(imgOriginal, flippedImage, 1);
		string text1, text2;
		text1 = to_string(posFromImage.x);
		text2 = to_string(posFromImage.y);
		string textToDisplay = "Coords to the center: x:" + text1 + " y:" + text2 + " area:" + to_string(int(round(dArea))) + " z-value:" + to_string(posFromImage.z);
		putText(flippedImage, textToDisplay, Point(0, 15), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 255, 255), 1);
		imshow("rrreeeeeEEEEEEEEEEEEEEEEE-", flippedImage); //show the flipped image
		if (waitKey(30) == 27) //wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
		{
			cout << "esc key is pressed by user" << endl;
			destroyAllWindows();
			break;
		}
		if (dArea > trackLimit) {
			robotArm_coordsToArduino(&arduino, posFromImage);
			cout << ">> " << output << endl;
			cout << "----------\n" << endl;
		}
		Sleep(loopDelayTimer);
	}
}

void openCV_armAccuracy_test() {
	//put every point coordinate from .dat file in a string array holding the entire line of text
	//create 5 separate string arrays of the end position gotten with opencv (to then be compared with the given coords later. 5 arrays because of
	// 5 a.b. combinations)
	/*
		give robot arm an XYZ coord from string array with a certain a.b choice (-45.0, 45.0, 0.-45, 0.45, 0.0)
		wait
		read position with opencv
		store in respective array element (i.e. the end array element is the same as the start array element)
		repeat for all elements with said orientation
		store the array data in a new .dat file with the orientation name (a.b)
		repeat for all five orientation combinations
	*/
	system("cls");

	VideoCapture TempCap(1); //capture the video from web cam
	if (!TempCap.isOpened()) {// if not success, exit program
		cout << "Cannot open the web cam" << endl;
		return;
	}
	namedWindow("Control", WINDOW_AUTOSIZE); //create a window called "Control"
	int iLowH = 0;
	int iHighH = 179;
	int iLowS = 0;
	int iHighS = 209;
	int iLowV = 230;
	int iHighV = 255;
	long int pauseVal = -1;
	bool normalPath = true;
	//Create trackbars in "Control" window
	createTrackbar("LowH", "Control", &iLowH, 179); //Hue (0 - 179)
	createTrackbar("HighH", "Control", &iHighH, 179);
	createTrackbar("LowS", "Control", &iLowS, 255); //Saturation (0 - 255)
	createTrackbar("HighS", "Control", &iHighS, 255);
	createTrackbar("LowV", "Control", &iLowV, 255); //Value (0 - 255)
	createTrackbar("HighV", "Control", &iHighV, 255);
	Mat imgTemp;
	while (true) {
		pauseVal = waitKey(5);
		if (pauseVal == 32) { normalPath = false; }
		else if (pauseVal == 8) { normalPath = true; }
		if (normalPath) {
			bool bSuccess = TempCap.read(imgTemp); // read a new frame from video
			if (!bSuccess) //if not success, break loop
			{ cout << "Cannot read a frame from video stream" << endl; break; }
		}
		Mat imgGrid = Mat::zeros(imgTemp.size(), CV_8UC3); //the grid on the background of the displayed image
		line(imgGrid, Point(320, 0), Point(320, 480), Scalar(255, 255, 255), 1);
		imgTemp = imgTemp + imgGrid;
		imgGrid = Mat::zeros(imgTemp.size(), CV_8UC3);
		Vector2f camRes = Vector2f(640, 480); //the resolution of the camera output in xy
		float graphScalar = 0.5;
		for (int i = 1; i <= 3; i++) {
			line(imgGrid, Point(320 + i * (50 / graphScalar), 0), Point(320 + i * (50 / graphScalar), 480), Scalar(250, 250, 250), 1); //positive x grid lines
			imgTemp = imgTemp + imgGrid;
			line(imgGrid, Point(320 - i * (50 / graphScalar), 0), Point(320 - i * (50 / graphScalar), 480), Scalar(250, 250, 250), 1); //negative x grid lines
			imgTemp = imgTemp + imgGrid;
		}
		for (int i = 0; i <= int(floor(480 / (50 / graphScalar))); i++) {
			line(imgGrid,
				Point(0, floor(480 - (50 / graphScalar) * floor(float(480) / (50 / graphScalar))) + i * (50 / graphScalar)),
				Point(640, floor(480 - (50 / graphScalar) * floor(float(480) / (50 / graphScalar))) + i * (50 / graphScalar)),
				Scalar(250, 250, 250), 1); //y grid lines
			imgTemp = imgTemp + imgGrid;
		}
		//printf("------------%f\n", float(480) / (50 / graphScalar));
		circle(imgGrid, Point(320, 480), 50 / graphScalar, Scalar(255, 160, 100), 2);
		imgTemp = imgTemp + imgGrid;
		circle(imgGrid, Point(320, 480), 100 / graphScalar, Scalar(255, 160, 100), 2);
		imgTemp = imgTemp + imgGrid;
		circle(imgGrid, Point(320, 480), 150 / graphScalar, Scalar(255, 160, 100), 2);
		imgTemp = imgTemp + imgGrid;
		Mat imgTempHSV;
		cvtColor(imgTemp, imgTempHSV, COLOR_BGR2HSV); //Convert the captured frame from BGR to HSV
		Mat imgTempThresholded;
		inRange(imgTempHSV, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), imgTempThresholded); //Threshold the image
		//morphological opening (remove small objects from the foreground)
		erode(imgTempThresholded, imgTempThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
		dilate(imgTempThresholded, imgTempThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
		//morphological closing (fill small holes in the foreground)
		dilate(imgTempThresholded, imgTempThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
		erode(imgTempThresholded, imgTempThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
		imshow("Thresholded Image", imgTempThresholded); //show the thresholded image
		imshow("Original", imgTemp); //show the temporary image
		if (waitKey(30) == 27) {//wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
			cout << "esc key is pressed by user" << endl;
			destroyAllWindows();
			return;
		}
		if (waitKey(30) == 18) { //wait for 'ctrl + r'
			cout << "Starting Tracking..." << endl;
			destroyAllWindows();
			break;
		}
	}


	SerialPort arduino(port);

	if (arduino.isConnected()) { cout << "Connection established" << endl << endl; }
	else { cout << "Error in port name" << endl << endl; }

	string input[213];
	string ab0[213], ab1[213], ab2[213], ab3[213], ab4[213]; // (0:0.0, 1:-45.0, 2:45.0, 3:0.-45, 4:0.45); output array containing the actual position AND the error rate compared to the input array
	/*
	note: the 4, 5 and 6th column will be the the error rate, i.e. the delta value between the given and seen position of end-effector
	*/

	int arrLength = 39;

	string preName = "z216";
	ifstream myfile("ws-calc/opencv acc. test/res10_" + preName + ".dat");

	if (!myfile.is_open()) { cout << "can't open myfile address file\n"; return; }
	getline(myfile, input[0]);
	for (int i = 0; i < arrLength; i++) { getline(myfile, input[i]); }
	myfile.close();


	int armToCap_timer = 1000; //in ms
	Vector3f PP; //what's sent
	char posOption = '-';


	//video tracking
	VideoCapture cap(1); //capture the video from webcam
	bool foundPos = true;
	Vector3f opencvPos = Vector3f(0, 0, 0); //what's read/seen
	float a, b; //in radians
	float camScalar = 2; //1 means each square has a length of 50mm, 2 will be 100mm

	a = toRadians(0); b = toRadians(0);
	printf("array: a0.b0\n");
	for (int i = 0; i < arrLength; i++) {
		string temp = input[i];
		cout << temp << endl;
		PP.x = stof(temp.substr(0, temp.find(" ", 0)));
		temp.erase(0, temp.find(" ", 0) + 1);
		PP.y = stof(temp.substr(0, temp.find(" ", 0)));
		temp.erase(0, temp.find(" ", 0) + 1);
		PP.z = stof(temp.substr(0, temp.find(" ", 0)));
		if (areAngles_NaN(PP, a, b, posOption)) {
			ab0[i] = "#unreachable\n";
		}
		else {
			sendCoords_7DOF_sendCoords(PP, posOption, &arduino, a, b, 0, false, 100, "true");
			Sleep(armToCap_timer);
			opencv_trackColor(
				&opencvPos,
				&cap,
				&foundPos,
				true,
				iLowH,
				iHighH,
				iLowS,
				iHighS,
				iLowV,
				iHighV,
				camScalar
			);
			if (foundPos) {
				ab0[i] =
					to_string(opencvPos.x) + " " +
					to_string(opencvPos.y) + " " +
					to_string(opencvPos.z) + " " +
					to_string(PP.x - opencvPos.x) + " " +
					to_string(PP.y - opencvPos.y) + " " +
					to_string(PP.z - opencvPos.z) + " " +
					"\n";
			}
			else if (!foundPos) {
				ab0[i] = "error\n";
			}
		}
	}

	printf("array: a-45.b0\n");
	a = toRadians(-45); b = toRadians(0);
	for (int i = 0; i < arrLength; i++) {
		string temp = input[i];
		PP.x = stof(temp.substr(0, temp.find(" ", 0)));
		temp.erase(0, temp.find(" ", 0) + 1);
		PP.y = stof(temp.substr(0, temp.find(" ", 0)));
		temp.erase(0, temp.find(" ", 0) + 1);
		PP.z = stof(temp.substr(0, temp.find(" ", 0)));
		if (areAngles_NaN(PP, a, b, posOption)) {
			ab1[i] = "#unreachable\n";
		}
		else {
			sendCoords_7DOF_sendCoords(PP, posOption, &arduino, a, b, 0, false, 100, "true");
			Sleep(armToCap_timer);
			opencv_trackColor(
				&opencvPos,
				&cap,
				&foundPos,
				true,
				iLowH,
				iHighH,
				iLowS,
				iHighS,
				iLowV,
				iHighV,
				camScalar
			);
			if (foundPos) {
				ab1[i] =
					to_string(opencvPos.x) + " " +
					to_string(opencvPos.y) + " " +
					to_string(opencvPos.z) + " " +
					to_string(PP.x - opencvPos.x) + " " +
					to_string(PP.y - opencvPos.y) + " " +
					to_string(PP.z - opencvPos.z) + " " +
					"\n";
			}
			else if (!foundPos) {
				ab1[i] = "error\n";
			}
		}
	}

	printf("array: a45.b0\n");
	a = toRadians(45); b = toRadians(0);
	for (int i = 0; i < arrLength; i++) {
		string temp = input[i];
		PP.x = stof(temp.substr(0, temp.find(" ", 0)));
		temp.erase(0, temp.find(" ", 0) + 1);
		PP.y = stof(temp.substr(0, temp.find(" ", 0)));
		temp.erase(0, temp.find(" ", 0) + 1);
		PP.z = stof(temp.substr(0, temp.find(" ", 0)));
		if (areAngles_NaN(PP, a, b, posOption)) {
			ab2[i] = "#unreachable\n";
		}
		else {
			sendCoords_7DOF_sendCoords(PP, posOption, &arduino, a, b, 0, false, 100, "true");
			Sleep(armToCap_timer);
			opencv_trackColor(
				&opencvPos,
				&cap,
				&foundPos,
				true,
				iLowH,
				iHighH,
				iLowS,
				iHighS,
				iLowV,
				iHighV,
				camScalar
			);
			if (foundPos) {
				ab2[i] =
					to_string(opencvPos.x) + " " +
					to_string(opencvPos.y) + " " +
					to_string(opencvPos.z) + " " +
					to_string(PP.x - opencvPos.x) + " " +
					to_string(PP.y - opencvPos.y) + " " +
					to_string(PP.z - opencvPos.z) + " " +
					"\n";
			}
			else if (!foundPos) {
				ab2[i] = "error\n";
			}
		}
	}

	printf("array: a0.b-45\n");
	a = toRadians(0); b = toRadians(-45);
	for (int i = 0; i < arrLength; i++) {
		string temp = input[i];
		PP.x = stof(temp.substr(0, temp.find(" ", 0)));
		temp.erase(0, temp.find(" ", 0) + 1);
		PP.y = stof(temp.substr(0, temp.find(" ", 0)));
		temp.erase(0, temp.find(" ", 0) + 1);
		PP.z = stof(temp.substr(0, temp.find(" ", 0)));
		printf("x:%f y:%f z:%f\n", PP.x, PP.y, PP.z);
		if (areAngles_NaN(PP, a, b, posOption)) {
			ab3[i] = "#unreachable\n";
		}
		else {
			sendCoords_7DOF_sendCoords(PP, posOption, &arduino, a, b, 0, false, 100, "true");
			Sleep(armToCap_timer);
			opencv_trackColor(
				&opencvPos,
				&cap,
				&foundPos,
				true,
				iLowH,
				iHighH,
				iLowS,
				iHighS,
				iLowV,
				iHighV,
				camScalar
			);
			if (foundPos) {
				ab3[i] =
					to_string(opencvPos.x) + " " +
					to_string(opencvPos.y) + " " +
					to_string(opencvPos.z) + " " +
					to_string(PP.x - opencvPos.x) + " " +
					to_string(PP.y - opencvPos.y) + " " +
					to_string(PP.z - opencvPos.z) + " " +
					"\n";
			}
			else if (!foundPos) {
				ab3[i] = "error\n";
			}
		}
	}

	printf("array: a0.b45\n");
	a = toRadians(0); b = toRadians(45);
	for (int i = 0; i < arrLength; i++) {
		string temp = input[i];
		PP.x = stof(temp.substr(0, temp.find(" ", 0)));
		temp.erase(0, temp.find(" ", 0) + 1);
		PP.y = stof(temp.substr(0, temp.find(" ", 0)));
		temp.erase(0, temp.find(" ", 0) + 1);
		PP.z = stof(temp.substr(0, temp.find(" ", 0)));
		if (areAngles_NaN(PP, a, b, posOption)) {
			ab4[i] = "#unreachable\n";
		}
		else {
			sendCoords_7DOF_sendCoords(PP, posOption, &arduino, a, b, 0, false, 100, "true");
			Sleep(armToCap_timer);
			opencv_trackColor(
				&opencvPos,
				&cap,
				&foundPos,
				true,
				iLowH,
				iHighH,
				iLowS,
				iHighS,
				iLowV,
				iHighV,
				camScalar
			);
			if (foundPos) {
				ab4[i] =
					to_string(opencvPos.x) + " " +
					to_string(opencvPos.y) + " " +
					to_string(opencvPos.z) + " " +
					to_string(PP.x - opencvPos.x) + " " +
					to_string(PP.y - opencvPos.y) + " " +
					to_string(PP.z - opencvPos.z) + " " +
					"\n";
			}
			else if (!foundPos) {
				ab4[i] = "error\n";
			}
		}
	}

	ofstream r0_0("ws-calc/opencv acc. test/" + preName + "_0.0.dat");
	r0_0 << "#x y z\n";
	ofstream r_45_0("ws-calc/opencv acc. test/" + preName + "_-45.0.dat");
	r_45_0 << "#x y z\n";
	ofstream r45_0("ws-calc/opencv acc. test/" + preName + "_45.0.dat");
	r45_0 << "#x y z\n";
	ofstream r0__45("ws-calc/opencv acc. test/" + preName + "_0.-45.dat");
	r0__45 << "#x y z\n";
	ofstream r0_45("ws-calc/opencv acc. test/" + preName + "_0.45.dat");
	r0_45 << "#x y z\n";

	printf("------------------\n");
	for (int i = 0; i < arrLength; i++) {
		printf(" i:%d\n", i);
		r0_0 << ab0[i];
		r_45_0 << ab1[i];
		r45_0 << ab2[i];
		r0__45 << ab3[i];
		r0_45 << ab4[i];
	}
	r0_0.close();
	r_45_0.close();
	r45_0.close();
	r0__45.close();
	r0_45.close();

	printf("\nFinished\n");
	destroyAllWindows();
	system("pause");
}

void runServoDegrees_fromFile() {
	/*commands
	sleep100 - Sleep is the initiator and 100 is the time in ms
	# - ignores that line
	*/
	/*format
	q1:q2:q3:q4:q5:q6
	*/

	SerialPort arduino(port);

	if (arduino.isConnected()) { cout << "Connection established" << endl << endl; }
	else { cout << "Error in port name" << endl << endl; }

	system("cls");

	string textInput;
	cout << " 1.Use direct rotation commands\n";
	cout << " 2.Use position commands\n";
	cout << "input: ";
	cin >> textInput;
	if (textInput == "exit") { return; }



	if (stoi(textInput) == 1) {
		int q1_default = 90, //in degrees
			q2_default = 0, //in degrees
			q3_default = 135, //in degrees
			q4_default = 90, //in degrees
			q5_default = 90, //in degrees
			q6_default = 90; //in degrees

		float q1 = q1_default,
			q2 = toRadians(q2_default),
			q3 = toRadians(q3_default),
			q4 = toRadians(q4_default),
			q5 = toRadians(q5_default),
			q6 = toRadians(q6_default); //in radians
		int servoSpeed = 1;
		bool printText = true;
		string filePath, input, useTimer = "true";
		ifstream myfile;

		while (true) { //send straight rotation commands with sleep values
			while (true) {
				cout << " Enter .txt file path with \" marks: ";
				cin >> input;
				if (input == "exit") { return; }
				filePath = input;

				myfile.open(filePath);
				if (!myfile.is_open()) {
					cout << "could not open file. Try again\n";
					Sleep(1000);
				}
				else {
					break;
				}
			}
			string temp;

			while (true) { //loop to send the servo degrees to the robotarm
				getline(myfile, temp);
				if (temp.substr(0, 5) == "sleep") {
					printf("Sleep at: %d seconds..\n", stoi(temp.substr(5, temp.find("\n", 0) - 5)));
					Sleep(stoi(temp.substr(5, temp.find("\n", 0) - 5)));
				}
				else if (temp.substr(0, 1) == "\n") {}
				else if (temp.substr(0, 1) == "") {}
				else if (temp.substr(0, 1) == "#") {}
				else {
					q1 = toRadians(stof(temp.substr(0, temp.find(":", 0))));
					temp.erase(0, temp.find(":", 0) + 1);
					q2 = toRadians(stof(temp.substr(0, temp.find(":", 0))));
					temp.erase(0, temp.find(":", 0) + 1);
					q3 = toRadians(stof(temp.substr(0, temp.find(":", 0))));
					temp.erase(0, temp.find(":", 0) + 1);
					q4 = toRadians(stof(temp.substr(0, temp.find(":", 0))));
					temp.erase(0, temp.find(":", 0) + 1);
					q5 = toRadians(stof(temp.substr(0, temp.find(":", 0))));
					temp.erase(0, temp.find(":", 0) + 1);
					q6 = toRadians(stof(temp.substr(0, 3)));


					string data;
					data = (
						to_string(q1_default - int(round(toDegrees(q1)))) + ":" +
						to_string(q2_default + int(round(toDegrees(q2)))) + ":" +
						to_string(q3_default + int(round(toDegrees(q3)))) + ":" +
						to_string(q4_default + int(round(toDegrees(q4)))) + ":" + //note: it's set to be "reverse" because the playing field is in "reverse"
						to_string(q5_default + int(round(toDegrees(q5)))) + ":" +
						to_string(q6_default + int(round(toDegrees(q6)))) + "s" +
						to_string(servoSpeed) + "t" +
						useTimer
						);
					//cout << "---- sent:" << data << " with a:" << toDegrees(a) << " b:" << toDegrees(b) << " Y:" << toDegrees(Y) << endl;
					if (printText)
						printf("--- sent:%s\n", data.c_str());

					char* charArray = new char[data.size() + 1];
					addNULL_char(data, charArray);
					arduino.writeSerialPort(charArray, data.size() + 1);
					delete[] charArray;
					arduino.readSerialPort(output, MAX_DATA_LENGTH);
				}

				Sleep(10);
				if (myfile.eof()) { break; }
			}
			myfile.close();
		}
	}

	if (stoi(textInput) == 2) {

		string input[213];

		int arrLength = 29; //length of rows in the file

		string fileName = "rand1";
		ifstream myfile("ws-calc/opencv acc. test/" + fileName + ".dat");

		if (!myfile.is_open()) { cout << "can't open myfile address file\n"; return; }
		getline(myfile, input[0]); //to read the first line of the .dat file because i contains #x y z
		for (int i = 0; i < arrLength; i++) { getline(myfile, input[i]); } //read and store string rows from .dat in string array
		myfile.close();


		Vector3f PP; //what's sent
		char posOption = '-';

		int iterationDelay = 1000;

		//video tracking
		VideoCapture cap(1); //capture the video from webcam
		Vector3f opencvPos = Vector3f(0, 0, 0); //what's read/seen
		float a = 0, b = 0; //in radians
		float camScalar = 2; //1 means each square has a length of 50mm, 2 will be 100mm
		string temp;

		for (int i = 0; i < arrLength; i++) {
			temp = input[i];
			if (temp.substr(0, 5) == "sleep") {
				printf("Sleep at: %d seconds..\n", stoi(temp.substr(5, temp.find("\n", 0) - 5)));
				Sleep(stoi(temp.substr(5, temp.find("\n", 0) - 5)));
			}
			else if (temp.substr(0, 1) == "\n") {}
			else if (temp.substr(0, 1) == "") {}
			else if (temp.substr(0, 1) == "#") {}
			else {
				cout << temp << endl;
				PP.x = stof(temp.substr(0, temp.find(" ", 0)));
				temp.erase(0, temp.find(" ", 0) + 1);
				PP.y = stof(temp.substr(0, temp.find(" ", 0)));
				temp.erase(0, temp.find(" ", 0) + 1);
				PP.z = stof(temp.substr(0, temp.find(" ", 0)));
				if (areAngles_NaN(PP, a, b, posOption)) {
					bool solvedNaN = false;
					solveNaN(PP, &a, &b, &solvedNaN, posOption);
					if (solvedNaN) {
						sendCoords_7DOF_sendCoords(PP, posOption, &arduino, a, b, 0, false, 5, "true");
					}
					else { printf("can't reach coord on row: %d\n", i); }
				}
				else {
					sendCoords_7DOF_sendCoords(PP, posOption, &arduino, a, b, 0, false, 5, "true");
				}
				Sleep(iterationDelay);
			}
		}
	}
}
