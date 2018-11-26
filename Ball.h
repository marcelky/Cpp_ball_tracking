/*
 * Ball.h
 *
 *  Created on: 18 de nov de 2018
 *      Author: marcelo
 */

#ifndef BALL_H_
#define BALL_H_

#include "opencv2/highgui.hpp"
#include <vector>
#include <math.h>
#include <numeric>

class Ball {
	int id=0;
	int frames_since_seen = 0;
	int last_frame_number =0;
	std::vector<cv::Point> positions;
	std::vector<cv::Point> speed;
	std::vector<cv::Point> acceleration;
	cv::Point recenter_position;

public:
	Ball();
	cv::Point recenter(int factor);
	void draw_polyline(cv::Mat output_image, uint64 keep_n_positions);
	void delete_first_position(uint keep_n_positions);
	cv::Point get_last_position(int index);
	cv::Point get_last_speed(int index);
	cv::Point get_last_accel(int index);
	std::vector<cv::Point> get_speed();
	int get_id();
	void add_position(cv::Point new_position, int frame_counter);
	void calculate_speed();
	void calculate_acceleration();

	void draw_vectorxy(cv::Mat output_image, cv::Point source_data, int increase_size, cv::Scalar colorH, cv::Scalar colorV);
	cv::Point moving_avg_window(cv::Point new_point, std::vector<cv::Point> vector2D, uint n_points);
	void draw_rectangle_centroid(cv::Mat fr, int radius);



};

#endif /* BALL_H_ */
