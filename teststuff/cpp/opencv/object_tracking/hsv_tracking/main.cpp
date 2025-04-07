
#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"


#include <iostream>
#include <vector>

#include "useful/pos2d.hpp"


int main(int argc, char** argv) {
    int capIdx = 0; // Default camera index
    if(argc==2) {
        try {
            capIdx = std::stoi(argv[1]); // Convert argument to integer
        }
        catch(const std::exception& e) {
            std::cerr << e.what() << '\n';
            return -1;
        }
        
    }
    
    cv::VideoCapture cap(capIdx); // Open the default camera (0)
    if (!cap.isOpened()) { // Check if camera opened successfully
        std::cerr << "Error: Could not open camera." << std::endl;
        return -1;
    }
    
    
    std::vector<int> lowerHSV = {0, 100, 100}; // Lower bound for HSV thresholding
    std::vector<int> upperHSV = {10, 255, 255}; // Upper bound for HSV thresholding

    float areaLim = 1000; // Minimum area for contour detection


    cv::Mat frame, hsvFrame, thresholdFrame;
    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;

    pos2d<float> pos(0, 0); // Position of the detected object
    float totalArea = 0; // Total area of the detected object
    float avgArea = 0; // Average area of the detected object
    
    

    cv::namedWindow("Display Window", cv::WINDOW_AUTOSIZE);
    
    cv::createTrackbar("Lower H", "Display Window", &lowerHSV[0], 179); // Trackbar for lower H value
    cv::createTrackbar("Lower S", "Display Window", &lowerHSV[1], 255); // Trackbar for lower S value
    cv::createTrackbar("Lower V", "Display Window", &lowerHSV[2], 255); // Trackbar for lower V value
    cv::createTrackbar("Upper H", "Display Window", &upperHSV[0], 179); // Trackbar for upper H value
    cv::createTrackbar("Upper S", "Display Window", &upperHSV[1], 255); // Trackbar for upper S value
    cv::createTrackbar("Upper V", "Display Window", &upperHSV[2], 255); // Trackbar for upper V value


    while(true) {
        cap >> frame; // Capture a new frame from camera
        if (frame.empty()) {
            std::cerr << "Error: Could not capture frame." << std::endl;
            break;
        }
        cv::flip(frame, frame, 1); // Flip the frame horizontally
        cv::cvtColor(frame, hsvFrame, cv::COLOR_BGR2HSV); // Convert to HSV color space
        
        cv::inRange(hsvFrame, cv::Scalar(lowerHSV[0], lowerHSV[1], lowerHSV[2]), 
                    cv::Scalar(upperHSV[0], upperHSV[1], upperHSV[2]), thresholdFrame); // Thresholding

        cv::dilate(thresholdFrame, thresholdFrame, cv::Mat(), cv::Point(-1, -1), 1); // Morphological dilation
        cv::erode(thresholdFrame, thresholdFrame, cv::Mat(), cv::Point(-1, -1), 5); // Morphological erosion

        cv::findContours(thresholdFrame, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE); // Find contours

        totalArea = 0; // Reset total area
        for (size_t i = 0; i < contours.size(); i++) {
            double area = cv::contourArea(contours[i]); // Calculate area of each contour
            if (area > areaLim) { // Filter small areas
                totalArea += area; // Accumulate total area
                cv::Moments m = cv::moments(contours[i]); // Calculate moments
                pos.x += static_cast<float>(m.m10 / m.m00); // Calculate x position
                pos.y += static_cast<float>(m.m01 / m.m00); // Calculate y position
            }
        }
        if (totalArea > 0) {
            pos.x /= static_cast<float>(contours.size()); // Average x position
            pos.y /= static_cast<float>(contours.size()); // Average y position
            avgArea = totalArea / static_cast<float>(contours.size()); // Average area
        }

        cv::Mat displayImg;

        cv::cvtColor(thresholdFrame, thresholdFrame, cv::COLOR_GRAY2BGR); // Convert thresholded image to BGR for display
        thresholdFrame.convertTo(displayImg, CV_8UC3); // Convert to 8-bit unsigned integer for display

        cv::drawContours(thresholdFrame, contours, -1, cv::Scalar(0, 255, 0), 2); // Draw contours
        cv::circle(thresholdFrame, cv::Point(static_cast<int>(pos.x), static_cast<int>(pos.y)), 5, cv::Scalar(0, 0, 255), -1); // Draw detected object position
        cv::putText(thresholdFrame, "Area: " + std::to_string(avgArea), cv::Point(10, 30), cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(255, 255, 255), 2); // Display area
        
        
        cv::vconcat(frame, thresholdFrame, displayImg); // Concatenate original and thresholded images vertically


        cv::imshow("Display Window", displayImg); // Show the original frame

        int keyInp = cv::waitKey(30); // Wait for 30 ms or until a key is pressed
        if (keyInp == 27) { // ESC key to exit
            break;
        }
    }

    cv::destroyAllWindows(); // Close all OpenCV windows
    cap.release(); // Release the camera

    return 0;
}
