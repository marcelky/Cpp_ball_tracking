/***************************************************************************************
 * Ball.cpp
 *
 *  Created on: 18 de nov de 2018
 *      Author: marcelo
 ****************************************************************************************/
#include "Ball.h"
#include "logger-master/logger.hpp"
#include "opencv2/opencv.hpp"
#include "ColorsDef.h"

//Constructor
Ball::Ball() {
	SET_LOG_THREAD_NAME("Ball");
	id=0;
	frames_since_seen = 1;
	last_frame_number = 0;
}

/***************************************************************************************
 * cv::Point Ball::recenter(int factor)
 * This method recalculate the position of ball in order to overcome the
 * delay problem to project the image of centroid and its vectors vx/vy
 * on the wall. In this way it gets the centroid of ball in the current
 * frame and then it is added the speed (delta of position) multiplied by
 * factor.
 * Return: an (x,y) estimate position considering the delay to project on the wall
 * Parameters: factor, how many steps we take to catch an estimate position
 *             0, recenter does not change anything
 *             1, recenter add deltaX * factor, deltaY * factor *
 ****************************************************************************************/
cv::Point Ball::recenter(int factor){
	recenter_position = get_last_position(0);
	cv::Point delta_position = get_last_speed(0);
	recenter_position.x = recenter_position.x + delta_position.x * factor;
	recenter_position.y = recenter_position.y + delta_position.y * factor;
	return recenter_position;
}

/***************************************************************************************
 * void Ball::draw_polyline(cv::Mat output_image, uint64 keep_n_positions)
 * This method to draw a trajectory of ball using the latest n_positions
 * Return: none
 * Parameters:
 * 		output_image = image where to draw the polylines
 * 	    n_positions = vector of centroid to draw the polyline
 ***************************************************************************************/
void Ball::draw_polyline(cv::Mat output_image, uint64 n_positions){
	if (positions.size() >= n_positions){
		cv::Mat ball_path(positions, true);
		ball_path.convertTo(ball_path, CV_32S);
		cv::polylines(output_image, positions, false, ColorsDef::BLACK, 1, cv::LINE_AA, 0);
	}
}

/***************************************************************************************
 * void Ball::delete_first_position(int keep_n_positions)
 * This method is used to avoid the vectors positions/speed/acceleration
 * keep growing without end. It keep the size of vector with keep_n_positions, deleting
 * the first element of vector to keep the fixed size.
 * Return: none
 * Parameters:
 * 		keep_n_positions = Maximum fixed size of vector
 ***************************************************************************************/
void Ball::delete_first_position(uint keep_n_positions){
	if(positions.size()== keep_n_positions){
		positions.erase(positions.begin());
		speed.erase(speed.begin());
		acceleration.erase(acceleration.begin());
	}
}

/**************************************************************************************
 * cv::Point get_last_position (int index)
*  This method return the last position according to index value
 * Return: the last position according to index value
 * Parameters:
 * 		index = 0 return last position
 *      	    1 next to last next to last position
 **************************************************************************************/
cv::Point Ball::get_last_position(int index){
	return positions.at(positions.size() - 1 -index);
}

/**************************************************************************************
 * cv::Point get_last_speed (int index)
 * This method return the last speed according to index value
 * Return: the last speed according to index value
 * Parameters:
 * 		index = 0 return last speed
 *      	    1 next to last speed
 **************************************************************************************/
cv::Point Ball::get_last_speed(int index){
	return speed.at(speed.size() -1 - index);
}

/*************************************************************************************
 * cv::Point get_last_accel (int index)
 * This method return the last acceleration according to index value
 * Return: the last acceleration according to index value
 * Parameters:
 * 		index = 0 return last acceleration
 *      	    1 next to last acceleration and so on
 **************************************************************************************/
cv::Point Ball::get_last_accel(int index){
	return acceleration.at(speed.size() - 1 - index);
}

/*************************************************************************************
 * std::vector<cv::Point> Ball::get_position()
 * This method return the speed vector
 * Return: speed vector
 * Parameters: none
 **************************************************************************************/
std::vector<cv::Point> Ball::get_speed(){
	return speed;
}

/*************************************************************************************
 * int Ball::get_id()
 * This method return the id
 * Return: the last id number
 * Parameters: none
 **************************************************************************************/
int Ball::get_id(){
	return id;
}


/*************************************************************************************
 * void Ball::add_position(cv::Point new_position, int frame_counter)
 * This method store the value new_position in positions vector and calculate and save
 * speed and acceleration and save respectively in vectors speed and acceleration.
 * Return: void
 * Parameters:
 *		new_position= ball's center in detected in current frame
 *		frame_counter= frame number related to this new_position found
 **************************************************************************************/
void Ball::add_position(cv::Point new_position, int frame_counter){
	id += 1;
	positions.push_back(new_position);

	if(last_frame_number == 0){
		frames_since_seen = 1;    // does not affect the result
	}
	else{
		frames_since_seen = frame_counter - last_frame_number;
	}
	last_frame_number = frame_counter;
	calculate_speed();
	calculate_acceleration();
	//LOG1("Position: ", new_position, ", Speed:", get_last_speed(0),", Accel: ", get_last_accel(0));
	//LOG1("Position: ", new_position, ", Speed:", get_last_speed(0));
}

/*************************************************************************************
 * void Ball::calculate_speed()
 * This method calculate the speed based on the latest two values stored in
 * positions vector. Stored as point (x,y) where x is the speed horizontal and y is the
 * speed at vertical.
 * Return: void
 * Parameters: none
 **************************************************************************************/
void Ball::calculate_speed(){
	cv::Point curr_pt, prev_pt, curr_speed;
	if(speed.size()>= 2){
		curr_pt = get_last_position(0);
		prev_pt = get_last_position(1);

		curr_speed.x = int((curr_pt.x - prev_pt.x)/frames_since_seen);
		curr_speed.y = int((curr_pt.y - prev_pt.y)/frames_since_seen);

		curr_speed = moving_avg_window(curr_speed, speed, 3);

		speed.push_back(curr_speed);
	}
	else{
		speed.push_back(cv::Point(0,0));
	}
}

/*************************************************************************************
 * void Ball::calculate_acceleration()
 * This method calculate the acceleration based on the latest two values stored in
 * speed vector. Stored as point (x,y) where x is the acceleration horizontal and y is
 * the acceleration at vertical.
 * Return: void
 * Parameters: none
 **************************************************************************************/
void Ball::calculate_acceleration(){
	cv::Point curr_speed, prev_speed, accel;
	if(positions.size()>=3){
		curr_speed = get_last_speed(0);
		prev_speed = get_last_speed(1);

		accel.x = int((curr_speed.x - prev_speed.x)/frames_since_seen);
		accel.y = int((curr_speed.y - prev_speed.y)/frames_since_seen);
	}
	else{
		accel.x = 0;
		accel.y = 0;
	}
	acceleration.push_back(accel);
}

/*************************************************************************************
 * void Ball::draw_vectorxy(cv::Mat output_image,
 *                          cv::Point source_data,
 *                          int factor,
 *                          cv::Scalar colorH,
 *                          cv::Scalar colorV){
 * This draw vector on center of ball, according to the source_data (either speed or
 * acceleration). In speed or acceleration vector, source_data the (x,y), x represents
 * horizontal dimension and y represents the vertical dimension.
 * Return: void
 * Parameters:
 * 		output_image = place to draw the vectors
 * 		source_data = point (x,y) can be speed or acceleration x mean horizontal and y
 * 		              vertical
 * 		increase_size = how many times increase the vector (x,y) to make vector visible
 * 		colorH = horizontal color of vector
 * 		colorV = vertical color of vector
 **************************************************************************************/
void Ball::draw_vectorxy(cv::Mat output_image, cv::Point source_data, int increase_size, cv::Scalar colorH, cv::Scalar colorV){
	cv::Point p_end_axis_x, p_end_axis_y, p_end_axis_xy;

	//Calculate end point of horizontal Vector
	p_end_axis_x.x = recenter_position.x + source_data.x * increase_size;
	p_end_axis_x.y = recenter_position.y;

	//Calculate end point of vertical Vector
	p_end_axis_y.x = recenter_position.x;
	p_end_axis_y.y = recenter_position.y + source_data.y * increase_size;

	//Calculate end point of diagonal Vector
	p_end_axis_xy.x = recenter_position.x + source_data.x * increase_size;
	p_end_axis_xy.y = recenter_position.y + source_data.y * increase_size;

	//draw horizontal vector
	cv::arrowedLine(output_image, recenter_position, p_end_axis_x, colorH, 4);

	//draw vertical vector
	cv::arrowedLine(output_image, recenter_position, p_end_axis_y, colorV, 4);


	//draw diagonal vector
	//cv::arrowedLine(output_image, recenter_position, p_end_axis_xy, colorV, 4);

	//draw rectangle
	//cv::rectangle(output_image, recenter_position, p_end_axis_xy, colorH, 1);
}

/*************************************************************************************
 *  cv::Point Ball::moving_avg_window(cv::Point new_point, std::vector<cv::Point> vector2D, int n_points)
 *  Method calculate the average value among the given lastCentroid and centroid
 *  stored in the Vehicle object
 *  Return: Average value calculated considering n_points
 *  Parameters:
 *     new_point = new (x,y) value to use when take the average
 *     vector2D = vector to recover latest values (n_points - 1) to take average
 *     n_points = 0 not valid, 1 result is the own lastCentroid, 2 result is the
 *                average of lastCentroid and position[-1]. 3 result is the average
 *                of lastCentroid, position[-1], position[-2] and so on.
**************************************************************************************/
cv::Point Ball::moving_avg_window(cv::Point new_point, std::vector<cv::Point> vector2D, uint n_points){
	cv::Point avg_vector2D, input_vector;
	avg_vector2D= new_point;

	if(((vector2D.size())>= n_points - 1) and n_points!=0){
		for (uint i=0; i < n_points-1; i++){
			input_vector = vector2D.at(vector2D.size() - 1 - i);
			avg_vector2D.x = avg_vector2D.x + input_vector.x;
			avg_vector2D.y = avg_vector2D.y + input_vector.y;
		}

		avg_vector2D.x = int(avg_vector2D.x/n_points);
		avg_vector2D.y = int(avg_vector2D.y/n_points);
	}
	return avg_vector2D;
}



