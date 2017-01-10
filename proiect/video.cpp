#include <sstream>
#include <string>
#include <iostream>
//#include <opencv2\highgui.h>
#include "opencv2/highgui/highgui.hpp"
//#include <opencv2\cv.h>
#include "opencv2/opencv.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"


#include "Car.h"
#include "Car.cpp"


using namespace std;
using namespace cv;
//initial min and max HSV filter values.
//these will be changed using trackbars
int H_MIN = 0;
int H_MAX = 256;
int S_MIN = 0;
int S_MAX = 256;
int V_MIN = 0;
int V_MAX = 256;

//bun
int BLUE_H_MIN = 78;
int BLUE_H_MAX = 128;
int BLUE_S_MIN = 11;
int BLUE_S_MAX = 149;
int BLUE_V_MIN = 197;
int BLUE_V_MAX = 256;


int RED_H_MIN = 69;
int RED_H_MAX = 94;
int RED_S_MIN = 15;
int RED_S_MAX = 68;
int RED_V_MIN = 189;
int RED_V_MAX = 256;

int YELLOW_H_MIN = 59;
int YELLOW_H_MAX = 67;
int YELLOW_S_MIN = 76;
int YELLOW_S_MAX = 152;
int YELLOW_V_MIN = 0;
int YELLOW_V_MAX = 256;

int BLACK_H_MIN = 0;
int BLACK_H_MAX = 150;
int BLACK_S_MIN = 0;
int BLACK_S_MAX = 150;
int BLACK_V_MIN = 0;
int BLACK_V_MAX = 150;

//bun
int GREEN_H_MIN = 62;
int GREEN_H_MAX = 84;
int GREEN_S_MIN = 88;
int GREEN_S_MAX = 242;
int GREEN_V_MIN = 210;
int GREEN_V_MAX = 256;




/////////////
RNG rng(12345);
//////////////



const int FRAME_WIDTH = 640;
const int FRAME_HEIGHT = 480;
const int MAX_NUM_OBJECTS = 50;
const int MIN_OBJECT_AREA = 20 * 20;
const int MAX_OBJECT_AREA = FRAME_HEIGHT*FRAME_WIDTH / 1.5;
const std::string windowName = "Original Image";
const std::string windowName1 = "HSV Image";
const std::string windowName2 = "Thresholded Image";
const std::string windowName3 = "After Morphological Operations";
const std::string trackbarWindowName = "Trackbars";


void on_mouse(int e, int x, int y, int d, void *ptr)
{
	if (e == EVENT_LBUTTONDOWN)
	{
		cout << "Left button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
	}
}

void on_trackbar(int, void*)
{//This function gets called whenever a
 // trackbar position is changed
}

string intToString(int number) {


	std::stringstream ss;
	ss << number;
	return ss.str();
}

void createTrackbars() {
	//create window for trackbars


	namedWindow(trackbarWindowName, 0);
	//create memory to store trackbar name on window
	char TrackbarName[50];
	sprintf(TrackbarName, "H_MIN", H_MIN);
	sprintf(TrackbarName, "H_MAX", H_MAX);
	sprintf(TrackbarName, "S_MIN", S_MIN);
	sprintf(TrackbarName, "S_MAX", S_MAX);
	sprintf(TrackbarName, "V_MIN", V_MIN);
	sprintf(TrackbarName, "V_MAX", V_MAX);
	//create trackbars and insert them into window
	//3 parameters are: the address of the variable that is changing when the trackbar is moved(eg.H_LOW),
	//the max value the trackbar can move (eg. H_HIGH),
	//and the function that is called whenever the trackbar is moved(eg. on_trackbar)
	//                                  ---->    ---->     ---->
	createTrackbar("H_MIN", trackbarWindowName, &H_MIN, H_MAX, on_trackbar);
	createTrackbar("H_MAX", trackbarWindowName, &H_MAX, H_MAX, on_trackbar);
	createTrackbar("S_MIN", trackbarWindowName, &S_MIN, S_MAX, on_trackbar);
	createTrackbar("S_MAX", trackbarWindowName, &S_MAX, S_MAX, on_trackbar);
	createTrackbar("V_MIN", trackbarWindowName, &V_MIN, V_MAX, on_trackbar);
	createTrackbar("V_MAX", trackbarWindowName, &V_MAX, V_MAX, on_trackbar);


}
void drawObject(int x, int y, Mat &frame) {

	//use some of the openCV drawing functions to draw crosshairs
	//on your tracked image!

	//UPDATE:JUNE 18TH, 2013
	//added 'if' and 'else' statements to prevent
	//memory errors from writing off the screen (ie. (-25,-25) is not within the window!)

	circle(frame, Point(x, y), 20, Scalar(0, 255, 0), 2);
	if (y - 25 > 0)
		line(frame, Point(x, y), Point(x, y - 25), Scalar(0, 255, 0), 2);
	else line(frame, Point(x, y), Point(x, 0), Scalar(0, 255, 0), 2);
	if (y + 25 < FRAME_HEIGHT)
		line(frame, Point(x, y), Point(x, y + 25), Scalar(0, 255, 0), 2);
	else line(frame, Point(x, y), Point(x, FRAME_HEIGHT), Scalar(0, 255, 0), 2);
	if (x - 25 > 0)
		line(frame, Point(x, y), Point(x - 25, y), Scalar(0, 255, 0), 2);
	else line(frame, Point(x, y), Point(0, y), Scalar(0, 255, 0), 2);
	if (x + 25 < FRAME_WIDTH)
		line(frame, Point(x, y), Point(x + 25, y), Scalar(0, 255, 0), 2);
	else line(frame, Point(x, y), Point(FRAME_WIDTH, y), Scalar(0, 255, 0), 2);

	putText(frame, intToString(x) + "," + intToString(y), Point(x, y + 30), 1, 1, Scalar(0, 255, 0), 2);
	//cout << "x,y: " << x << ", " << y;
}
void morphOps(Mat &thresh) {

	

	Mat erodeElement = getStructuringElement(MORPH_RECT, Size(3, 3));
	//dilate with larger element so make sure object is nicely visible
	Mat dilateElement = getStructuringElement(MORPH_RECT, Size(8, 8));

	erode(thresh, thresh, erodeElement);
	erode(thresh, thresh, erodeElement);


	dilate(thresh, thresh, dilateElement);
	dilate(thresh, thresh, dilateElement);



}
void trackFilteredObject(int &x, int &y, Mat threshold, Mat &cameraFeed) {

	Mat temp;
	threshold.copyTo(temp);
	//these two vectors needed for output of findContours
	vector< vector<Point> > contours;
	vector<Vec4i> hierarchy;
	//find contours of filtered image using openCV findContours function
	findContours(temp, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);
	double refArea = 0;
	bool objectFound = false;
	if (hierarchy.size() > 0) {
		int numObjects = hierarchy.size();
		if (numObjects < MAX_NUM_OBJECTS) {
			for (int index = 0; index >= 0; index = hierarchy[index][0]) {

				Moments moment = moments((cv::Mat)contours[index]);
				double area = moment.m00;

		
				if (area > MIN_OBJECT_AREA && area<MAX_OBJECT_AREA && area>refArea) {
					x = moment.m10 / area;
					y = moment.m01 / area;
					objectFound = true;
					refArea = area;
                    drawObject(x,y,cameraFeed);
				}
				else objectFound = false;


			}
			//let user know you found an object
			if (objectFound == true) {
				putText(cameraFeed, "Tracking Object", Point(0, 50), 2, 1, Scalar(0, 255, 0), 2);
				draw object location on screen
				cout << x << "," << y;
                		drawObject(x, y, cameraFeed);

			}


		}
		else putText(cameraFeed, "TOO MUCH NOISE! ADJUST FILTER", Point(0, 50), 1, 2, Scalar(0, 0, 255), 2);
	}
}

void contouresStuff(InputOutputArray image){
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;

    findContours(image, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0) );

    Mat drawing = Mat::zeros( image.size(), CV_8UC3 );
   // cout<<contours.size()<<endl;

   // for( size_t i = 0; i< contours.size(); i++ )
    //{
        Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
        drawContours( drawing, contours, /*(int)i*/ 2, color, 2, 8, hierarchy, 0, Point() );
    //}

    for(size_t i=0; i<hierarchy.size(); i++){
        cout<<hierarchy[i][0];
        cout<<",";
        cout<<hierarchy[i][1];
        cout<<endl;
    }

    cout<<"----------------------------------------"<<endl;
    namedWindow( "Components", 1 );
    imshow( "Components", drawing );

}


int main(int argc, char* argv[])
{

	//some boolean variables for different functionality within this
	//program
	bool trackObjects = true;
	bool useMorphOps = true;

	Point p;
	//Matrix to store each frame of the webcam feed
	Mat cameraFeed;
	//matrix storage for HSV image
	Mat HSV;
	//matrix storage for binary thresholdBlue image
	Mat thresholdBlue;
    Mat thresholdRed;
	//x and y values for the location of the object
	int x = 0, y = 0;
	//create slider bars for HSV filtering
	//createTrackbars();
	//video capture object to acquire webcam feed
	VideoCapture capture;


    //open the capture
    if(argc < 2){
        //open capture object at location zero (default location for webcam)
        capture.open(0);
    }else{
        if(strstr(argv[1], "rtmp://") == NULL)
            capture.open("rtmp://172.16.254.63/live/live");
        }else{
            capture.open(argv[1]);
        }

    }

	
	capture.set(CV_CAP_PROP_FRAME_WIDTH, FRAME_WIDTH);
	capture.set(CV_CAP_PROP_FRAME_HEIGHT, FRAME_HEIGHT);
	//livefeed

/*
    Car c;
    c.connectToCar();

    char message[100];
    while(1){
        printf("Enter message : ");
        scanf("%s" , message);
        c.sendCommand(message);
    }
*/
	
	while (1) {


		//Saving the feed
		capture.read(cameraFeed);
		cvtColor(cameraFeed, HSV, COLOR_BGR2HSV);
        inRange(HSV, Scalar(BLACK_H_MIN, BLACK_S_MIN, BLACK_V_MIN), Scalar(BLACK_H_MAX, BLACK_S_MAX, BLACK_V_MAX), thresholdBlue);

        inRange(HSV, Scalar(RED_H_MIN, RED_S_MIN, RED_V_MIN), Scalar(RED_H_MAX, RED_S_MAX, RED_V_MAX), thresholdRed);
		//noise correction
		if (useMorphOps) {
            morphOps(thresholdBlue);
            morphOps(thresholdRed);
        }

        contouresStuff(thresholdBlue);

		//Cord return
		if (trackObjects) {
            trackFilteredObject(x, y, thresholdBlue, cameraFeed);
            trackFilteredObject(x, y, thresholdRed, cameraFeed);
            //trackFilteredObject(x, y, thresholdBlue, cameraFeed);
        }
		//show frames
		imshow("Blue threshold", thresholdBlue);
        imshow("Red threshold", thresholdRed);
		imshow(windowName, cameraFeed);
		//imshow(windowName1, HSV);
		setMouseCallback("Original Image", on_mouse, &p);
		//delay 30ms so that screen can refresh.
		//image will not appear without this waitKey() command
		waitKey(30);
	}

	return 0;
}
