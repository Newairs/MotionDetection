#include "highgui.h"
#include <opencv2/opencv.hpp>

#include <iostream>
#include <stdlib.h>
#include <vector>
#include <algorithm>

using namespace cv;
using namespace std;

int main( int argc, char** argv )
{
    int dilation_size = 10;
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
	
    VideoCapture cap(-1); // open the default camera
    if(!cap.isOpened())  // check if we succeeded
    	return -1;

    Mat edges;
    namedWindow("edges",1);
    Mat firstFrame, frameDiff, dist, lastFrame, final;
	Point leftmost(0,0);
	Point rightmost(0,0);
    for(;;)
    {
        Mat frame;
        cap >> frame; // get a new frame from camera
        cvtColor(frame, edges, CV_BGR2GRAY);
        GaussianBlur(edges, edges, Size(7,7), 1.5, 1.5);
	if(firstFrame.total()==0)
		edges.copyTo(firstFrame);
        //Canny(edges, edges, 0, 30, 3);
	if(!lastFrame.empty()){
	absdiff(edges,lastFrame,frameDiff);
	threshold( frameDiff, dist, 30, 255,THRESH_BINARY); //Alterar o threshold implica em alterar a semsibilidade da câmera ao movimento.
	Mat element = getStructuringElement( MORPH_RECT,
                                       Size( 2*dilation_size + 1, 2*dilation_size+1 ),
                                       Point( dilation_size, dilation_size ) );
  	/// Apply the dilation operation
  	dilate( dist, dist, element ); // Operação morfológica que torna as linhas mais cheias, facilitanto o encontro dos contornos.
	Canny(dist, final, 0, 30, 3);
	findContours( final, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
	vector<vector<Point> > contours_poly( contours.size() );
	vector<Rect> boundRect( contours.size() );

	int count=0;
	for( int i = 0; i< contours.size(); i++ )
	     {
	        if(contourArea(contours[i],true)>6){
			leftmost += contours[i][0];
			rightmost += contours[i][0];
			count++;
		}
     	     }
	if(count >0){
		int mean = (leftmost.x + rightmost.x)/(2*count);
		if(mean>640/2 + 30)
			cout << "esquerda\n";
		else if(mean<640/2 - 30)
			cout << "direita\n";
		else
			cout << "centro\n";
	}
	leftmost = Point(0,0);
	rightmost = Point(0,0);
	imshow("feed", edges);
        imshow("edges", dist);
	imshow("rect", final);}
        if(waitKey(30) >= 0) break;
	edges.copyTo(lastFrame);
    }
    imwrite("first.jpg",firstFrame);
    // the camera will be deinitialized automatically in VideoCapture destructor
    return 0;

}
