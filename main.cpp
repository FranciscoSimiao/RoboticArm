#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

void detectAndDisplay(Mat &frame);

int main(int argc, char **argv)
{
    VideoCapture cap(0); // Open the default camera

    if (!cap.isOpened())
    {
        cerr << "Error: Could not open camera." << endl;
        return -1;
    }

    Mat frame;

    while (true)
    {
        cap >> frame; // Capture frame-by-frame

        if (frame.empty())
        {
            cerr << "Error: Could not capture frame." << endl;
            break;
        }

        detectAndDisplay(frame);

        if (waitKey(1) >= 0)
            break;
    }

    return 0;
}

void detectAndDisplay(Mat &frame)
{
    Mat hsvFrame;
    cvtColor(frame, hsvFrame, COLOR_BGR2HSV);

    // Define range of skin color in HSV
    Scalar lower_skin = Scalar(0, 20, 70);
    Scalar upper_skin = Scalar(20, 255, 255);

    Mat mask;
    inRange(hsvFrame, lower_skin, upper_skin, mask);

    // Apply some morphological operations to remove noise
    erode(mask, mask, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
    dilate(mask, mask, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));

    dilate(mask, mask, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
    erode(mask, mask, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));

    // Find contours
    vector<vector<Point>> contours;
    findContours(mask, contours, RETR_TREE, CHAIN_APPROX_SIMPLE);

    for (size_t i = 0; i < contours.size(); i++)
    {
        double area = contourArea(contours[i]);

        // Ignore small contours that are likely noise
        if (area > 1000)
        {
            Rect bounding_box = boundingRect(contours[i]);
            rectangle(frame, bounding_box.tl(), bounding_box.br(), Scalar(0, 255, 0), 2);
        }
    }

    imshow("Hand Detection", frame);
}
