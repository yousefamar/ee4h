/************************************************************************\
| Card file for EE4H Assignment											 |
|																		 |
| Authors: Yousef Amar and Chris Lewis									 |
|																		 |
\************************************************************************/

#include "../include/stdafx.h"

using namespace std;

// Initialise corner Rects
const cv::Rect Card::TOP_CORNER_RECT = cv::Rect(5, 5, 0.15F * Card::WIDTH - 5, 0.28F * Card::HEIGHT - 5);
const cv::Rect Card::BOTTOM_CORNER_RECT = cv::Rect(Card::WIDTH - 0.15F * Card::WIDTH - 5, Card::HEIGHT - 0.28F * Card::HEIGHT - 5, 0.15F * Card::WIDTH, 0.28F * Card::HEIGHT);
const int Card::CORNER_AREA = Card::TOP_CORNER_RECT.area();

// Initialise colours
const cv::Scalar Card::LINE_COLOUR = cv::Scalar(0, 255, 0), Card::LINE_COLOUR_ALT = cv::Scalar(255, 0, 0);
const cv::Scalar Card::TEXT_COLOUR = cv::Scalar(255, 255, 0);

//Initialise others
int Card::card_window_count = 0;

/*
 * Default constructor
 * Initialise all the data items in the results
 */
Card::Card()
{
	//Set default values
	detected_suit = UNKNOWN_SUIT;
	detected_colour = UNKNOWN_COLOUR;
	detected_value = -1;
	detected_rank = UNKNOWN_RANK;
}

/*
 * Constructor
 * Initialise as default, set cv::Mat
 *
 * Arguments:
 * cv::Mat mat: Input matrix to for this card
 */
Card::Card(cv::Mat mat)
{
	//Set default values
	Card();

	//Set main matrix
	set_mat(mat);
}

/*
 * Set the card's main matrix
 *
 * Arguments:
 * cv::Mat mat: Input matrix to for this card
 */
void Card::set_mat(cv::Mat mat)
{
	//Set
	this->mat = mat;

	//Generate greyscale matrix
	mat_clahe = cv::Mat(mat.size(), CV_8U);
	cv::cvtColor(mat, mat_clahe, CV_BGR2GRAY);
	
	//CLAHE (Contrast Limited Adaptive Histogram Equalization)
	cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE(2.0, cv::Size(8, 8));
	clahe->apply(mat_clahe, mat_clahe);
	cv::blur(mat_clahe, mat_clahe, cv::Size(4, 4));

	//Generate binary matrix
	mat_bin = mat_clahe >= 140;
}

/*
 * Show the results data
 */
void Card::show()
{
	//Finally
	stringstream title_stream;
	title_stream << WINDOW_TITLE;
	cv::imshow(title_stream.str(), results_to_mat());
}

/*
 * Show results with a card
 *
 * Arguments:
 * cv::Mat card: Card to use
 */
void Card::show_with_card(cv::Mat card)
{
	//Render out results matrix
	cv::Mat results_mat = results_to_mat();

	//Copy to main canvas
	cv::Mat final(card.rows + results_mat.rows, max(card.cols, results_mat.cols), CV_8UC3);
	card.copyTo(final(cv::Rect(0, 0, card.cols, card.rows)));
	results_mat.copyTo(final(cv::Rect(0, card.rows, results_mat.cols, results_mat.rows)));

	//Finally, show
	stringstream title_stream;
	title_stream << WINDOW_TITLE << ++Card::card_window_count;
	cv::imshow(title_stream.str(), final);
}

/*
 * Convert results information to a matrix
 *
 * Returns:
 * cv::Mat: Image matrix of results information
 */
cv::Mat Card::results_to_mat()
{
	//Create canvas
	cv::Mat canvas(window_height + 20, window_width + 10, CV_8UC3, cv::Scalar(255, 255, 255));

	//Title
	cv::putText(canvas, "Results:", cv::Point(10, 35), CV_FONT_HERSHEY_PLAIN, 3, cv::Scalar(0, 0, 0), 1, 8, false);
	
	//Render colour
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

	//Render suit
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

	// Small hack; equivalent of != null check in Java
	if (mat_sym.size().width)
	{
		mat_sym.copyTo(canvas(cv::Rect(window_width - mat_sym.size().width, window_height - mat_sym.size().height, mat_sym.size().width, mat_sym.size().height)));
	}
	if (mat_rank.size().width)
	{
		mat_rank.copyTo(canvas(cv::Rect(window_width - mat_rank.size().width - mat_sym.size().width, window_height - mat_rank.size().height, mat_rank.size().width, mat_rank.size().height)));
	}

	//Render detected value
	stringstream val_stream;
	val_stream << "Value: " << detected_value;
	cv::putText(canvas, val_stream.str(), cv::Point(10, 110), CV_FONT_HERSHEY_PLAIN, 2, cv::Scalar(0, 0, 0), 1, 8, false);

	//Detected picture card?
	if(is_picture_card == true)
	{
		switch(detected_rank) {
		case RANK_JACK:
			cv::putText(canvas, "Rank: JACK", cv::Point(10, 135), CV_FONT_HERSHEY_PLAIN, 2, cv::Scalar(0, 0, 0), 1, 8, false);
			break;
		case RANK_QUEEN:
			cv::putText(canvas, "Rank: QUEEN", cv::Point(10, 135), CV_FONT_HERSHEY_PLAIN, 2, cv::Scalar(0, 0, 0), 1, 8, false);
			break;
		case RANK_KING:
			cv::putText(canvas, "Rank: KING", cv::Point(10, 135), CV_FONT_HERSHEY_PLAIN, 2, cv::Scalar(0, 0, 0), 1, 8, false);
			break;
		case RANK_ACE:
			cv::putText(canvas, "Rank: ACE", cv::Point(10, 135), CV_FONT_HERSHEY_PLAIN, 2, cv::Scalar(0, 0, 0), 1, 8, false);
			break;
		default:
			cv::putText(canvas, "Rank: UNKNOWN", cv::Point(10, 135), CV_FONT_HERSHEY_PLAIN, 2, cv::Scalar(0, 0, 0), 1, 8, false);
			break;
		}
	}
	else
	{
		cv::putText(canvas, "Rank: N/A", cv::Point(10, 130), CV_FONT_HERSHEY_PLAIN, 2, cv::Scalar(0, 0, 0), 1, 8, false);
	}

	//Finally
	return canvas;
}

/* 
 * Show card as a results matrix
 *
 * Returns:
 * cv::Mat: Rendered card with results
 */
cv::Mat Card::as_mat_with_results()
{
	cv::Mat results_mat = results_to_mat();
	cv::Mat final(mat.rows + results_mat.rows, max(mat.cols, results_mat.cols), CV_8UC3);
	mat.copyTo(final(cv::Rect(0, 0, mat.cols, mat.rows)));
	results_mat.copyTo(final(cv::Rect(0, mat.rows, results_mat.cols, results_mat.rows)));

	//Show regions searched on output window
	cv::rectangle(final, Card::TOP_CORNER_RECT, Card::LINE_COLOUR);
	cv::rectangle(final, Card::BOTTOM_CORNER_RECT, Card::LINE_COLOUR);
	cv::rectangle(final, _last_aabb, Card::LINE_COLOUR_ALT);
	cv::rectangle(final, _rank_aabb, Card::LINE_COLOUR_ALT);

	return final;
}

/*
 * Show all cards as a cascade
 *
 * Arguments:
 * vector<Card> cards: Vector of all cards
 */
void show_cascade(vector<Card> cards)
{
	if (cards.size() < 1)
		return;

	// Max 6 cards per row; could put this elsewhere as a constant/define/static/global
	int cards_per_row = 6;

	for (size_t i = 0; i < cards.size(); ++i)
		cards[i].mat = cards[i].as_mat_with_results();

	cv::Mat final(((cards.size() - 1)/cards_per_row + 1) * cards[0].mat.rows, min(cards.size() * cards[0].mat.cols, cards_per_row * cards[0].mat.cols), CV_8UC3, cv::Scalar(0, 0, 0));

	for (size_t i = 0; i < cards.size(); ++i)
		cards[i].mat.copyTo(final(cv::Rect(cards[i].mat.cols * (i%cards_per_row), cards[i].mat.rows * (i/cards_per_row), cards[i].mat.cols, cards[i].mat.rows)));

	//Finally
	stringstream title_stream;
	title_stream << WINDOW_TITLE;
	cv::imshow(title_stream.str(), final);

	//Nice CMD summary
	cout << endl << "============== Detected Cards ==============" << endl << endl;

	for(size_t i = 0; i < cards.size(); i++)
	{
		cout << "Card " << (i + 1) << ": The ";

		if(cards[i].is_picture_card)
		{
			switch(cards[i].detected_rank)
			{
			case Card::RANK_JACK:
				cout << "Jack";
				break;
			case Card::RANK_QUEEN:
				cout << "Queen";
				break;
			case Card::RANK_KING:
				cout << "King";
				break;
			case Card::RANK_ACE:
				cout << "Ace";
				break;
			default:
				cout << "Something";
				break;
			}
		}
		else
		{
			cout << cards[i].detected_value;
		}

		cout << " of ";

		switch(cards[i].detected_suit)
		{
		case Card::CLUBS:
			cout << "Clubs";
			break;
		case Card::DIAMONDS:
			cout << "Diamonds";
			break;
		case Card::HEARTS:
			cout << "Hearts";
			break;
		case Card::SPADES:
			cout << "Spades";
			break;
		default:
			cout << "Something";
			break;
		}

		cout << endl;
	}

	cout << endl << "============================================" << endl << endl;
}