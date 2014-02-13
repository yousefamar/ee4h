/************************************************************************\
| Card isolation file for EE4H Assignment						         |
|																		 |
| Authors: Yousef Amar and Chris Lewis									 |
| Last Modified: 12/02/2014												 |
|																		 |
\************************************************************************/

#include "../include/stdafx.h"

using namespace std;

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
	return hough_trans(input);
}