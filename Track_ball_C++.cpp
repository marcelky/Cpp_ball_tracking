/*
 * Track_ball_C++.cpp
 *
 *  Created on: 16 de nov de 2018
 *      Author: marcelo
 */

/******************************************************************************
Versions
========
PA1       port code from python to C++                       23/11/2018

Overview
========
   This script tracks a ball and over the center of ball draw speed and
   acceleration color vectors. In addition these data can be seem on real
   time on graphs along the movement of ball.
   Detection of ball based on blog: www.pyimagesearch.com

Environment
===================
 * Opencv 4.0 Alpha
 * Ubuntu 16.04
 * g++ 6.5 (must be above version 6) if fail to compile with version 5.5
 * Tested using Ubuntu 16.04


Description
===========
   This script tracks a green ball seem on webcam or video file. The ball once
   detected, will have an small black circle created on an gray frame output,
   this position is called centroid.
   The movement of ball then can be seem looking at this black centroid, since
   we are using video input or webcam, the movement of ball will registered in
   different frames at different positions in the frame. This different of
   position in different frames is the distance covered by the ball between frames,
   in this script it is called as delta x (dx) and delta y (dy). dx is the distance
   covered horizontally and dy covered vertically.

   Since the time between frames are fixed, Tf, the average speed can be
   calculated as:
       speed horizontal : Vx= dx/Tf
       speed vertical   : Vy= dy/Tf
   To make simple we assume Vx is direct proportional to dx and Vy proportional
   to dy.
       Vx ~ dx and Vy ~ dy

   Acceleration is variation of speed, and based on the assumption above,
   we calculate the acceleration using:
       Acx = Vx[current] - Vx[last]
       Acy = Vy[current] - Vy[last]

   With these values V and Ac calculated it is used to put it as vector size
   on centroid.

   Notes ball moving left to right : Vx positive
                     right to left : Vx negative
                     Up to down    : Vy positive
                     Down to up    : Vy negative
                     *Acelleration follow the same sign rules

Usage
=====
case 1) Run the program "Track_ball_C++" and track a ball over input video file
"VID_track_ball.mp4". See command below:

./Track_ball_C++ VID_track_ball.mp4

case 2) Run the program "Track_ball_C++" and track a ball via webcam input.
See command below:
./Track_ball_C++

configuration of eclipse CDT C++ in  ubuntu
===========================================
Need to set this to make it work
	https://docs.opencv.org/2.4/doc/tutorials/introduction/linux_eclipse/linux_eclipse.html

Compilation tips using g++:

1)if pkg_config is not configure set it with opencv4.pc:
	export PKG_CONFIG_PATH=/usr/local/lib/pkgconfig/opencv4.pc

2)check version of opencv4
	pkg-config --modversion opencv4

3)check libs included in pkg-config this is useful to configure the compilation directive in eclipse.
	pkg-config --libs opencv4

	-L/usr/local/lib -lopencv_stitching -lopencv_superres -lopencv_videostab -lopencv_dpm
	-lopencv_tracking -lopencv_hfs -lopencv_line_descriptor -lopencv_img_hash -lopencv_aruco
	-lopencv_optflow -lopencv_reg -lopencv_ccalib -lopencv_ximgproc -lopencv_surface_matching
	-lopencv_saliency -lopencv_freetype -lopencv_fuzzy -lopencv_plot -lopencv_bgsegm -lopencv_video
	-lopencv_dnn_objdetect -lopencv_bioinspired -lopencv_datasets -lopencv_text -lopencv_dnn
	-lopencv_face -lopencv_photo -lopencv_xobjdetect -lopencv_objdetect -lopencv_stereo -lopencv_xphoto
	-lopencv_xfeatures2d -lopencv_shape -lopencv_ml -lopencv_rgbd -lopencv_structured_light
	-lopencv_calib3d -lopencv_features2d -lopencv_highgui -lopencv_videoio -lopencv_imgcodecs
	-lopencv_phase_unwrapping -lopencv_imgproc -lopencv_flann -lopencv_core

During the configuration of eclipse it will need some configuration explained in the links below:
	http://www.codebind.com/cpp-tutorial/install-opencv-ubuntu-cpp/
	https://docs.opencv.org/2.4/doc/tutorials/introduction/linux_eclipse/linux_eclipse.html

Remember to set the C/C++ general => paths and symbols

Very important opencv must have g++ version 6.5 or above with version 5.5, otherwise the compilation will fail.!!!


Eclipse settings:
=================
GCC C++ compiler => Includes => Include paths (-I) add:
/usr/local/include/opencv4
/usr/local/include/opencv4/opencv2
command use g++-6 (instead of g++ which this system is 5.5)


GCC compiler => Includes => Include paths (-I) add:
/usr/local/include/opencv4
/usr/local/include/opencv4/opencv2


GCC C++ linker => Libraries => Libraries (-I)
opencv_core
opencv_imgcodecs
opencv_imgproc
opencv_highgui
command use g++-6 (instead of g++ which this system is 5.5)

GCC C++ linker => Libraries => Libraries (-L)
/usr/local/lib

TIPS to Integrate with logger
=============================
source code of logger:
https://github.com/fjanisze/logger

In case the compilation of logs fail, add the following configuration to
Project -> Properties -> GCC++ builder ->
C/C++ Build -> C/C++Build -> Settings -> GCC C++ Linker -> General ->
Set Support for pthread (-pthread)

Just for information in case the trick above does not work with logger try this extra configuration:
C/C++ Build -> C/C++Build -> Settings -> GCC C++ Compiler -> Miscellaneous ->
  Set Support for pthread (-pthread)
	At Other flags add "-lz". It will looks like this:
	Other flags:   "-c -fmessage-length=0 -lz"
*******************************************************************************/

#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/video.hpp>
#include <opencv2/plot.hpp>

#include <iostream>
#include <vector>
#include <math.h>
#include <numeric>


#include "Ball.h"
#include "logger-master/logger.hpp"
#include "ColorsDef.h"

using namespace cv;

Mat make_grid(Mat fr){
	uint8_t step = 80;

	//write horizontal line
	for (int r=step; r<=fr.rows; r+=step){
		line(fr, Point(0,r), Point(fr.cols, r), ColorsDef::BLACK, 1);
	}

	//write vertical line
	for (int c=step; c <=fr.cols; c+=step){
		line(fr, Point(c,0), Point(c, fr.rows), ColorsDef::BLACK, 1);
	}
	return fr;
}

int main( int argc, char** argv) {  //or char* argv[]

	SET_LOG_THREAD_NAME( "MAIN" );

	//Declaring variables
	float e1, e2, freq, tick_freq, timeC;
	Point pt;  //point (x,y)
	Point2f center; //
	float radius;
	Moments M;

	Mat frame, blurred, hsv, mask, plot_result;
	std::vector<std::vector<cv::Point>> contours;

	uint64 frame_counter = 0;
	Ball green_ball;

    LOG1("Starting the application..");

    //Defining the source of images webcam or file
    VideoCapture camera;
    if (argc < 2){
    	camera.open(0);
    }
    else{
    	camera.open(argv[1]);
    }

	if(!camera.isOpened()){
		std::cout <<"cannot open camera";
	}


	//unconditional loop to process each frame
	while (true){

		e1 = cv::getTickCount();
		frame_counter+= 1;

		//Declaring variables
		//Mat frame, blurred, hsv, mask, plot_result;
		//std::vector<std::vector<cv::Point>> contours;

		camera.read(frame);
		if(frame.empty()) break;

		//LOG1("height: ", frame.rows, " width: ", frame.cols );
		Mat blank_frame(frame.rows, frame.cols, CV_8UC3, ColorsDef::GRAY); //create a frame with gray color
		blank_frame = make_grid(blank_frame);

		GaussianBlur(frame, blurred, Size(11,11), 0, 0, BORDER_DEFAULT);
		cvtColor(blurred, hsv, COLOR_BGR2HSV, 0);
		inRange(hsv, ColorsDef::GREEN_DARK, ColorsDef::GREEN_LIGHT, mask);
		erode(mask, mask, Mat(), Point(-1,-1), 2, BORDER_CONSTANT, morphologyDefaultBorderValue());
		dilate(mask, mask, Mat(), Point(-1,-1), 2, BORDER_CONSTANT, morphologyDefaultBorderValue());
		findContours(mask, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

		//Stackoverflow 33401745 - find largest contours opencv
		std::vector<int> indices(contours.size());
		iota(indices.begin(),indices.end(),0);
		sort(indices.begin(), indices.end(), [&contours](int lhs, int rhs){
			return contours[lhs].size() > contours[rhs].size();
		});

		int N = 1;
		N = cv::min(N, int(contours.size()));
		for (int i = 0; i < N; i++){
			//StackOverflow 47457918 - how get the minimum enclosing circle with opencv
			minEnclosingCircle(contours[indices[i]], center, radius);
			M = moments(contours[i], 0);
			center = Point(int(M.m10/M.m00),int(M.m01/M.m00));

			if(radius > 10){

				//collectting data
				green_ball.add_position(center, frame_counter);
				green_ball.delete_first_position(20);
				green_ball.recenter(1);


				//drawing information on original frame
				//green_ball.draw_polyline(frame, 10);
				//circle(frame, center, radius, Scalar(255,0,0), 2);
				//circle(frame, center, 5, Scalar(0,0,255), 1);

				//drawing information on blank_frame
				green_ball.draw_polyline(blank_frame, 10);
				circle(blank_frame, center, radius, ColorsDef::BLACK, 2);
				circle(blank_frame, center, 5, ColorsDef::BLACK, 1);


				//Ptr<plot::Plot2d> plot = plot::Plot2d::create(green_ball.get_speed());
				//plot->render(plot_result);
				//imshow("plot", plot_result);
				//plot::Plot2d::create(green_ball.get_speed());
			}
		}

		if (green_ball.get_id()>= 3){
			green_ball.draw_vectorxy(blank_frame, green_ball.get_last_speed(0), 15, ColorsDef::NAVY,  ColorsDef::RED);
			green_ball.draw_vectorxy(blank_frame, green_ball.get_last_accel(0), 15, ColorsDef::PURPLE, ColorsDef::CYAN);
		}

		//imshow("original frame", frame);
		imshow("processed", blank_frame);


		if(waitKey(1)>=0)
			break;

		e2 = getTickCount();
		tick_freq = getTickFrequency();
		timeC = e2-e1;
		timeC = timeC/tick_freq;
		freq = 1/timeC;
		LOG1("Frequency:", freq);
	}
	camera.release();
	return 0;
}



