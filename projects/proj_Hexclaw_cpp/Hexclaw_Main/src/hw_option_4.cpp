/**
 * @file hw_option_4.cpp
 * @author your name (you@domain.com)
 * @brief Calibrate servo motors by reading error rate and creating correction functions. (NOTE: will be changes so that in the future option 4 contains all the calibration methods/functions for various things, HSV, servo error rate e.t.)
 * @version 0.1
 * @date 2025-02-02
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include "hw_options.hpp"

#include <ctime>
#include <fstream>

TUI::termMenu menu__calibrateMotor_main({
	{" Motors", 0, 0, TUI::CELLOPTSPEC_TEXT}, {" Error regr", 1, 0, TUI::CELLOPTSPEC_TEXT}, {" Regr solution", 2, 0, TUI::CELLOPTSPEC_TEXT},
	
	{" q[0]", 0, 2, '0'}, {std::string(15, ' '), 1, 2, TUI::CELLOPTSPEC_TEXT}, {" [auto] ", 3, 2, -1},
	{" q[1]", 0, 3, '1'}, {std::string(15, ' '), 1, 3, TUI::CELLOPTSPEC_TEXT}, {" [auto] ", 3, 3, -1},
	{" q[2]", 0, 4, '2'}, {std::string(15, ' '), 1, 4, TUI::CELLOPTSPEC_TEXT}, {" [auto] ", 3, 4, -1},
	{" q[3]", 0, 5, '3'}, {std::string(15, ' '), 1, 5, TUI::CELLOPTSPEC_TEXT}, {" [auto] ", 3, 5, -1},
	{" q[4]", 0, 6, '4'}, {std::string(15, ' '), 1, 6, TUI::CELLOPTSPEC_TEXT}, {" [auto] ", 3, 6, -1},
	{" q[5]", 0, 7, '5'}, {std::string(15, ' '), 1, 7, TUI::CELLOPTSPEC_TEXT}, {" [auto] ", 3, 7, -1},

	{" back", 0, 9, 27}
}, false);

TUI::termMenu menu__calibrateMotor_angles({


	{"   0", 1,  2, -1}, {std::string(17, ' '), 2,  2, TUI::CELLOPTSPEC_TEXT},
	{"  10", 1,  3, -1}, {std::string(17, ' '), 2,  3, TUI::CELLOPTSPEC_TEXT},
	{"  20", 1,  4, -1}, {std::string(17, ' '), 2,  4, TUI::CELLOPTSPEC_TEXT},
	{"  30", 1,  5, -1}, {std::string(17, ' '), 2,  5, TUI::CELLOPTSPEC_TEXT},
	{"  40", 1,  6, -1}, {std::string(17, ' '), 2,  6, TUI::CELLOPTSPEC_TEXT},
	{"  50", 1,  7, -1}, {std::string(17, ' '), 2,  7, TUI::CELLOPTSPEC_TEXT},
	{"  60", 1,  8, -1}, {std::string(17, ' '), 2,  8, TUI::CELLOPTSPEC_TEXT},
	{"  70", 1,  9, -1}, {std::string(17, ' '), 2,  9, TUI::CELLOPTSPEC_TEXT},
	{"  80", 1, 10, -1}, {std::string(17, ' '), 2, 10, TUI::CELLOPTSPEC_TEXT},
	{"  90", 1, 11, -1}, {std::string(17, ' '), 2, 11, TUI::CELLOPTSPEC_TEXT},
	{" 100", 1, 12, -1}, {std::string(17, ' '), 2, 12, TUI::CELLOPTSPEC_TEXT},
	{" 110", 1, 13, -1}, {std::string(17, ' '), 2, 13, TUI::CELLOPTSPEC_TEXT},
	{" 120", 1, 14, -1}, {std::string(17, ' '), 2, 14, TUI::CELLOPTSPEC_TEXT},
	{" 130", 1, 15, -1}, {std::string(17, ' '), 2, 15, TUI::CELLOPTSPEC_TEXT},
	{" 140", 1, 16, -1}, {std::string(17, ' '), 2, 16, TUI::CELLOPTSPEC_TEXT},
	{" 150", 1, 17, -1}, {std::string(17, ' '), 2, 17, TUI::CELLOPTSPEC_TEXT},
	{" 160", 1, 18, -1}, {std::string(17, ' '), 2, 18, TUI::CELLOPTSPEC_TEXT},
	{" 170", 1, 19, -1}, {std::string(17, ' '), 2, 19, TUI::CELLOPTSPEC_TEXT},
	{" 180", 1, 20, -1}, {std::string(17, ' '), 2, 20, TUI::CELLOPTSPEC_TEXT},

	{" Solve",1, 21, -1},
	
	{" back", 1, 23, 27},
	{" exit", 1, 24, 27}
}, false);


/**
 * A vector of preset servo angles for a 6-DOF robotic arm.
 * 
 * This vector contains predefined sets of angles for the servos of a 
 * 6-degree-of-freedom robotic arm. Each element in the vector represents 
 * a specific configuration of the arm, with six angles corresponding to 
 * the six servos.
 * 
 * The angles are specified in degrees and follow the order:
 * {servo1, servo2, servo3, servo4, servo5, servo6}.
 * 
 * Preset configurations:
 * - { -90,  90,   0,   0,   0,   0} : Initial position with servo1 at -90° and servo2 at 90°.
 * - {   0,   0, -45,   0,   0,   0} : Servo3 is tilted to -45°.
 * - {   0, 135,-135,   0,   0,   0} : Servo2 and servo3 are at 135° and -135°, respectively.
 * - {   0,  90, -90, -90,   0,   0} : Servo4 is tilted to -90°.
 * - {   0,  90,   0,   0, -90,   0} : Servo5 is tilted to -90°.
 * - {   0,  90,   0,   0, -90, -90} : Servo5 and servo6 are tilted to -90°.
 * 
 * note: These configurations can be used as starting points or presets 
 *       for controlling the robotic arm.
 */
std::vector<servo_angles_6DOF> presetAngles_start{
	{ -90,  90,   0,   0,   0,   0},	// q1
	{   0,   0, -45,   0,   0,   0},	// q2
	{   0, 135,-135,   0,   0,   0},	// q3
	{   0,  90, -90, -90,   0,   0},	// q4
	{   0,  90,   0,   0, -90,   0},	// q5
	{   0,  90,   0,   0, -90, -90}		// q6
};
/**
 * A vector of preset servo angles for a 6 Degrees of Freedom (6DOF) robotic arm.
 * 
 * This vector contains predefined sets of angles for the servos in the robotic arm to reach.
 * Each element in the vector represents a configuration of servo angles, where:
 * - The first value corresponds to the angle of the first servo.
 * - The second value corresponds to the angle of the second servo.
 * - The third value corresponds to the angle of the third servo.
 * - The fourth value corresponds to the angle of the fourth servo.
 * - The fifth value corresponds to the angle of the fifth servo.
 * - The sixth value corresponds to the angle of the sixth servo.
 * 
 * These preset angles can be used to move the robotic arm to specific positions or
 * perform predefined actions.
 */
std::vector<servo_angles_6DOF> presetAngles_end{
	{  90,  90,   0,   0,   0,   0},	// q1
	{   0, 180, -90,   0,   0,   0},	// q2
	{   0, 135,  45,   0,   0,   0},	// q3
	{   0,  90, -90,  90,   0,   0},	// q4
	{   0,  90,   0,   0,  90,   0},	// q5
	{   0,  90,   0,   0, -90,  90}		// q6
};

float extractAngle() {
	return orientObj.pitch+90;
}

void testMotor(size_t motorID) {
	if(motorID>=6) throw std::runtime_error("testMotor: motorID is out of range: "+std::to_string(motorID));
	servo_angles_6DOF newAngles = oldRotation
}

void _updateFunc_option4_servoCalibration(TUI::termMenu* ptr_menu) {
	std::string textCell_text = "";
	bool TextCellModified = false;
	try {
		orientObj.update(false);
		textCell_text += "{";
		textCell_text += "p:"+formatNumber(orientObj.pitch, 6, 1) + ",";
		textCell_text += "r:"+formatNumber(orientObj.roll, 6, 1);
		textCell_text += "}";
		TextCellModified = true;
	}
	catch(const std::exception& e) {
		textCell_text += e.what();
		TextCellModified = true;
	}
	ptr_menu->addTextCell(" FPS: "+formatNumber(ptr_menu->FPS, 5, 1), 3, 0);
	ptr_menu->addTextCell(textCell_text, 3, 1);
	
	if(TextCellModified) {
		ptr_menu->updateTable();
	}
	
}

void HW_option4() {
	// simplified_init();
	if((!_init_status.get("orientObj").isInit())) { // any options within this option requires orientObj to be available/initialised.
		ANSI_mvprint(0, 0, "ERROR: both pca and orientObj has not been initialised (required for either).", true, "abs", "rel");
		if(!_init_status.get("pca").isInit()) 		ANSI_mvprint(0, 0, "ERROR: pca    	 : "+_init_status.get("pca").get_callMsg(), true, "abs", "rel");
		if(!_init_status.get("orientObj").isInit()) ANSI_mvprint(0, 0, "ERROR: orientObj : "+_init_status.get("orientObj").get_callMsg(), true, "abs", "rel");
		for(int _=0; _<3; _++) {
			// std::cout << " ."; std::cout.flush();
			ANSI_mvprint(0, -1, " .", true, "rel", "rel");
			SHLEEP((1'000));
		}
		return;
	}
	
	size_t numPolynomials = 3;


	// Total container for all the data readings
	std::vector<std::vector<float>> servoMotor_readAngleData(6, std::vector<float>(181, -1));

	std::vector<std::vector<float>> servoMotor_coeffs(6, std::vector<float>(numPolynomials, 0));
	std::vector<std::vector<float>> servoMotor_coeffs_sol(6, std::vector<float>(numPolynomials, 0));

	bool motorsCalibrated = false;

	bool runMain = true;
	while(runMain) {
		printFuncLabel(" Running: opt4: Calibrate servo motors");
		ANSI_mvprint(0, 0, "Select a joint/motor to calibrate", true, "abs", "rel", true);

		pos2d<int> pressed_pos = menu__calibrateMotor_main.driver(1, 5, 5, false, _updateFunc_option4_servoCalibration, false);

		if(pressed_pos==pos2d<int>(0, 9)) { // "back" option pressed

			if(motorsCalibrated) {
				try {
					std::fstream file_readAngles(_XSTRINGLIT(_PROGRAM_PATH__DATA)+std::string("/data_servoMotorDrift/servoMotorDrift_readAngles.csv"), std::ios_base::app);
					std::fstream file_regrFunc_read(_XSTRINGLIT(_PROGRAM_PATH__DATA)+std::string("/data_servoMotorDrift/servoMotorDrift_regrFunc_read.csv"), std::ios_base::app);
					std::fstream file_regrFunc_sol(_XSTRINGLIT(_PROGRAM_PATH__DATA)+std::string("/data_servoMotorDrift/servoMotorDrift_regrFunc_sol.csv"), std::ios_base::app);
		
					if(!file_readAngles.is_open()) 		throw std::runtime_error("file failed to open: file_readAngles");
					if(!file_regrFunc_read.is_open()) 	throw std::runtime_error("file failed to open: file_regrFunc_read");
					if(!file_regrFunc_sol.is_open()) 	throw std::runtime_error("file failed to open: file_regrFunc_sol");

					time_t timestamp;
					time(&timestamp);
					std::string dateStr = ctime(&timestamp);
					if(dateStr.at(dateStr.size()-1)=='\n') dateStr = dateStr.substr(0, dateStr.size()-1);

					/// Fill in readAngles file
					for(int angle=0; angle<=180; angle++) {
						file_readAngles << dateStr << "," << angle;
						for(size_t motor=0; motor<=5; motor++) {
							file_readAngles << "," << servoMotor_readAngleData.at(motor).at(angle);
						}
						file_readAngles << "\n";
					}
		
					/// Fill in regrFunc file(s)
					for(size_t motor=0; motor<=5; motor++) {
						file_regrFunc_read	<< dateStr << "," << motor;
						file_regrFunc_sol 	<< dateStr << "," << motor;
						for(size_t numCoeffs=0; numCoeffs<10; numCoeffs++) {
							file_regrFunc_read	<< "," << (numCoeffs<servoMotor_coeffs.at(motor).size()? 		servoMotor_coeffs.at(motor).at(numCoeffs) : -1);
							file_regrFunc_sol 	<< "," << (numCoeffs<servoMotor_coeffs_sol.at(motor).size()? 	servoMotor_coeffs_sol.at(motor).at(numCoeffs) : -1);
						}
						file_regrFunc_read	<< "\n";
						file_regrFunc_sol	<< "\n";
					}
		
					file_readAngles.close();
					file_regrFunc_read.close();
					file_regrFunc_sol.close();
					
				}
				catch(const std::exception& e) {
					std::cerr << e.what() << std::endl;
					SHLEEP(1000);
					exit(1);
				}
			}

			break;
		}
		if(pressed_pos.inRegion({0, 2}, {0, 7})) { // pressed_pos is in region of the selectable motor
			ANSI_mvprint(0, 2, "Calibrating motor: q["+formatNumber(pressed_pos[1]-2)+"]", true, "abs", "abs", false);
			
			// calibrate motor
			bool runAngles = true;
			while(runAngles) {
				ANSI_mvprint(0, 0, "Select an angle to calibrate", true, "abs", "abs", true);
				pos2d<int> pressed_pos_angles = menu__calibrateMotor_angles.driver(1, 4, 5, false, _updateFunc_option4_servoCalibration, false);
				if(pressed_pos_angles==pos2d<int>(0, 9)) break;
				if(pressed_pos_angles.inRegion({1, 2}, {1, 20})) { // Pressed button in region of the selectable motor angles
					if(_init_status.get("pca").isInit()) {
						// set motor to angle
						servo_angles_6DOF newAngles(0);
						int motorIdx = pressed_pos[1]-2;
						float angle = (pressed_pos_angles[1]-2)*10.0f;
						// float rad = toRadians(angle);
						newAngles[motorIdx] = angle;
						sendToServo(&pca, newAngles, current_q, false, 0, 0, false);
					}
					else {
						// ANSI_mvprint(0, 0, "ERROR: pca has not been initialised: "+_init_status.get("pca").get_callMsg(), true, "abs", "rel");
					}
					SHLEEP((500));
					// orientObj.update(false);

					menu__calibrateMotor_angles.addTextCell(formatNumber(orientObj.pitch+90, 7, 1), 2, pressed_pos_angles[1]);

					/// Manual input of angles
					// std::string inpValue = menu__calibrateMotor_angles.termInput(0);
					// if(inpValue=="exit") return;
					// else if(inpValue=="back") break;
					// else {
					// 	menu__calibrateMotor_angles.rename_opt(2, pressed_pos[1], inpValue);
					// }
				}
				else if(pressed_pos_angles==pos2d<int>(1, 21)) { // "Solve" button pressed
					// solve servo drift regression
					std::vector<float> refrAngles(19, 0);
					std::vector<float> readAngles(19, 0);
					for(size_t i=0; i<19; i++) {
						refrAngles[i] = i*100;
						try {
							readAngles[i] = std::stof(menu__calibrateMotor_angles.getCellName(2, i+2));
						}
						catch(const std::invalid_argument& e) {
							ANSI_mvprint(0, 0, "ERROR: invalid input: \""+menu__calibrateMotor_angles.getCellName(2, i+2)+"\" "+std::string(e.what()), true, "abs", "rel");
							std::this_thread::sleep_for(std::chrono::seconds(1));
							readAngles[i] = 0;
							// break;
						}
					}

					ANSI_mvprint(0, 0, "refrAngles: "+formatVector(refrAngles, 6, 1), true, "abs", "rel");
					ANSI_mvprint(0, 1, "readAngles: "+formatVector(readAngles, 6, 1), true, "abs", "rel");

					std::vector<float> coeffs 		= HW_KINEMATICS::solveServoDriftRegression(refrAngles, readAngles, numPolynomials);
					std::vector<float> coeffs_sol	= HW_KINEMATICS::solveServoDriftRegression(readAngles, refrAngles, numPolynomials);
					
					menu__calibrateMotor_main.addTextCell(formatVector(coeffs, 6, 2), 1, pressed_pos[1]);
					menu__calibrateMotor_main.addTextCell(formatVector(coeffs_sol, 6, 2), 2, pressed_pos[1]);
					
					ANSI_mvprint(0, 2, "solved coeffs: "+formatVector(coeffs, 6, 1), true, "abs", "rel");
					std::this_thread::sleep_for(std::chrono::seconds(2));
					for(size_t cnt=0; cnt<3; cnt++) {
						SHLEEP(1000);
						ANSI_mvprint(0, -1, ".", true, "rel", "abs");
					}
				}
				else if(pressed_pos_angles==pos2d<int>(1, 23)) { // "back" button pressed
					runAngles = false;
				}
				else if(pressed_pos_angles==pos2d<int>(1, 24)) { // "exit" button pressed
					runAngles = false;
					runMain = false;
				}

			}
		}
		if(pressed_pos.inRegion({3, 2}, {3, 7})) { // pressed_pos is in region of the [auto] for selectable motor.
			// Index to the motor that's being tested
			int motorIdx = pressed_pos[1]-2;
			ANSI_mvprint(0, 0, "Running automated servo motor read for q["+std::to_string(motorIdx)+"]", true, "abs", "abs", true);

			std::vector<float> refrAngles;
			std::vector<float> readAngles;

			std::vector<float> coeffs		= std::vector<float>(numPolynomials, 0);
			std::vector<float> coeffs_sol	= std::vector<float>(numPolynomials, 0);

			float progress = 0;
			servo_angles_6DOF anglesToSend(0);
			for(float angle=0; angle<=180; angle+=1) {
				ANSI_mvprint(0, 3, "angle: "+formatNumber(angle, 3, 0)+" deg");

				refrAngles.push_back(angle);
				
				progress = angle/180;
				if(_init_status.get("pca").isInit()) {
					for(size_t motorID=0; motorID<6; motorID++) {
						anglesToSend[motorID] = GET_PRGRSS(presetAngles_start[motorIdx][motorID], presetAngles_end[motorIdx][motorID], progress);
					}
					int returCode_sendToServo = 0;
					if((returCode_sendToServo=sendToServo(&pca, anglesToSend, current_q, false, 0, 0, true))!=0) {
						throw std::runtime_error("ERROR: sendToServo failed with code: "+std::to_string(returCode_sendToServo));
					}
				}
				SHLEEP(100); //total movement will take 18 seconds
				
				try {
					orientObj.update(false);
					ANSI_mvprint(0, 5, "{"+formatNumber(orientObj.pitch+90, 6, 1)+","+formatNumber(orientObj.roll+90, 6, 1)+"}");
					readAngles.push_back(std::roundf(extractAngle()));
				}
				catch(const std::exception& e) {
					readAngles.push_back(angle);
				}

				if(int(angle)%10==0) {
					coeffs 		= HW_KINEMATICS::solveServoDriftRegression(refrAngles, readAngles, numPolynomials);
					coeffs_sol	= HW_KINEMATICS::solveServoDriftRegression(readAngles, refrAngles, numPolynomials);

					ANSI_mvprint(0, 7, "coeffs     : "+formatVector(coeffs    , 9, 4));
					ANSI_mvprint(0, 8, "coeffs_sol : "+formatVector(coeffs_sol, 9, 4));
				}
				
			}

			servoMotor_coeffs.at(motorIdx) = coeffs;
			servoMotor_coeffs_sol.at(motorIdx) = coeffs_sol;

			menu__calibrateMotor_main.addTextCell(formatVector(coeffs, 6, 2)	, 1, pressed_pos[1]);
			menu__calibrateMotor_main.addTextCell(formatVector(coeffs_sol, 6, 2), 2, pressed_pos[1]);
			
			motorsCalibrated = true;


			ANSI_mvprint(0, 10, "solved coeffs: "+formatVector(coeffs, 6, 1), true, "abs", "rel");
			
			// servoMotor_readAngleData.at(motorIdx)
			for(size_t i=0; i<readAngles.size(); i++) {
				servoMotor_readAngleData.at(motorIdx).at(i) = readAngles.at(i);
			}

			std::this_thread::sleep_for(std::chrono::seconds(2));
			for(size_t cnt=0; cnt<3; cnt++) {
				SHLEEP(1000);
				ANSI_mvprint(0, -1, ".", true, "rel", "abs");
			}
			
		}
	}

}


TUI::termMenu menu__calibrateAccel_main({
	{" Adjust accel board and select an axis to calibrate", 0, 0, TUI::CELLOPTSPEC_TEXT},
	
	{"reset", 0, 2, -1},

	{" X_neg: "	, 0, 4, -1}, {" X_pos: ", 2, 4, -1},
	{" Y_neg: "	, 0, 5, -1}, {" Y_pos: ", 2, 5, -1},
	{" Z_neg: "	, 0, 6, -1}, {" Z_pos: ", 2, 6, -1},

	{" apply offset", 0, 8, -1},

	{"back"	, 0, 10, 27}
}, false);

void _updateFunc_opt4_accelOffset(TUI::termMenu* ptr_menu) {
	static std::string prevStr = "";
	assert(_init_status.get("orientObj").isInit());
	std::string textCell_text;
	try {
		orientObj.update(false);
		ptr_menu->addTextCell(formatContainer1(orientObj.accel, 3, 5, 2), 2, 0);
	}
	catch(const std::exception& e) {
		ptr_menu->addTextCell(e.what(), 2, 0);
	}
	
	if(prevStr!=textCell_text) {
		prevStr = textCell_text;
	}
	ptr_menu->updateTable();
}

void HW_option4_accelOffsets() {
	if(!_init_status.get("orientObj").isInit()) {
		ANSI_mvprint(0, 0, "ERROR: orientObj has not been initialised (required): "+_init_status.get("orientObj").get_callMsg(), true, "abs", "rel");
		for(size_t _=0; _<3; _++) {
			ANSI_mvprint(0, -1, " .", true, "rel", "rel");
			SHLEEP(1000);
		}
		return;
	}

	size_t numValuesToMeasure = 100;
	std::vector<pos2d<float>> endValues(3, pos2d<float>(-1, 1));

	bool runMain = true;
	while(runMain) {
		for(size_t axis=0; axis<3; axis++) {
			for(size_t limID=0; limID<2; limID++) {
				menu__calibrateAccel_main.addTextCell(formatNumber(endValues.at(axis)[limID], 4, 2), limID*2+1, axis+4);
			}
		}
		
		try {
			pos2d<int> pressed_pos = menu__calibrateAccel_main.driver(1, 3, 5, true, _updateFunc_opt4_accelOffset, true);
			
			if(pressed_pos==pos2d<int>{0, 2}) endValues = std::vector<pos2d<float>>(3, pos2d<float>(-1, 1));
			else if(pressed_pos==pos2d<int>{0, 8}) {
				orientObj.accel_offs[0] = endValues.at(0)[1] - std::abs(endValues.at(0)[0]);
				orientObj.accel_offs[1] = endValues.at(1)[1] - std::abs(endValues.at(1)[0]);
				orientObj.accel_offs[2] = endValues.at(2)[1] - std::abs(endValues.at(2)[0]);
			}
			else if(pressed_pos==pos2d<int>{0, 10}) break;
			else if(pressed_pos.inRegion({0, 4}, {2, 6})) {
				/**
				 * The accelerometer axis value to find the max(abs()) value of.
				 *  - [0]: min/max
				 *  - [1]: x/y/z
				 * 
				 */
				pos2d<int> valIdx = {pressed_pos[0]/2, (pressed_pos[1]-4)};
				float furthestVal = 0;
				for(size_t i=0; i<numValuesToMeasure; i++) {
					try {
						orientObj.update(false);
					}
					catch(const std::exception& e) {
						continue;
					}
					float val = std::abs(orientObj.accel[valIdx[1]]);
					if(val>furthestVal) {
						furthestVal = val;
						endValues.at(valIdx[1])[valIdx[0]] = orientObj.accel[valIdx[1]];
					}
				}
	
	
			}
		}
		catch(const std::exception& e) {
			std::cout << e.what() << '\n';
			SHLEEP(1000);
			exit(1);
		}
		
	}

}


