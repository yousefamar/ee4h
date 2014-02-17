/************************************************************************\
| Card isolation file for EE4H Assignment						         |
|																		 |
| Authors: Yousef Amar and Chris Lewis									 |
| Last Modified: 17/02/2014												 |
|																		 |
\************************************************************************/

#include "../include/stdafx.h"

using namespace std;
using namespace cv;

int N = 255;

void findSquares(Mat image, vector<vector<Point> >& squares)
{
	squares.clear();

	Mat timg, gray0(image.size(), CV_8U), gray;
	blur(image, timg, cv::Size(4, 4));
	cv::cvtColor(timg, gray0, CV_BGR2GRAY);

	cv::imshow("Contours", gray0);

	vector<vector<Point> > contours;

	// try several threshold levels
	for(int l = 0; l < N; l++)
	{
		// tgray(x,y) = gray(x,y) < (l+1)*255/N ? 255 : 0
		gray = gray0 >= (l+1)*255/N;

		// find contours and store them all as a list
		findContours(gray, contours, RETR_LIST, CHAIN_APPROX_SIMPLE);

		vector<Point> approx;

		// test each contour
		for(size_t i = 0; i < contours.size(); i++)
		{
			// approximate contour with accuracy proportional
			// to the contour perimeter
			approxPolyDP(Mat(contours[i]), approx, arcLength(Mat(contours[i]), true)*0.02, true);

			// square contours should have 4 vertices after approximation
			// relatively large area (to filter out noisy contours)
			// and be convex.
			// Note: absolute value of an area is used because
			// area may be positive or negative - in accordance with the
			// contour orientation
			if( approx.size() == 4 &&
				fabs(contourArea(Mat(approx))) > 1000 &&
				isContourConvex(Mat(approx)) )
			{

				squares.push_back(approx);
			}
		}
	}
}

cv::Mat hough_trans(cv::Mat input)
{
	cv::Mat edges, output;
	cv::Canny(input, edges, 50, 200, 3);

	cv::cvtColor(edges, output, cv::COLOR_GRAY2BGR);

	vector<cv::Vec4i> lines;
	cv::HoughLinesP(edges, lines, 1, CV_PI/180, 50, 50, 10 );
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
cv::Mat find_card(cv::Mat input)
{
	Mat found = input.clone();

	vector<vector<Point> > squares;

	findSquares(found, squares);

	printf("%lu cards found.\n", squares.size());

	for(size_t i = 0; i < squares.size(); i++)
	{
		const Point* p = &squares[i][0];
		int n = (int)squares[i].size();
		polylines(found, &p, &n, 1, true, Scalar(0,255,0), 3, CV_AA);
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

	std::vector<cv::Point2f> corners;
	corners.push_back(cv::Point2f(squares[0][3].x, squares[0][3].y));
	corners.push_back(cv::Point2f(squares[0][2].x, squares[0][2].y));
	corners.push_back(cv::Point2f(squares[0][1].x, squares[0][1].y));
	corners.push_back(cv::Point2f(squares[0][0].x, squares[0][0].y));

	// Get transformation matrix
	cv::Mat transmtx = cv::getPerspectiveTransform(corners, quad_pts);

	// Apply perspective transformation
	cv::warpPerspective(input, quad, transmtx, quad.size());

	return quad;
	//return hough_trans(input);
}