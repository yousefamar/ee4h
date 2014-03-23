/************************************************************************\
| Results file for EE4H Assignment										 |
|																		 |
| Authors: Yousef Amar and Chris Lewis									 |
| Last Modified: 05/02/2014												 |
|																		 |
\************************************************************************/

#include "../include/stdafx.h"

using namespace std;

int Results::results_window_count = 0;

/**
  * Initialise all the data items in the results
  */
void Results::init()
{
	detected_suit = UNKNOWN_SUIT;
	detected_colour = UNKNOWN_COLOUR;
	detected_value = -1;

	/*stringstream title_stream;
	title_stream << WINDOW_TITLE;
	cv::destroyWindow(title_stream.str());*/
}

/**
  * Show the results data
  */
void Results::show()
{
	//Finally
	stringstream title_stream;
	title_stream << WINDOW_TITLE;
	cv::imshow(title_stream.str(), as_mat());
}

void Results::show_with_card(cv::Mat card)
{
	cv::Mat results_mat = as_mat();

	cv::Mat final(card.rows + results_mat.rows, max(card.cols, results_mat.cols), CV_8UC3);

	card.copyTo(final(cv::Rect(0, 0, card.cols, card.rows)));
	results_mat.copyTo(final(cv::Rect(0, card.rows, results_mat.cols, results_mat.rows)));

	//Finally
	stringstream title_stream;
	title_stream << WINDOW_TITLE << ++Results::results_window_count;
	cv::imshow(title_stream.str(), final);
}

/**
 * NOTE: Assumes constant dims à la GridLayout
 */
void Results::show_cascade(vector<cv::Mat> cards)
{
	if (cards.size() < 1)
		return;

	// Max 8 cards per row; could put this elsewhere as a constant/define/static/global
	int cards_per_row = 8;

	for (size_t i = 0; i < cards.size(); ++i)
		cards[i] = as_mat_with_card(cards[i]);

	cv::Mat final((cards.size()/cards_per_row + 1) * cards[0].rows, min(cards.size() * cards[0].cols, cards_per_row * cards[0].cols), CV_8UC3);

	for (size_t i = 0; i < cards.size(); ++i)
		cards[i].copyTo(final(cv::Rect(cards[0].cols * (i%cards_per_row), cards[0].rows * (i/cards_per_row), cards[i].cols, cards[i].rows)));

	//Finally
	stringstream title_stream;
	title_stream << WINDOW_TITLE;
	cv::imshow(title_stream.str(), final);
}

cv::Mat Results::as_mat()
{
	//Create canvas
	cv::Mat canvas(window_height, window_width, CV_8UC3, cv::Scalar(255, 255, 255));

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

	//Detected value
	stringstream val_stream;
	val_stream << "Value: " << detected_value;
	cv::putText(canvas, val_stream.str(), cv::Point(10, 110), CV_FONT_HERSHEY_PLAIN, 2, cv::Scalar(0, 0, 0), 1, 8, false);

	return canvas;
}

cv::Mat Results::as_mat_with_card(cv::Mat card)
{
	cv::Mat results_mat = as_mat();
	cv::Mat final(card.rows + results_mat.rows, max(card.cols, results_mat.cols), CV_8UC3);
	card.copyTo(final(cv::Rect(0, 0, card.cols, card.rows)));
	results_mat.copyTo(final(cv::Rect(0, card.rows, results_mat.cols, results_mat.rows)));

	return final;
}