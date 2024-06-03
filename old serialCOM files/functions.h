#pragma once

float toDegrees(float radians) {
	return (radians * 180) / PI;
}

float toRadians(float degrees) {
	return (degrees * PI) / 180;
}


bool aIsBigger(float a, float b) {
	bool ans = false;
	if (a > b) { ans = true; }
	else { ans = false; }
	return ans;
}


void updateArduino_output(
	Vector3f* coordChange,
	Vector2f* tiltChange,
	bool printText = true
) {
	int n = 0;
	string toConvert = "";
	
	while (output[n] != 'r') {
		n++;
		if (n >= 255) {
			return;
		}
	}
	while (output[n + 1] != ':') {
		n++;
		toConvert += output[n];
	}
	n++;
	(*tiltChange).y = stof(toConvert);
	toConvert = "";
	if (printText) {
		printf("roll: %f", (*tiltChange).y);
	}
	while (output[n] != 'p') {
		n++;
		if (n >= 255) {
			return;
		}
	}
	while (output[n + 1] != ':') {
		n++;
		toConvert += output[n];
	}
	n++;
	(*tiltChange).x = stof(toConvert);
	toConvert = "";
	if (printText) {
		printf(" pitch: %f", (*tiltChange).x);
	}
	while (output[n] != 'x') {
		n++;
		if (n >= 255) {
			return;
		}
	}
	if (printText) {
		printf(" moving x:");
	}
	while (output[n + 1] != ':') {
		n++;
		toConvert += output[n];
	}
	n++;
	(*coordChange).x = stof(toConvert);
	if (printText) {
		printf("%f", (*coordChange).x);
	}
	toConvert = "";

	while (output[n] != 'y') {
		n++;
		if (n >= 255) {
			return;
		}
	}
	if (printText) {
		printf(" moving y:");
	}
	while (output[n + 1] != ':') {
		n++;
		toConvert += output[n];
	}
	n++;
	(*coordChange).y = stof(toConvert);
	if (printText) {
		printf("%f", (*coordChange).y);
	}
	toConvert = "";

	while (output[n] != 'z') {
		n++;
		if (n >= 255) {
			return;
		}
	}
	if (printText) {
		printf(" moving z:");
	}
	while (output[n + 1] != ':') {
		n++;
		toConvert += output[n];
	}
	n++;
	(*coordChange).z = stof(toConvert);
	if (printText) {
		printf("%f", (*coordChange).z);

		printf(" n:%f\n", n);
	}

}

void correctAngleValue(float* angle, float defaultVal) {
	if (toDegrees(*angle) + defaultVal > 180)
		(*angle) = PI - toRadians(defaultVal);
	else if (toDegrees(*angle) + defaultVal < 0)
		(*angle) = 0 - toRadians(defaultVal);
} //angle value in radians, defaultVal in degrees

int ifPositiveOrNegative(float input) {
	int ans;
	if (input / abs(input) > 0) { ans = 1; }
	if (input / abs(input) < 0) { ans = -1; }
	return ans;
}


Vector3f squarebox2_angles(int n) {
	Vector3f angles = Vector3f(0, 0, 0); //in a, b and Y
	switch (n) {
	case 0:
		angles = Vector3f(toRadians(-45), toRadians(-45), toRadians(0));
		break;
	case 1:
		angles = Vector3f(toRadians(-45), toRadians(-45), toRadians(0));
		break;
	case 2:
		angles = Vector3f(toRadians(45), toRadians(-45), toRadians(0));
		break;
	case 3:
		angles = Vector3f(toRadians(45), toRadians(-45), toRadians(0));
		break;
	case 4:
		angles = Vector3f(toRadians(45), toRadians(-45), toRadians(0));
		break;
	case 5:
		angles = Vector3f(toRadians(-45), toRadians(-45), toRadians(0));
		break;
	case 6:
		angles = Vector3f(toRadians(-45), toRadians(-45), toRadians(0));
		break;
	case 7:
		angles = Vector3f(toRadians(45), toRadians(-45), toRadians(0));
		break;
	case 8:
		angles = Vector3f(toRadians(45), toRadians(-45), toRadians(0));
		break;
	default:
		printf("incorrect box paramenter has been given\n");
	}
	return angles;
} //note: this has 9 angles max, and returns in radians



void opencv_trackColor(
	Vector3f* pos,
	VideoCapture* cap,
	bool* foundPos,
	bool displayWindow = true,
	int iLowH = 0,
	int iHighH = 255,
	int iLowS = 0,
	int iHighS = 255,
	int iLowV = 0,
	int iHighV = 255,
	float camScalar = 1
) {
	int trackLimit = 20000;//min area to start tracking in
	if (!(*cap).isOpened()) { cout << "Cannot open the web cam" << endl; return; }
	int TrackBarToggle = 0;
	int TrackingSquare = 100;
	int TrackerScaling = 5;
	int Y_offVal = 50;
	int X_offVal = 50;
	double trackerScaledouble = 0.0001;
	int iLastX = -1;
	int iLastY = -1;
	int fakePosX, fakePosY;
	//Capture a temporary image from the camera
	Mat imgTmp;
	(*cap).read(imgTmp);
	//Create a black image with the size as the camera output
	Mat imgLines = Mat::zeros(imgTmp.size(), CV_8UC3);
	Mat imgTrackSquare = Mat::zeros(imgTmp.size(), CV_8UC3);
	Mat imgGrid = Mat::zeros(imgTmp.size(), CV_8UC3); //the grid on the background of the displayed image
	Vector2f camRes = Vector2f(640, 480); //the resolution of the camera output in xy
	Vector3f posFromImage = Vector3f(0, 0, 140); //the positions from the camera needs to reverted
	float zVal_default = 50;
	Vector3f coordScalar = Vector3f(10, 2, 1);

	for (int i = 0; i < 10; i++) {
		Mat imgOriginal;
		trackerScaledouble = 0.00001;
		if (TrackerScaling > 0) { trackerScaledouble = (TrackerScaling)*trackerScaledouble; }
		else { trackerScaledouble = 0.1 * trackerScaledouble; }
		bool bSuccess = (*cap).read(imgOriginal); // read a new frame from video
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
		circle(imgGrid, Point(320, 480), 50 / graphScalar, Scalar(255, 160, 100), 2);
		imgOriginal = imgOriginal + imgGrid;
		circle(imgGrid, Point(320, 480), 100 / graphScalar, Scalar(255, 160, 100), 2);
		imgOriginal = imgOriginal + imgGrid;
		circle(imgGrid, Point(320, 480), 150 / graphScalar, Scalar(255, 160, 100), 2);
		imgOriginal = imgOriginal + imgGrid;
		if (dArea > trackLimit) { //creating track square and setting the coordinates for PP
			int posX = dM10 / dArea;
			int posY = dM01 / dArea;
			TrackingSquare = trackerScaledouble * dArea;
			posFromImage.z = zVal_default + 1.5 * (100 - TrackingSquare);
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
				(camRes.x / 2 - iLastX) * graphScalar * camScalar/*this part sets x=0 as the center, meaning x ranges from [-320:320]*/,
				(camRes.y - iLastY) * graphScalar * camScalar,
				posFromImage.z
			);
		}
		////////
		Mat flippedThreshold;
		flip(imgThresholded, flippedThreshold, 1);
		Mat flippedImage;
		flip(imgOriginal, flippedImage, 1);
		string text1, text2;
		text1 = to_string(posFromImage.x);
		text2 = to_string(posFromImage.y);
		string textToDisplay = "Coords to the center: x:" + text1 + " y:" + text2 + " area:" + to_string(int(round(dArea))) + " z-value:" + to_string(posFromImage.z);
		putText(flippedImage, textToDisplay, Point(0, 15), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 255, 255), 1);
		if (displayWindow) {
			imshow("threshold image", flippedThreshold);
			imshow("rrreeeeeEEEEEEEEEEEEEEEEE-", flippedImage); //show the flipped image
			if (waitKey(30) == 27) {} //wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
		}
		if (dArea < trackLimit) {
			cout << "\n--can't find the color to detect\n";
			(*foundPos) = false;
		}
		else {
			(*foundPos) = true;
			break;
		}
		Sleep(50);
	}

	(*pos) = posFromImage;
}

//the two functions that are communicating between eachother on different threads
bool readyToReceiveCoords = false;

void robotArm_OpencvFunc() {
	//variable declarations


	VideoCapture TempCap(1); //capture the video from web cam
	if (!TempCap.isOpened()) {// if not success, exit program
		cout << "Cannot open the web cam" << endl;
		return;
	}
	//

	namedWindow("Control", WINDOW_AUTOSIZE); //create a window called "Control"
	int iLowH = 37;
	int iHighH = 109;
	int iLowS = 63;
	int iHighS = 255;
	int iLowV = 12;
	int iHighV = 137;

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

	Vector2f camRes = Vector2f(640, 480); //the resolution of the camera output in xy
	Vector3f posFromImage = Vector3f(0, 0, 140); //the positions from the camera needs to reverted
	Vector3f posFromImage_filter = posFromImage;
	float zVal_default = 0;
	Vector3f coordScalar = Vector3f(10, 2, 1);



	while (true) {
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
		if (dArea > trackLimit) {
			int posX = dM10 / dArea;
			int posY = dM01 / dArea;
			TrackingSquare = trackerScaledouble * dArea;
			posFromImage.z = zVal_default + 0.25 * (int(1000) - TrackingSquare);
			if (iLastX >= 0 && iLastY >= 0 && posX >= 0 && posY >= 0) {
				imgLines = Mat::zeros(imgTmp.size(), CV_8UC3);
				imgTrackSquare = Mat::zeros(imgTmp.size(), CV_8UC3);
				fakePosY = posY + (Y_offVal - 50);
				fakePosX = posX + (X_offVal - 50);
				line(imgTrackSquare, Point(fakePosX - (TrackingSquare / 2), fakePosY + (TrackingSquare / 2)), Point(fakePosX + (TrackingSquare / 2), fakePosY + (TrackingSquare / 2)), Scalar(0, 0, 255), 1);
				imgOriginal = imgOriginal + imgTrackSquare;
				line(imgTrackSquare, Point(fakePosX - (TrackingSquare / 2), fakePosY + (TrackingSquare / 2)), Point(fakePosX - (TrackingSquare / 2), fakePosY - (TrackingSquare / 2)), Scalar(0, 0, 255), 1);
				imgOriginal = imgOriginal + imgTrackSquare;
				line(imgTrackSquare, Point(fakePosX - (TrackingSquare / 2), fakePosY - (TrackingSquare / 2)), Point(fakePosX + (TrackingSquare / 2), fakePosY - (TrackingSquare / 2)), Scalar(0, 0, 255), 1);
				imgOriginal = imgOriginal + imgTrackSquare;
				line(imgTrackSquare, Point(fakePosX + (TrackingSquare / 2), fakePosY + (TrackingSquare / 2)), Point(fakePosX + (TrackingSquare / 2), fakePosY - (TrackingSquare / 2)), Scalar(0, 0, 255), 1);
				imgOriginal = imgOriginal + imgTrackSquare;
			}
			iLastX = posX;
			iLastY = posY;
			//sets x and y for the end-effector pos
			posFromImage = Vector3f(
				0.8 * posFromImage.x + 0.2 * (camRes.x / 2 - iLastX)/*this part sets x=0 as the center, meaning x ranges from [-320:320]*/,
				0.8 * posFromImage.y + 0.2 * (camRes.y - iLastY),
				0.8 * posFromImage.z + 0.2 * posFromImage.z
			);
		}
		////////
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


	}
} ///thread 2