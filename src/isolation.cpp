/************************************************************************\
| Card isolation file for EE4H Assignment						         |
|																		 |
| Authors: Yousef Amar and Chris Lewis									 |
| Last Modified: 05/03/2014												 |
|																		 |
\************************************************************************/

#include "../include/stdafx.h"

using namespace std;

//Configuration, have as #defines instead?
float
	corner_h_perc = 0.15F,
	corner_v_perc = 0.25F
;

// NOTE: This is slight optimisation; it's unlikely that you'll be able to see any contours at low thresholds.
int thresh_lower = 200, thresh_upper = 255, thresh_increment = 2;

// The minimum difference between two contour squares (in pixels; risky) for them to be considered different contours.
int min_square_diff = 100;

/**
 * TODO: Docs
 */
int square_diff(vector<cv::Point> square1, vector<cv::Point> square2) {
	return dist_manhattan(square1[0].x, square2[0].x, square1[0].y, square2[0].y)
			+ dist_manhattan(square1[1].x, square2[1].x, square1[1].y, square2[1].y)
			+ dist_manhattan(square1[2].x, square2[2].x, square1[2].y, square2[2].y)
			+ dist_manhattan(square1[3].x, square2[3].x, square1[3].y, square2[3].y);
}

/*
 * Find the number of squares in an image.
 * 
 * Arguments
 *                cv::Mat image:     The image input
 * vector<vector<Point> >& squares:   The array of squares
 *                    int threshold: Threshold for finding a quad
 */
void find_squares(cv::Mat image, vector<vector<cv::Point> >& squares, int threshold)
{
	cv::Mat grey8(image.size(), CV_8U);
	cv::cvtColor(image, grey8, CV_BGR2GRAY);
	
	// Use other channels as the "grey" channel instead of converting to grey-scale
	//int ch[] = {1, 0};
	//cv::mixChannels(&image, 1, &grey8, 1, ch, 1);

	//cv::imshow("Grey", grey8);

	//cv::equalizeHist(grey8, grey8);
	// CLAHE (Contrast Limited Adaptive Histogram Equalization)
	// NOTE: Changing the thresh and size parameters has some interesting effects!
	cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE(2.0, cv::Size(32, 32));
	clahe->apply(grey8, grey8);
	//cv::imshow("Grey + Equalised Histogram", grey8);
	
	cv::blur(grey8, grey8, cv::Size(4, 4));
	//cv::imshow("Grey + Equalised Histogram + 4x4 Blur", grey8);

	cv::Mat grey = grey8 >= threshold;	//Seriously? ONE LINER!
	//cv::imshow("Grey + Equalised Histogram + 4x4 Blur + Threshold", grey);

	// find contours and store them all as a list
	vector<vector<cv::Point> > contours;
	cv::findContours(grey, contours, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);

	vector<cv::Point> approx;

	// test each contour
	for(size_t i = 0; i < contours.size(); i++)
	{
		// approximate contour with accuracy proportional
		// to the contour perimeter
		cv::approxPolyDP(cv::Mat(contours[i]), approx, cv::arcLength(cv::Mat(contours[i]), true)*0.02, true);

		/* Contour checks:
		 * - 4 corners
		 * - Area larger than 1000 pixels (approx. 32*32), fabs for negative areas (contour orientation) // FIXME: Dangerous
		 * - Convex
		 * - Is not as large as the whole image (75% of it max)
		 * //- Has a lot of whiteness inside (like a normal card would)
		 * - Is too similar to the previous (same contour)
		 * //- Is too similar to any other
		 * //- Angle between joint edges is larger than 45° // NOTE: squares.cpp has this
		 */

		int area = fabs(cv::contourArea(cv::Mat(approx)));

		if( approx.size() == 4 &&
			area > 1000 &&
			cv::isContourConvex(cv::Mat(approx)) &&
			area < (image.size().width * image.size().height * 0.75F) &&
			(squares.empty() || square_diff(squares.back(), approx) > min_square_diff))
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

/**
  * Find and perspective transform a card in an image
  *
  * Arguments
  * cv::Mat input:		     Image matrix
  *
  * Returns
  * cv::Mat: A perspective-corrected image of a card
  */
cv::Mat find_cards(cv::Mat input)
{
	vector<cv::Mat> cards;

	cv::Mat found = input.clone();
	vector<vector<cv::Point> > squares;

	for (int thresh = thresh_lower; thresh < thresh_upper; thresh += thresh_increment)
	{
		find_squares(found, squares, thresh);
	}

	Results results;
	results.init();

	printf("%lu cards found.\n", squares.size());

	for(size_t i = 0; i < squares.size(); i++)
	{
		const cv::Point* p = &squares[i][0];
		int n = (int)squares[i].size();
		cv::polylines(found, &p, &n, 1, true, cv::Scalar(0,255,0), 1, CV_AA);
	}

	cv::imshow("Card Found", found);

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
		// FIXME: 0 for now.
		corners.push_back(cv::Point2f(squares[0][3].x, squares[0][3].y));
		corners.push_back(cv::Point2f(squares[0][2].x, squares[0][2].y));
		corners.push_back(cv::Point2f(squares[0][1].x, squares[0][1].y));
		corners.push_back(cv::Point2f(squares[0][0].x, squares[0][0].y));

		// Get transformation matrix
		cv::Mat transmtx = cv::getPerspectiveTransform(corners, quad_pts);

		// Apply perspective transformation
		cv::warpPerspective(input, quad, transmtx, quad.size());

		/********************** Output and detection ***********************/

		cv::Size input_size = quad.size();

		//Get suit colour
		cv::Mat working = make_background_black(quad, 100);
		working = filter_red_channel(working, 0);
		results.detected_colour = is_red_suit_by_corners(working, corner_h_perc, corner_v_perc, 100, 2, 0.15F) == true ? Results::RED : Results::BLACK;

		//Get card value
		cv::Mat working_bin = binary_threshold(quad, 110, 0, 255);
		cv::imshow("Binary Threshold", working_bin);
		results.detected_value = count_blobs(working_bin) - 4;	// Count symbols, -4 for corners

		//Show regions searched on output window
		int region_width = (int) (corner_h_perc * (float) input_size.width);
		int region_height = (int) (corner_v_perc * (float) input_size.height);
		cv::Point start = cv::Point(0, 0);
		cv::Point finish = cv::Point(region_width, region_height);
		cv::rectangle(quad, start, finish, line_colour, 1, 8, 0);	//Top left
		start = cv::Point(input_size.width - region_width, input_size.height - region_height);
		finish = cv::Point(input_size.width, input_size.height);
		cv::rectangle(quad, start, finish, line_colour, 1, 8, 0);	//Bottom right
		cv::imshow("Perpective Transformed Card", quad);

		//Show results
		results.show();

		return working;	//NOT USED?
		//return hough_trans(input);
	}
	else
	{
		cout << "NO CARDS TO TRANSFORM" << endl;
		return input;
	}
}