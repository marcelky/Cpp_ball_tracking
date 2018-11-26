# Cpp_ball_tracking
SW to track a ball draw speed/acceleration vectors XY on it

# Overview

   This script tracks a ball and over the center of ball draw speed and
   acceleration color vectors.
   
   Detection of ball based on blog: www.pyimagesearch.com

# Environment

 * Opencv 4.0 Alpha
 
 * Ubuntu 16.04
 
 * g++ 6.5 (must be above version 6) if fail to compile with version 5.5
 
 * Tested using Ubuntu 16.04
 
# Description

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

# Usage

case 1) Run the program "Track_ball_C++" and track a ball over input video file
"VID_track_ball.mp4". See command below:

./Track_ball_C++ VID_track_ball.mp4

case 2) Run the program "Track_ball_C++" and track a ball via webcam input.
See command below:
./Track_ball_C++

# Configuration of eclipse CDT C++ in  ubuntu

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


# Eclipse settings:

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

# TIPS to Integrate with logger

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
