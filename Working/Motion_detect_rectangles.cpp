#include "highgui.h"
#include <opencv2/opencv.hpp>

#include <iostream>
#include <stdlib.h>

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

	for( int i = 0; i< contours.size(); i++ )
	     {
	        if(contourArea(contours[i],true)>6){
			cout << "Hello\n" << i;
			approxPolyDP( Mat(contours[i]), contours_poly[i], 3, true );
       			boundRect[i] = boundingRect( Mat(contours_poly[i]) );
			boundRect[i] = boundingRect( Mat(contours[i]) );
			rectangle( final, boundRect[i].tl(), boundRect[i].br(), (0,255,0), 10, 3, 0 );
		}
     	     }


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
