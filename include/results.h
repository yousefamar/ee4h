//Results file to show output that can be global

#include "../include/stdafx.h"

//Include guard
#ifndef RESULTS_H
#define RESULTS_H

//Configuration
#define window_width 200
#define window_height 300

#define line_colour cv::Scalar(0, 255, 0)
#define text_colour cv::Scalar(255, 255, 0)

//Constants
#define WINDOW_TITLE "Processing Results";

class Results {
public:
	//Constants - Maybe not the best place
	enum Suit {
		CLUBS,
		DIAMONDS,
		HEARTS,
		SPADES,
		UNKNOWN_SUIT
	};

	enum Colour {
		BLACK,
		RED,
		UNKNOWN_COLOUR
	};

	//Data
	Suit detected_suit;
	Colour detected_colour;
	int detected_value;

	//Methods
	void init();
	void show();
};

#endif