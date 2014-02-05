/************************************************************************\
| Results file for EE4H Assignment										 |
|																		 |
| Authors: Yousef Amar and Chris Lewis									 |
| Last Modified: 05/02/2014												 |
|																		 |
\************************************************************************/

#include "../include/stdafx.h"

using namespace std;

/**
  * Initialise all the data items in the results
  */
void Results::init()
{
	detected_suit = UNKNOWN_SUIT;
	detected_colour = UNKNOWN_COLOUR;
	detected_value = -1;
}

/**
  * Show the results data
  */
void Results::show()
{
	//Create canvas
	cv::Mat canvas(window_width, window_height, CV_8UC3, cv::Scalar(255, 255, 255));

	//Title
	cv::putText(canvas, "Results:", cv::Point(10, 35), CV_FONT_HERSHEY_PLAIN, 3, cv::Scalar(0, 0, 0), 1, 8, false);
	
	//Colour
	switch(detected_colour) {
	case RED:
		cv::putText(canvas, "Colour: RED", cv::Point(10, 60), CV_FONT_HERSHEY_PLAIN, 2, cv::Scalar(0, 0, 0), 1, 8, false);
		break;
	case BLACK:
		cv::putText(canvas, "Colour: BLACK", cv::Point(10, 60), CV_FONT_HERSHEY_PLAIN, 2, cv::Scalar(0, 0, 0), 1, 8, false);
		break;
	default:
		cv::putText(canvas, "Colour: UNKNOWN", cv::Point(10, 60), CV_FONT_HERSHEY_PLAIN, 2, cv::Scalar(0, 0, 0), 1, 8, false);
		break;
	}

	//Suit
	switch(detected_suit) {
	case CLUBS:
		cv::putText(canvas, "Suit: CLUBS", cv::Point(10, 85), CV_FONT_HERSHEY_PLAIN, 2, cv::Scalar(0, 0, 0), 1, 8, false);
		break;
	case DIAMONDS:
		cv::putText(canvas, "Suit: DIAMONDS", cv::Point(10, 85), CV_FONT_HERSHEY_PLAIN, 2, cv::Scalar(0, 0, 0), 1, 8, false);
		break;
	case HEARTS:
		cv::putText(canvas, "Suit: HEARTS", cv::Point(10, 85), CV_FONT_HERSHEY_PLAIN, 2, cv::Scalar(0, 0, 0), 1, 8, false);
		break;
	case SPADES:
		cv::putText(canvas, "Suit: SPADES", cv::Point(10, 85), CV_FONT_HERSHEY_PLAIN, 2, cv::Scalar(0, 0, 0), 1, 8, false);
		break;
	default:
		cv::putText(canvas, "Suit: UNKNOWN", cv::Point(10, 85), CV_FONT_HERSHEY_PLAIN, 2, cv::Scalar(0, 0, 0), 1, 8, false);
		break;
	}

	//Finally
	stringstream s;
	s << WINDOW_TITLE;
	cv::imshow(s.str(), canvas);
}