/************************************************************************\
| Card isolation file for EE4H Assignment						         |
|																		 |
| Authors: Yousef Amar and Chris Lewis									 |
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
int thresh_lower = 150, thresh_upper = 255, thresh_increment = 2;

// The minimum difference between two contour squares (factor to mean of width + height) for them to be considered different contours.
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
 * vector<vector<Point> >& squares:   The array of squares
 *                    int threshold: Threshold for finding a quad
 */
void find_squares(cv::Mat image, vector<vector<cv::Point> >& squares, int threshold)
{
	cv::Size image_size = image.size();

	cv::Mat grey8(image_size, CV_8U);
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
	cv::findContours(grey.clone(), contours, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);

	vector<cv::Point> approx;

	bool is_duplicate_contour = false;
	int min_square_diff_rel = (image_size.width + image_size.height) * 0.5F * min_square_diff;

	// test each contour
	for(size_t i = 0; i < contours.size(); i++)
	{
		// approximate contour with accuracy proportional
		// to the contour perimeter
		cv::approxPolyDP(cv::Mat(contours[i]), approx, cv::arcLength(cv::Mat(contours[i]), true)*0.02, true);

		/* Contour checks:
		 * - 4 corners
		 * - Area larger than 0.04 of the image in pixels, fabs for negative areas (contour orientation)
		 * - Convex
		 * - Is not as large as the whole image (75% of it max)
		 * //- Has a lot of whiteness inside (like a normal card would)
		 * - Is too similar to previous ones (same contour)
		 * //- Angle between joint edges is larger than 45° // NOTE: squares.cpp has this
		 */

		int area = fabs(cv::contourArea(cv::Mat(approx)));

		if( approx.size() == 4 &&
			area > (image_size.width * image_size.height * 0.04F) &&
			cv::isContourConvex(cv::Mat(approx)) &&
			area < (image_size.width * image_size.height * 0.75F))
		{
			is_duplicate_contour = false;
			
			if (!squares.empty())
			{
				// NOTE: Warning O(N) complexity on top of complexity of high-res images
				for (size_t j = 0; j < squares.size(); ++j)
				{
					if (square_diff(squares[j], approx) < min_square_diff_rel)
					{
						is_duplicate_contour = true;
						break;
					}
				}	
			}
			
			if (!is_duplicate_contour)
			{
				squares.push_back(approx);
				//ostringstream s;
				//s << "Thresh = " << threshold << ", Contour #" << i;
				//cv::imshow(s.str(), grey);
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
	cv::Mat found = input.clone();
	vector<vector<cv::Point> > squares;

	for (int thresh = thresh_lower; thresh < thresh_upper; thresh += thresh_increment)
	{
		find_squares(found, squares, thresh);
	}

	printf("%lu cards found.\n", squares.size());

	if(squares.size() < 1)
		return input;

	for(size_t i = 0; i < squares.size(); i++)
	{
		const cv::Point* p = &squares[i][0];
		int n = (int)squares[i].size();
		cv::polylines(found, &p, &n, 1, true, cv::Scalar(0,255,0), 1, CV_AA);
	}

	cv::imshow("Cards Found", found);

	Results results;
	results.init();

	vector<cv::Mat> cards;
	
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

		/********************** Output and detection ***********************/

		cv::Size input_size = quad.size();

		//Get suit colour
		cv::Mat working = make_background_black(quad, 100);
		working = filter_red_channel(working, 0);
		results.detected_colour = is_red_suit_by_corners(working, corner_h_perc, corner_v_perc, 100, 2, 0.15F) == true ? Results::RED : Results::BLACK;

		//Get card value
		cv::Mat working_bin = binary_threshold(quad, 110, 0, 255);
		cv::imshow("Binary Threshold", working_bin);
		results.detected_value = count_blobs(working_bin) - 4;	//Count symbols, -4 for corners

		//Try and find suit
		if(i == 0)
		{
			int suit = find_suit(working_bin, 0.8F);
		} //nasty hack for testing with pers1.jpg!
		//switch(suit)
		//{
		//case CLUB:
		//	results.detected_suit = Results.Suit.CLUBS;	//Make all in terms of #defined as enum can't be prototype return type!
		//	break;
		//case DIAMOND:
		//	results.detected_suit = Results.Suit.DIAMONDS;	
		//	break;
		//case HEART:
		//	results.detected_suit = Results.Suit.HEARTS;	
		//	break;
		//case SPADE:
		//	results.detected_suit = Results.Suit.SPADES;	
		//	break;
		//default:
		//	//Init'd to UNKNOWN
		//	break;
		//}

		//Show regions searched on output window
		int region_width = (int) (corner_h_perc * (float) input_size.width);
		int region_height = (int) (corner_v_perc * (float) input_size.height);
		cv::Point start = cv::Point(0, 0);
		cv::Point finish = cv::Point(region_width, region_height);
		cv::rectangle(quad, start, finish, line_colour, 1, 8, 0);	//Top left
		start = cv::Point(input_size.width - region_width, input_size.height - region_height);
		finish = cv::Point(input_size.width, input_size.height);
		cv::rectangle(quad, start, finish, line_colour, 1, 8, 0);	//Bottom right

		//stringstream s;
		//s << "Perpective Transformed Card " << i;
		//cv::imshow(s.str(), quad);
		
		cards.push_back(quad);
	}

	results.show_cascade(cards);

	//Show results
	//results.show_with_card(quad);

	return input;
	//return hough_trans(input);
}