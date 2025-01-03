
#include "IR_camTrack.hpp"


IR_camTracking::IR_camTracking(
    int camIndex,
    int prefWidth,
    int prefHeight,
    bool setAutoBright,
    bool useWindow,
    bool readDelays,
    const char* windowName
): cap(camIndex), perfObj("["+to_string(camIndex)+"]") {
    if(!cap.isOpened()) {
        throw std::logic_error(("IR_camTracking: cannot open webcam with idx:"+std::to_string(camIndex)).c_str());
        return;
    }

    win_name = windowName;
    camIdx = camIndex;
    prefSize[0] = prefWidth;
    prefSize[1] = prefHeight;

    useAutoBrightness = setAutoBright;
    displayToWindow = useWindow;
    takePerformance = readDelays;

    if(!useAutoBrightness) cap.set(cv::CAP_PROP_AUTO_EXPOSURE, 1);
    if(displayToWindow) {
        cv::namedWindow(win_name, 0);
        // createTrackbars(win_name);
        cv::resizeWindow(win_name, 1280, 960);
    }
}
IR_camTracking::IR_camTracking(
    int camIndex,
    const char* windowName,
    int prefWidth,
    int prefHeight
): cap(camIndex), perfObj("["+to_string(camIndex)+"]") {
    if(!cap.isOpened()) {
        throw std::logic_error(("IR_camTracking: cannot open webcam with idx:"+std::to_string(camIndex)).c_str());
        return;
    }
    const char* win_name = windowName;
    camIdx = camIndex;
    prefSize[0] = prefWidth;
    prefSize[1] = prefHeight;
    displayToWindow = false;
}


void IR_camTracking::setup_window(
    const char* windowName,
    int width_resize,
    int height_resize
) {
    win_name = windowName;
    displayToWindow = true;
    if(displayToWindow) {
        cv::namedWindow(win_name, 0);
        // createTrackbars(win_name);
        cv::resizeWindow(win_name, width_resize, height_resize);
    }
}

/// @brief Create trackbar
/// @param win_name pointer to opencv window name
void IR_camTracking::createTrackbars(const char* win_name) {
	cv::createTrackbar("LowH", win_name, &l_HSV[0], 179);
	cv::createTrackbar("HighH", win_name, &u_HSV[0], 179);
	cv::createTrackbar("LowS", win_name, &l_HSV[1], 255);
	cv::createTrackbar("HighS", win_name, &u_HSV[1], 255);
	cv::createTrackbar("LowV", win_name, &l_HSV[2], 255);
	cv::createTrackbar("HighV", win_name, &u_HSV[2], 255);
}

/// @brief set trackbar pos/value
/// @param win_name pointer to opencv window name
void IR_camTracking::updateTrackbarPos(const char* win_name) {
	cv::setTrackbarPos("LowH", win_name, l_HSV[0]);
	cv::setTrackbarPos("LowS", win_name, l_HSV[1]);
	cv::setTrackbarPos("LowV", win_name, l_HSV[2]);
	cv::setTrackbarPos("HighH", win_name, u_HSV[0]);
	cv::setTrackbarPos("HighS", win_name, u_HSV[1]);
	cv::setTrackbarPos("HighV", win_name, u_HSV[2]);
}

/// @brief release cams videocapture object
void IR_camTracking::close() {
    cap.release();
}

/// @brief read from Cam, process raw Mat img and get camPos[2]
/// @return 0 if successful; -1 if errors occured
int IR_camTracking::processCam() {
    bool test = (cap.read(imgRaw));
    if(!test) {
        printf("error: Cannot read frame from webcam[%d]",camIdx);
        cv::destroyAllWindows();
        processReturnCode = -1;
        return -1;
    }
    processReturnCode = 0;
    if(takePerformance) perfObj.add_checkpoint("["+to_string(camIdx)+"]"+"read"); /*c1*/

    cv::resize(imgRaw, imgOriginal, cv::Size(prefSize[0],prefSize[1]), cv::INTER_LINEAR);
    if(takePerformance) perfObj.add_checkpoint("["+to_string(camIdx)+"]"+"resize"); /*c2*/

    // cv::flip(imgOriginal, imgFlipped, 1); //temporarily disabled
    imgFlipped = imgOriginal;
    cv::cvtColor(imgFlipped, imgHSV, cv::COLOR_BGR2HSV);
    if(takePerformance) perfObj.add_checkpoint("["+to_string(camIdx)+"]"+"cvtC"); /*c3*/

    cv::inRange(
        imgHSV,
        cv::Scalar(l_HSV[0], l_HSV[1], l_HSV[2]),
        cv::Scalar(u_HSV[0], u_HSV[1], u_HSV[2]),
        imgThreshold
    );
    if(takePerformance) perfObj.add_checkpoint("["+to_string(camIdx)+"]"+"inRan"); /*c4*/

    cv::erode(imgThreshold, imgThreshold, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5)), cv::Point(-1, -1), 1);
    if(takePerformance) perfObj.add_checkpoint("["+to_string(camIdx)+"]"+"erode"); /*c5*/
    cv::dilate(imgThreshold, imgThreshold, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5)), cv::Point(-1, -1), 6); 
    if(takePerformance) perfObj.add_checkpoint("["+to_string(camIdx)+"]"+"dilate"); /*c6*/

    cv::Moments imgMoments = cv::moments(imgThreshold);
    double dM01 = imgMoments.m01;
    double dM10 = imgMoments.m10;
    double dArea = imgMoments.m00;
    cv::findContours(imgThreshold, contours, hierarchy, cv::RETR_TREE,cv::CHAIN_APPROX_NONE);
    dArea = 0;
    totCnt_area = 0;
    allCnt_pos.clear();
    for(unsigned int i=0; i<contours.size(); i++) {
        dArea = cv::contourArea(contours[i]);
        if(dArea >= areaLim) {
            cv::RotatedRect minRect = cv::minAreaRect(cv::Mat(contours[i]));            
            vector<float> temp;
            temp.push_back(contours[i][0].x); //+minRect.size.width/2); //not sure if minRect addition is needed. Just saw some issue a long time ago during my insomnic season
            temp.push_back(contours[i][0].y+minRect.size.height/2);
            allCnt_pos.push_back(temp);
            totCnt_area += dArea;
        }
    }
    if(takePerformance) perfObj.add_checkpoint("["+to_string(camIdx)+"]"+"cntArea"); /*c7*/
    if(allCnt_pos.size() > 0) {

        getAvg_cntPos();
        if(displayToWindow) {
            cv::circle(imgFlipped,cv::Point(totCnt_pos[0],totCnt_pos[1]),50,cv::Scalar(0,0,0),2);
            cv::putText(
                imgFlipped, "["+to_string(int(totCnt_pos[0]))+","+to_string(int(totCnt_pos[1]))+"]",
                cv::Point(totCnt_pos[0],totCnt_pos[1]),cv::FONT_HERSHEY_SIMPLEX,1,cv::Scalar(0,0,0),2,false
            );
        }
    }

    if(takePerformance) perfObj.add_checkpoint("["+to_string(camIdx)+"]"+"avgPos"); /*c8*/
    if(displayToWindow) {
        cv::cvtColor(imgThreshold, imgThreshold, cv::COLOR_GRAY2BGR);
        if(takePerformance) perfObj.add_checkpoint("["+to_string(camIdx)+"]"+"cvtCol"); /*c9*/
        cv::vconcat(imgFlipped, imgThreshold, imgFlipped);
        if(takePerformance) perfObj.add_checkpoint("["+to_string(camIdx)+"]"+"vconc"); /*c10*/
    }
    if(takePerformance) perfObj.update_totalInfo(true, printAll, printAll, ' ','\r');

    return 0;
}


/// @brief Get avg xy coordinates from every contour
void IR_camTracking::getAvg_cntPos() {
	float xTot=0, yTot = 0;
    for(auto vec: allCnt_pos) {
        xTot += vec.at(0);
        yTot += vec.at(1);
    }
	// totCnt_pos[0] = float(prefSize[0]) - xTot / allCnt_pos.size();
	// totCnt_pos[1] = float(prefSize[1]) - yTot / allCnt_pos.size();
	totCnt_pos[0] = xTot / allCnt_pos.size();
	totCnt_pos[1] = yTot / allCnt_pos.size();
}


/// @brief read hsv settings from a file and store them in array pointer
/// @param camObject `IR_camTracking` 
/// @param argHSV 
/// @param window_name 
/// @param indeks 
/// @param filePath 
/// @param displayWin 
void hsv_settingsRead(
    std::vector<IR_camTracking>& camObject,
    int argHSV[2][3],
    const char* window_name,
    int indeks,
    string filePath,
    bool displayWin
) {
	//read HSV values from file with given indeks and change global l_HSV/u_HSV variables
	ifstream hsvFile(filePath);
	if(!hsvFile.is_open()) {
		printf("can't open file \"%s\"",filePath);
		return;
	}
	string line;
	int tempVal[6];
	while(getline(hsvFile,line)) {
		if(line.substr(0,1)==to_string(indeks)) {
			line.erase(0,3);
			//lower HSV
			tempVal[0]=stoi(line.substr(0,line.find(',')));
			line.erase(0,line.find(',')+1);
			tempVal[1]=stoi(line.substr(0,line.find(',')));
			line.erase(0,line.find(',')+1);
			tempVal[2]=stoi(line.substr(0,line.find(':')));
			line.erase(0,line.find(':')+1);
			//upper HSV
			tempVal[3]=stoi(line.substr(0,line.find(',')));
			line.erase(0,line.find(',')+1);
			tempVal[4]=stoi(line.substr(0,line.find(',')));
			line.erase(0,line.find(',')+1);
			tempVal[5]=stoi(line.substr(0,line.find(']')));

			for(int i=0; i<6; i++) { printf(" %d", tempVal[i]); }
			argHSV[0][0] = tempVal[0];
			argHSV[0][1] = tempVal[1];
			argHSV[0][2] = tempVal[2];
			argHSV[1][0] = tempVal[3];
			argHSV[1][1] = tempVal[4];
			argHSV[1][2] = tempVal[5];
			if(displayWin) camObject.at(0).updateTrackbarPos(window_name);
			hsvFile.close();
			return;
		}
	}
	printf("index not found\n");
}

void hsv_settingsWrite(
    int argHSV[2][3],
    int indeks,
    bool overWrite,
    string filePath
) {
	//write global HSV values from l_HSV and u_HSV into the file
	//overwrite:	if indeks already exists the new one will be written over the old one if overWrite is true,
	//				otherwise it'll add the values at the end of the files
	ifstream rFile(filePath);
	if(!rFile.is_open()) {
		printf("error: Cannot open file\"");
		std::cout << filePath << "\"\n";
	}
	string fileContents="", line;
	// printf("rowLen_1: %d\n", rowLen);
	while(getline(rFile, line)) {
		fileContents+=line;
		fileContents+="\n";
	} //ex: "row1\nrow2\nrow3"
	rFile.close();
	ofstream wFile(filePath);
	int rowLen_2 = count(fileContents.begin(),fileContents.end(),'\n');
	// printf("total rows:%d\n",rowLen_2);
	if(!overWrite) {rowLen_2++;}
	else {}
	string fileRows[rowLen_2];
	for(int i=0; i<rowLen_2; i++) {
		// printf("row %d out of %d rows: ",i,rowLen_2);
		// std::cout << fileContents.substr(0,fileContents.find('\n')+1) << std::endl;
		if(i==rowLen_2-1 && !overWrite) {
			fileRows[i] = to_string(i-2)+";["+
			to_string(argHSV[0][0])+","+to_string(argHSV[0][1])+","+to_string(argHSV[0][2])+":"+
			to_string(argHSV[1][0])+","+to_string(argHSV[1][1])+","+to_string(argHSV[1][2])+"]\n";
		}
		else {
			fileRows[i] = fileContents.substr(0,fileContents.find('\n')+1);
			fileContents.erase(0,fileContents.find('\n')+1);
			if(fileRows[i].substr(0,1)==to_string(indeks-1) && overWrite) {
				i++;
				fileRows[i] = to_string(i)+";["+
				to_string(argHSV[0][0])+","+to_string(argHSV[0][1])+","+to_string(argHSV[0][2])+":"+
				to_string(argHSV[1][0])+","+to_string(argHSV[1][1])+","+to_string(argHSV[1][2])+"]\n";
				fileContents.erase(0,fileContents.find('\n')+1);
			}
		}
	}
	string totText = "";
	for(int i=0; i<rowLen_2; i++) {
		// printf("fileRows[%d]: \"", i);
		// std::cout << fileRows[i] << "\"" << std::endl;
		totText+=fileRows[i];
		}
	wFile << totText;
	printf("File written\n");
	wFile.close();
}

