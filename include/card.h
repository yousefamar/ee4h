//Card file to show output that can be global

#include "../include/stdafx.h"

//Include guard
#ifndef CARD_H
#define CARD_H

//Configuration
#define window_width 250
#define window_height 120

#define line_colour cv::Scalar(0, 255, 0)
#define text_colour cv::Scalar(255, 255, 0)

//Constants
#define WINDOW_TITLE "Processing Card"

class Card {
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

	static int card_window_count;

	//Data
	cv::Mat mat, mat_clahe, mat_bin;
	Suit detected_suit;
	Colour detected_colour;
	int detected_value;

	//Constructor declarations
	Card();
	Card(cv::Mat);

	//Methods
	void set_mat(cv::Mat mat);
	void init();
	void show();
	void show_with_card(cv::Mat card);
	cv::Mat results_to_mat();
	cv::Mat as_mat_with_results();
};

void show_cascade(std::vector<Card> cards);

#endif