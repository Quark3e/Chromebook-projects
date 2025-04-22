
/*
 * Project Name: Hexclaw  
 * File: main.cpp
 * Description: Robot Arm Control project  
 * Author: Erkhme Byambadorj.
 * Created May 2023
 * Description: See URL for full details.
 * NOTE :: NOT FINISHED
 * URL: shit
 */


#include "includes.hpp"
#include "servo_control.hpp"
#include "global_variables.hpp"
#include "constants.hpp"

#if _MACHINE__RPI_MAIN
#include <pigpio.h>
#endif


/// @brief 0-direct function; 1-use 2d coefs; 2-use arificial; 3-use two_cam_triangle
int zSol=3;

// void initPaths() {
//     char result[PATH_MAX];
//     ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
//     const char *pathP;
//     if (count != -1) {
//         pathP = dirname(result);
//     }

//     char path[100];
//     std::string temp = strcat(strcpy(path, pathP), "/");
//     absPath = temp.substr(0, temp.find("/projects"))+"/teststuff/python/opencv/angleArea/data/csv_artif/";
// }



/// @brief 2d coefficients for a single layer
float angleArea_coef[181][181];
/// @brief 3d artifically pre-generated area results
float artifVal[181][181][401]; //x = [0, 90, 181] = [-90, 0, 01]

void load_csvFile(std::string filePath = "data/csv_dataSet_pf17_fuse-True.csv") {
    printf("Loading csv file:\n");
	std::ifstream csvFile(filePath);
	if(!csvFile.is_open()) {
		printf("error: Cannot open csv file");
		std::cout << filePath << "\"\n";
	}
	std::string line;
	std::getline(csvFile, line);
	while(std::getline(csvFile, line)) {
		int x, y;
		x = stof(line.substr(0, line.find(",")));
		line.erase(0, line.find(",")+1);
		y = stof(line.substr(0, line.find(",")));
		line.erase(0, line.find(",")+1);
        x = int(round(x+90));
        y = int(round(y+90));
        float saveVal = stof(line.substr(0, line.find(",")));
		angleArea_coef[x][y] = saveVal;
        printf("- [%d][%d]: %f\r", x, y, saveVal);
	}
	std::cout << std::endl;
	csvFile.close();

}


/// @brief solve z-axis/cam-distance from area
/// @param area [float]: cntArea
/// @param posX [float]: x coordinate of the object [NOTE: assuming center of screen=[0, 0]]
/// @param posY [float]: y coordinate
/// @return [float]: assumed z-axis value
float zAxisFunc(float area, float posX, float posY) {
	float val=0, ans = 0;

    float c[11] = {
		 1.41350147 * pow(10, -32),
		-7.39588055 * pow(10, -28),
		 1.70543946 * pow(10, -23),
		-2.27903757 * pow(10, -19),
		 1.95152915 * pow(10, -15),
		-1.11701396 * pow(10, -11),
		 4.32049796 * pow(10, -8),
		-1.11313493 * pow(10, -4),
		 1.82568080 * pow(10, -1),
		-1.72185711 * pow(10, 2),
		 7.15691211 * pow(10, 4)
			 };
	for(int i=0; i<11; i++) { val+=c[i]*pow(area, 10-i); }
	if(zSol==0) return val;
	/*
	[-x:x] = [alpha:-alpha]
	[-y:y] = [-beta:beta]
	*/
	// if(zSol==1) {
	// 	ans = val * (1 / angleArea_coef[
	// 			int(round(/*orient[0]*/-(0-posX*angPerPix)))+90
	// 		][
	// 			int(round(/*orient[1]+*/posY*angPerPix))+90
	// 		]);
	// 	return ans;
	// }
	
	if(zSol==3) {
		
	}

	int chosenIdx=0;
	chosenIdx = getClosestValIdx(artifVal[int(Roll)+90][int(Pitch)+90],401,int(area));
	// std::cout << chosenIdx << artifVal[int(Roll)+90][int(Pitch)+90][200]<< "\t";
	return chosenIdx;
}


void loadData_csvArtif(bool printVar=true) {
	if (printVar) std::cout << "Starting to load the data\n";

	for(int x=0; x<181; x++) {
		for(int y=0; y<181; y++) {
			for(int z=0; z<401; z++) {
				artifVal[x][y][z]=-1;
			}
		}
	}

	std::string filename;
	std::string filenom[2] = {"csv_[1,1,1]_6568781_p", "_artificial.csv"};

	int columns=4;
	int parts=4;
	char temp[16]="0123456789;,.- ";
	int rowCount=0;
	float tempArr[4];
	bool fullBreak;
	std::string line;


	for(int part=0; part<parts; part++) {
		std::cout << "- p" << part << std::endl;
		rowCount=0;
		std::fstream csvFile;

		filename = absPath+filenom[0]+std::to_string(part)+filenom[1];
		csvFile.open(filename, std::ios::in);
		if(!csvFile.is_open()) std::cout << "error: file \""<< filename <<"\" could not be opened" << std::endl;
		
		std::getline(csvFile, line);
		while(std::getline(csvFile, line)) {
			int idx=0;
			fullBreak=false;
			for(int n=0; n<100; n++) {
				for(int i=0; i<sizeof(temp)/sizeof(temp[0]); i++) {
					if(line[n] == temp[i]) break;
					else if(i>=sizeof(temp)/sizeof(temp[0])-1) {
						idx=n;
						fullBreak=true;
						break;
					}
				}
				if(fullBreak) break;
			}
			splitString(line.substr(0, idx), ",", tempArr, columns, false);
			if(printVar) {
				printf(
					"- p%d   %7d |%36s| x:%3d y:%3d z:%4d area:%0.4f        \r",
					part, rowCount, line.substr(0, idx).c_str(),
					int(tempArr[0]), int(tempArr[1]), int(tempArr[2]), tempArr[3]
				);
			}
			artifVal[int(tempArr[0])+90][int(tempArr[1])+90][int(tempArr[2])] = round(tempArr[3]);
			rowCount++;
			
		}
		csvFile.close();
	}
	if(printVar) std::cout << "\nFinished loading the data: Total rows:" << rowCount << std::endl;
	std::cout << artifVal[90][90][200] << std::endl;
	// std::cout << "paused:";
	// std::string inp;
	// cin >> inp;
	// cin.ignore();
	// cin.clear();
}


#include <hw_options.hpp>


int main(int argc, char* argv[]) {

	std::cout << "PROGRAM START" << std::endl;
	std::cout << " path: _PROGRAM_PATH		= \"" << _XSTRINGLIT(_PROGRAM_PATH) 		<< "\"" << std::endl;
	std::cout << " path: _PROGRAM_PATH__EXE	= \"" << _XSTRINGLIT(_PROGRAM_PATH__EXE) 	<< "\"" << std::endl;
	std::cout << " path: _PROGRAM_PATH__DATA= \"" << _XSTRINGLIT(_PROGRAM_PATH__DATA)	<< "\"" << std::endl;
	// SHLEEP(2'000);
	// std::cout<<"\x1B[2J"<<std::endl;

	int termSize[2] = {0, 0};
	getTermSize(termSize[0], termSize[1]);

	// pca.init();

	// startMenu.addOpt("[1]   Intro    ", 0, 0, '1', HW_option1_intro);
	// startMenu.addOpt("[2]   Calibrate", 0, 1, '2', HW_option2);
	// startMenu.addOpt("[3]   Main     ", 0, 2, '3', HW_option0);
	// startMenu.addOpt("[t]   Terminal ", 0, 3, 't', HW_option6_terminal);
	// startMenu.addOpt("[m]   mode:    ", 0, 4, 'm', subMenu_mode);
	// startMenu.addOpt("[esc] Exit     ", 0, 5, 27, TUI::bool_true, &startMenu.exitDriver);

	// startMenu.setButtonWidth(10);

	//pca9685 board setup
	// pca.init();	
	// pca.set_pwm_freq(50.0);

	
	// std::cout << "simplified init called..."<<std::endl;
	// usleep(5'000'000);

	cv::utils::logging::setLogLevel(cv::utils::logging::LogLevel::LOG_LEVEL_SILENT);
	

	if(_init_status.get("pca").isInit()) sendToServo(&pca, new_q, current_q, true);
	HW_setup_options();

	if(argc==2) {
		if(hexclaw_cmdArgs.call_func(argv[1])==1) {
			std::cout << "ERROR: no matching flag or argument input"<<std::endl;
			return 1;
		}
	}
	else {
		HW_group__main();
		// std::cout<<"\x1B[2J"<<std::endl;
		// startMenu.driver(1, 1, 5, false);
	}

	if(hardExit) {
		return 0;
	}

	if(menu_group__main.exitDriver) {
		std::cout << "\x1B[H"<<"\x1B[2J";
		std::cout.flush();
		return 0;
	}
	
	
	for(int n=0; n<6; n++) new_q[n] = startup_q[n];
	std::cout << "\n- section: \"closing\"\n";

	if(_init_status.get("pca").isInit()) {
		sendToServo(&pca, new_q, current_q, false, 2, 2);
	}
	if(_init_status.get("pigpio").isInit()) {
		#if _MACHINE__RPI_MAIN
		gpioWrite(pin_ledRelay, 0);
		// gpioTerminate();
		#endif
	}
	
	std::cout << "\x1B[H";
	std::cout.flush();
	return 0;
}
