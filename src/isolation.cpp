/************************************************************************\
| Card isolation file for EE4H Assignment						         |
|																		 |
| Authors: Yousef Amar and Chris Lewis									 |
|																		 |
\************************************************************************/

#include "../include/stdafx.h"

using namespace std;

//Configuration, have as #defines instead?
int thresh_lower = 150, thresh_upper = 255, thresh_increment = 2;
float min_square_diff = 0.1F;

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
 * vector<vector<Point>>& squares:   The array of squares
 *                    int threshold: Threshold for finding a quad
 */
void find_squares(cv::Mat image, vector<vector<cv::Point> >& squares, int threshold)
{
	cv::Size image_size = image.size();

	cv::Mat grey8(image_size, CV_8U);
	cv::cvtColor(image, grey8, CV_BGR2GRAY);
	
	//CLAHE (Contrast Limited Adaptive Histogram Equalization)
	cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE(2.0, cv::Size(32, 32));
	clahe->apply(grey8, grey8);
	cv::blur(grey8, grey8, cv::Size(4, 4));

	cv::Mat grey = grey8 >= threshold;

	//Find contours and store them all as a list
	vector<vector<cv::Point> > contours;
	cv::findContours(grey.clone(), contours, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);

	vector<cv::Point> approx;
	bool is_duplicate_contour = false;
	int min_square_diff_rel = (image_size.width + image_size.height) * 0.5F * min_square_diff;

	// test each contour
	for(size_t i = 0; i < contours.size(); i++)
	{
		// approximate contour
		cv::approxPolyDP(cv::Mat(contours[i]), approx, cv::arcLength(cv::Mat(contours[i]), true)*0.02, true);

		int area = fabs(cv::contourArea(cv::Mat(approx)));

		if( approx.size() == 4 &&	//4 corners
			area > (image_size.width * image_size.height * 0.04F) &&	//Area larger than 0.04 of the image in pixels
			cv::isContourConvex(cv::Mat(approx)) &&	//Is convex
			area < (image_size.width * image_size.height * 0.75F))	//Is not as large as the whole image (75% of it max)
		{
			is_duplicate_contour = false;	
			if (!squares.empty())
			{
				for (size_t j = 0; j < squares.size(); ++j)
				{
					//Is too similar to previous ones (same contour)
					if (square_diff(squares[j], approx) < min_square_diff_rel)
					{
						is_duplicate_contour = true;
						break;
					}
				}	
			}
			
			if(!is_duplicate_contour)
			{
				squares.push_back(approx);
			}
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
  * Find and perspective transform cards in an image
  *
  * Arguments
  * cv::Mat input:	Image matrix
  * cv::Mat cards:	Output card Mats
  *
  */
void find_cards(cv::Mat input, vector<cv::Mat>* cards)
{
	cv::Mat found = input.clone();
	vector<vector<cv::Point> > squares;

	for (int thresh = thresh_lower; thresh < thresh_upper; thresh += thresh_increment)
	{
		find_squares(found, squares, thresh);
	}

	printf("%lu cards found.\n", squares.size());

	if(squares.size() < 1)
		throw 1;

	for(size_t i = 0; i < squares.size(); i++)
	{
		const cv::Point* p = &squares[i][0];
		int n = (int)squares[i].size();
		cv::polylines(found, &p, &n, 1, true, cv::Scalar(0,255,0), 1, CV_AA);
	}

	cv::imshow("Cards Found", found);

	std::vector<cv::Point2f> quad_pts;
	std::vector<cv::Point2f> corners;

	for (int i = 0; i < squares.size(); ++i)
	{
		// Define the destination image
		cv::Mat quad = cv::Mat::zeros(350, 250, CV_8UC3);

		// Corners of the destination image
		quad_pts.clear();
		quad_pts.push_back(cv::Point2f(0, 0));
		quad_pts.push_back(cv::Point2f(quad.cols, 0));
		quad_pts.push_back(cv::Point2f(quad.cols, quad.rows));
		quad_pts.push_back(cv::Point2f(0, quad.rows));

		corners.clear();
		corners.push_back(cv::Point2f(squares[i][3].x, squares[i][3].y));
		corners.push_back(cv::Point2f(squares[i][2].x, squares[i][2].y));
		corners.push_back(cv::Point2f(squares[i][1].x, squares[i][1].y));
		corners.push_back(cv::Point2f(squares[i][0].x, squares[i][0].y));

		// Get transformation matrix
		cv::Mat transmtx = cv::getPerspectiveTransform(corners, quad_pts);

		// Apply perspective transformation
		cv::warpPerspective(input, quad, transmtx, quad.size());

		//stringstream s;
		//s << "Perpective Transformed Card " << i;
		//cv::imshow(s.str(), quad);
		
		cards->push_back(quad);
	}
}