#pragma once


void colorTracking(int serial_port) {
	VideoCapture cap(0); //capture the video from webcam
	int vidWidth = 640, vidHeight = 480;
	cap.set(CAP_PROP_FRAME_HEIGHT, vidHeight);
	cap.set(CAP_PROP_FRAME_WIDTH, vidWidth);
	if (!cap.isOpened())  // if not success, exit program
	{
		cout << "Cannot open the web cam" << endl;
		return;
	}
	namedWindow("Control", WINDOW_AUTOSIZE); //create a window called "Control"
	namedWindow("Original", WINDOW_AUTOSIZE);
	namedWindow("Threshold", WINDOW_AUTOSIZE);
	moveWindow("Control", 0, 0);
	moveWindow("Original", 1920, 100);
	moveWindow("Threshold", 1920, 100 + vidHeight);
	/*int iLowH = 170;
	int iHighH = 179;

	int iLowS = 150;
	int iHighS = 255;

	int iLowV = 60;
	int iHighV = 255;*/

	int TrackBarToggle = 2;
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
	float X = 0, Y = 0, Z = 0;
	float filterVal = 0.7;
	float coordScalar = 0.1; //Lower means faster response rate

	int fakePosX, fakePosY;

	//Capture a temporary image from the camera
	Mat imgTmp;
	cap.read(imgTmp);

	//Create a black image with the size as the camera output
	Mat imgLines = Mat::zeros(imgTmp.size(), CV_8UC3);
	Mat imgTrackSquare = Mat::zeros(imgTmp.size(), CV_8UC3);

    string toSend, input;
    string text1, text2, text3; //x, y and z values stored in string

    float fps = 10;
    clock_t start = clock();

	while (true) {
		Mat imgOriginal;
		trackerScaledouble = 0.00001;
		if (TrackerScaling > 0) { trackerScaledouble = (TrackerScaling) * trackerScaledouble; }
		else { trackerScaledouble = 0.1 * trackerScaledouble; }
		bool bSuccess = cap.read(imgOriginal); // read a new frame from video
		if (!bSuccess) //if not success, break loop
		{
			cout << "Cannot read a frame from video stream" << endl;
			break;
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
		// if the area <= 10000, I consider that the there are no object in the image and it's because of the noise, the area is not zero
		int areaSize = 8000;
		if (dArea > areaSize)
		{
			//cout << dArea << endl;
			//calculate the position of the ball
			int posX = dM10 / dArea;
			int posY = dM01 / dArea;
			if (TrackBarToggle == 0) {
				imgLines = Mat::zeros(imgTmp.size(), CV_8UC3);
				imgTrackSquare = Mat::zeros(imgTmp.size(), CV_8UC3);
			}
			else if (TrackBarToggle == 1) {
				if (iLastX >= 0 && iLastY >= 0 && posX >= 0 && posY >= 0)
				{
					/*Draw a red line from the previous point to the current point*/
					line(imgLines, Point(posX, posY), Point(iLastX, iLastY), Scalar(0, 0, 255), 2);
					imgOriginal = imgOriginal + imgLines;
				}
			}
			else {
				TrackingSquare = trackerScaledouble * dArea;
				//cout << TrackingSquare << endl;
				//cout << trackerScaledouble << endl;
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
			}
			iLastX = posX;
			iLastY = posY;
			X = filterVal*X + (1-filterVal)*iLastX;
			Y = filterVal*Y + (1-filterVal)*iLastY;
			Z = filterVal*Z + (1-filterVal)*(300 - int(round(TrackingSquare * coordScalar)));
		}
		//imshow("HSV image", imgHSV); //show hsv image (basic before fixing)
		/*Mat flippedImage;
		flip(imgOriginal, flippedImage, 1);*/
		text1 = to_string(int(round(coordScalar * float(float(vidWidth)/2 - X))));
		text2 = to_string(int(round(coordScalar * float(vidHeight - Y))));
		text3 = to_string(Z);
		/*string textToDisplay = "Coords to the center: x:" + text1 + " y:" + text2 + " z:" + text3;
		putText(flippedImage, textToDisplay, Point(0,15), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 255, 255), 1);
		putText(flippedImage, to_string(fps), Point(vidWidth - 50, 15), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 255, 255), 1);
		circle(flippedImage, Point(float(vidWidth)/2, vidHeight), float(50)/coordScalar, Scalar(255, 255, 255), 1);
		circle(flippedImage, Point(float(vidWidth)/2, vidHeight), float(100)/coordScalar, Scalar(255, 255, 255), 1);
		circle(flippedImage, Point(float(vidWidth)/2, vidHeight), float(150)/coordScalar, Scalar(255, 255, 255), 1);
		circle(flippedImage, Point(float(vidWidth)/2, vidHeight), float(200)/coordScalar, Scalar(255, 255, 255), 1);
		flip(imgThresholded, imgThresholded, 1);
		imshow("Threshold", imgThresholded); //show the thresholded image
		imshow("Original", flippedImage); //show the original image*/
		printf("fps: %f\n", fps);
		if (waitKey(30) == 27) //wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
		{
			cout << "esc key is pressed by user" << endl;
			destroyAllWindows();
			break;
		}
		//send the coords to port
		toSend = text1 + ":" + text2 + ":" + text3 + "\n";
		unsigned char* msg = (unsigned char*)toSend.c_str();
		printf("msg size: %d bytes\n", toSend.length());
        write(serial_port, msg, toSend.length());
        char read_buf [256];
        memset(&read_buf, '\0', sizeof(read_buf));
        int num_bytes = read(serial_port, &read_buf, sizeof(read_buf));
        if (num_bytes < 0) {
            printf("Error reading: %s", strerror(errno));
            close(serial_port);
            return;
        }
        printf("---------------\n");
        printf("Read %i bytes. Received message: %s\n", num_bytes, read_buf);
        clock_t finish = clock();
        fps = 1/(double(finish-start) / CLOCKS_PER_SEC);
        start = clock();
	}
	close(serial_port);
}
