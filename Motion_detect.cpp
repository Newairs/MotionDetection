#include "highgui.h"
#include <opencv2/opencv.hpp>

#include <iostream>
#include <stdlib.h>
#include <vector>
#include <algorithm>

using namespace cv;
using namespace std;

#define dilation_size 10  //Declaration for the dilation window size, used in the morphologic operations
#define cam_width 320
#define cam_lenght 180

int main( int argc, char** argv )
{
   
	/* Loop invariant variables declaration block */
	//begin
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;	
	Mat gray, blur, firstFrame, frameDiff, dist, lastFrame, final;
	Point leftmost(0,0);
	Point rightmost(0,0);
	//end
    	
	/* Camera inicialization */
	VideoCapture cap(-1); // open the default camera
	
	cap.set (CV_CAP_PROP_FRAME_WIDTH, cam_width);
    cap.set (CV_CAP_PROP_FRAME_HEIGHT, cam_lenght);
	
    if(!cap.isOpened())  // check if we succeeded
    	return -1;

    //namedWindow("edges",1); // creates a window?

	/* Main loop */    
    for(;;)
    {
        /* Frame capture block */
		/* Captures frame and converts it to grayscale. It also blurs the frame
		for further analysis later. */
		// begin 
		Mat frame; //This frame receives the current camera frame
        cap >> frame; // get a new frame from camera
        cvtColor(frame, gray, CV_BGR2GRAY); //Converts the frame to grayscale
        GaussianBlur(gray, blur, Size(7,7), 1.5, 1.5); // Blur image for easy diference capture
		// end

		if(firstFrame.total()==0)	//If its the first frame, save it.
			blur.copyTo(firstFrame);
        
		/* Comparison block */
		/* Only applied after the second frame is captured. The different pixel between the current frame and old frame, albeit 
		a threshold consideration, idicate movement in the scene. The movement sources are contourned and the mean central 
		point of movement is calculated, intending to find wether the camera will turn left, right or stay put. */
		// begin
		if(!lastFrame.empty())
		{
			
			absdiff(blur, lastFrame, frameDiff); // Finds different pixels among the two frames.
			threshold( frameDiff, dist, 30, 255,THRESH_BINARY); // The threshold indicates how sensible is the camera to 											environmental movement. It should be noted that a minimun 											threshold is requires, as even two consecutive frames of a 											motionless capture are different to a certain degree.
			
			Mat element = getStructuringElement( MORPH_RECT,
        			                               Size( 2*dilation_size + 1, 2*dilation_size+1 ),
        			                               Point( dilation_size, dilation_size ) );	// Creates the window for 														morphological operations.
  		
  			dilate( dist, dist, element ); // Apply the dilation operation
			
			// These two operations find the contours in the image, effectively tracing the sources of movement.
			Canny(dist, final, 0, 30, 3); 
			findContours( final, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
			
			vector<vector<Point> > contours_poly( contours.size() );
			vector<Rect> boundRect( contours.size() );

			/* Movement source localization block */
			/* This block takes in consideration all the contours that have sufficient area to be recognized as a 
			valid movement source. It then calculates the leftmost and rightmost point, and calculates the mean 
			central point for the frame. */
			// begin
			
			int count=0;
			for( int i = 0; i< contours.size(); i++ )
			{
				if(contourArea(contours[i],true)>6)
				{
					leftmost += contours[i][0];
					rightmost += contours[i][0];
					count++;
				}
     		}
			
			if(count >0)
			{
				int mean = (leftmost.x + rightmost.x)/(2*count);
				if(mean>cam_width/2 + cam_width/6)
					cout << "esquerda\n";
				else if(mean<cam_width/2 - cam_width/6)
					cout << "direita\n";
				else
					cout << "centro\n";
			}
			// end
	
			leftmost = Point(0,0); //reinitializes the variables for the next loop
			rightmost = Point(0,0); 

			//imshow("feed", blur); // Show results.
		    //imshow("edges", dist);
			//imshow("rect", final);
		}
        		
		//if(waitKey(30) >= 0) break; // Awaits for any key to be pressed. closes the program.
		
		blur.copyTo(lastFrame); // Saves the frame for the analysis in the next loop.
	}
	// end    

	imwrite("first.jpg",firstFrame); // Saves the first frame.
	// the camera will be deinitialized automatically in VideoCapture destructor
	return 0;
}
