/************************************************************************\
| Card isolation file for EE4H Assignment						         |
|																		 |
| Authors: Yousef Amar and Chris Lewis									 |
| Last Modified: 19/02/2014												 |
|																		 |
\************************************************************************/

#include "../include/stdafx.h"

using namespace std;

//Globals - may be more elegant another way
static int current_square = 0;
static int current_threshold = 0;
static cv::Mat current_image;

/*
 * Find the number of squares in an image.
 * 
 * Arguments
 *                cv::Mat image:     The image input
 * vector<vector<Point>>& squares:   The array of squares
 *                    int threshold: Threshold for finding a quad
 */
static void find_squares(cv::Mat image, vector<vector<cv::Point> >& squares, int threshold)
{
	squares.clear();

	cv::Mat grey8(image.size(), CV_8U), grey;
	cv::cvtColor(image, grey8, CV_BGR2GRAY);
	cv::imshow("Grey", grey8);
	cv::equalizeHist(grey8, grey8);
	cv::imshow("Grey + Equalised Histogram", grey8);
	cv::blur(grey8, grey8, cv::Size(4, 4));
	cv::imshow("Grey + Equalised Histogram + 4x4 Blur", grey8);
	
	vector<vector<cv::Point> > contours;

	grey = grey8 >= threshold;
	cv::imshow("Grey + Equalised Histogram + 4x4 Blur + Threshold", grey);

	// find contours and store them all as a list
	cv::findContours(grey, contours, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);

	vector<cv::Point> approx;

	// test each contour
	for(size_t i = 0; i < contours.size(); i++)
	{
		// approximate contour with accuracy proportional
		// to the contour perimeter
		cv::approxPolyDP(cv::Mat(contours[i]), approx, cv::arcLength(cv::Mat(contours[i]), true)*0.02, true);

		// square contours should have 4 vertices after approximation
		// relatively large area (to filter out noisy contours)
		// and be convex.
		// Note: absolute value of an area is used because
		// area may be positive or negative - in accordance with the
		// contour orientation
		if( approx.size() == 4 &&
			fabs(cv::contourArea(cv::Mat(approx))) > 1000 &&
			cv::isContourConvex(cv::Mat(approx)))
		{

			squares.push_back(approx);
		}
	}
}

/*
 * Apply a Hough Transform (!)
 *
 * Arguments
 * cv::Mat input: Input image
 *
 * Returns
 * cv::Mat: Output with lines drawn on
 */
cv::Mat hough_trans(cv::Mat input)
{
	cv::Mat edges, output;
	cv::Canny(input, edges, 50, 200, 3);

	cv::cvtColor(edges, output, cv::COLOR_GRAY2BGR);

	vector<cv::Vec4i> lines;
	cv::HoughLinesP(edges, lines, 1, CV_PI/180, 50, 50, 10);
	for(size_t i = 0; i < lines.size(); i++)
	{
		cv::Vec4i l = lines[i];
		cv::line(output, cv::Point(l[0], l[1]), cv::Point(l[2], l[3]), cv::Scalar(0,0,255), 3, CV_AA);
	}

	return output;
}

static void on_trackbar_change(int, void*)
{
	find_card(current_image, current_square, current_threshold);

	//Reset card number
	current_square = 0;
}

/**
  * Find and perspective transform a card in an image
  *
  * Arguments
  * cv::Mat input:		     Image matrix
  *
  * Returns
  * cv::Mat: A perspective-corrected image of a card
  */
cv::Mat find_card(cv::Mat input, int which_square, int threshold)
{
	cv::Mat found = input.clone();

	//Assign to global for use in callback
	current_image = input.clone();
	current_threshold = threshold;

	vector<vector<cv::Point> > squares;

	find_squares(found, squares, threshold);

	printf("%lu cards found.\n", squares.size());

	for(size_t i = 0; i < squares.size(); i++)
	{
		const cv::Point* p = &squares[i][0];
		int n = (int)squares[i].size();
		cv::polylines(found, &p, &n, 1, true, cv::Scalar(0,255,0), 1, CV_AA);
	}

	cv::imshow("Card Found", found);
	
	//Add threshold trackbar
	cv::createTrackbar("tb_thresh", "Card Found", &current_threshold, 255, on_trackbar_change);

	// Define the destination image
	cv::Mat quad = cv::Mat::zeros(350, 250, CV_8UC3);

	// Corners of the destination image
	std::vector<cv::Point2f> quad_pts;
	quad_pts.push_back(cv::Point2f(0, 0));
	quad_pts.push_back(cv::Point2f(quad.cols, 0));
	quad_pts.push_back(cv::Point2f(quad.cols, quad.rows));
	quad_pts.push_back(cv::Point2f(0, quad.rows));

	if(squares.size() > 0)
	{
		std::vector<cv::Point2f> corners;
		corners.push_back(cv::Point2f(squares[which_square][3].x, squares[which_square][3].y));
		corners.push_back(cv::Point2f(squares[which_square][2].x, squares[which_square][2].y));
		corners.push_back(cv::Point2f(squares[which_square][1].x, squares[which_square][1].y));
		corners.push_back(cv::Point2f(squares[which_square][0].x, squares[which_square][0].y));

		//Add trackbar if more than one to choose from
		if(squares.size() > 1)
		{
			cv::createTrackbar("tb_card", "Card Found", &current_square, squares.size(), on_trackbar_change);
		}

		// Get transformation matrix
		cv::Mat transmtx = cv::getPerspectiveTransform(corners, quad_pts);

		// Apply perspective transformation
		cv::warpPerspective(input, quad, transmtx, quad.size());

		cv::imshow("Perpective Transformed Card", quad);

		return quad;
		//return hough_trans(input);
	}
	else
	{
		cout << "NO CARDS TO TRANSFORM" << endl;
		return input;
	}
}