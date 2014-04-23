/************************************************************************\
| Card isolation file for EE4H Assignment						         |
|																		 |
| Authors: Yousef Amar and Chris Lewis									 |
|																		 |
\************************************************************************/

#include "../include/stdafx.h"

using namespace std;

//Improves CLAHE performance
bool multi_mode = false;

//Configuration
int thresh_lower = 150, thresh_upper = 255, thresh_increment = 2;
float min_square_diff = 0.1F;

/*
 * Square diff operation
 *
 * Arguments:
 * vector<cv::Point> square1: First square
 * vector<cv::Point> square2: Second square
 *
 * Returns:
 * int: Manhattan distance between the two squares
 */
int square_diff(vector<cv::Point> square1, vector<cv::Point> square2)
{
	return dist_manhattan(square1[0].x, square2[0].x, square1[0].y, square2[0].y)
			+ dist_manhattan(square1[1].x, square2[1].x, square1[1].y, square2[1].y)
			+ dist_manhattan(square1[2].x, square2[2].x, square1[2].y, square2[2].y)
			+ dist_manhattan(square1[3].x, square2[3].x, square1[3].y, square2[3].y);
}

/*
 * Points in triangle operation
 * Credit to http://stackoverflow.com/questions/2049582/how-to-determine-a-point-in-a-triangle for the algorithm
 *
 * Arguments:
 * cv::Point p: Point
 * cv::Point p0: Triangle point 0
 * cv::Point p1: Triangle point 1
 * cv::Point p2: Triangle point 2
 */
bool point_in_triangle(cv::Point p, cv::Point p0, cv::Point p1, cv::Point p2)
{
	float area = 0.5F * (-p1.y * p2.x + p0.y * (-p1.x + p2.x) + p0.x * (p1.y - p2.y) + p1.x * p2.y);
	float sign = area < 0.0F ? -1.0F : 1.0F;
	float s = (p0.y * p2.x - p0.x * p2.y + (p2.y - p0.y) * p.x + (p0.x - p2.x) * p.y) * sign;
	float t = (p0.x * p1.y - p0.y * p1.x + (p0.y - p1.y) * p.x + (p1.x - p0.x) * p.y) * sign;

	return s > 0.0F && t > 0.0F && (s + t) < 2.0F * area * sign;
}

/*
 * Point in quad operation
 * (See above)
 * 
 * Arguments:
 *         cv::Point p: Point
 * vector<cv::Point> q: Vector of points in quad
 */
bool point_in_quad(cv::Point p, vector<cv::Point> q) {
	return point_in_triangle(p, q[0], q[1], q[2]) || point_in_triangle(p, q[2], q[3], q[0]);
}

/*
 * Quad in quad operation
 * (See above)
 * 
 * Arguments:
 * vector<cv::Point> inner: Inner quad
 * vector<cv::Point> outer: Outer quad
 */
bool quad_in_quad(vector<cv::Point> inner, vector<cv::Point> outer) {
	int vertexCount = 0;

	for (int i = 0; i < 4; ++i)
	{
		if (point_in_quad(inner[i], outer))
		{
			vertexCount++;
		}
	}

	return vertexCount == 4;
}

/*
 * Find the number of squares in an image.
 * 
 * Arguments
 *                cv::Mat     image:     The image input
 * vector<vector<Point>>&   squares:   The array of squares
 *                    int threshold: Threshold for finding a quad
 */
void find_squares(cv::Mat image, vector<vector<cv::Point> >& squares, int threshold)
{
	cv::Size image_size = image.size();

	cv::Mat grey8(image_size, CV_8U);
	cv::cvtColor(image, grey8, CV_BGR2GRAY);

	int clahe_size = (image_size.width + image_size.height)>>8;
	//cout << clahe_size << endl;

	//CLAHE (Contrast Limited Adaptive Histogram Equalization)
	cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE(2.0, multi_mode?cv::Size(28, 28):cv::Size(32, 32));
	clahe->apply(grey8, grey8);
	//cv::equalizeHist(grey8, grey8);
	if (clahe_size)
		cv::blur(grey8, grey8, cv::Size(clahe_size, clahe_size));
	
	static int i = 0;
	stringstream s;
	s << "CLAHE " << i++;
	if (i == 52)
		cv::imshow(s.str(), grey8);
	
	cv::Mat grey = grey8 >= threshold;

	//Find contours and store them all as a list
	vector<vector<cv::Point> > contours;
	cv::findContours(grey.clone(), contours, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);

	vector<cv::Point> approx;
	bool is_duplicate_contour = false;
	bool is_inside_another = false;
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
			is_inside_another = false;
			if (!squares.empty())
			{
				for (size_t j = 0; j < squares.size(); ++j)
				{
					//Is too similar to previous ones (same contour)
					for (int i = 0; i < 4; ++i)
					{
						vector<cv::Point> square = squares[j];
						rotate(&square[0], &square[i], &square[4]);
						if (square_diff(square, approx) < min_square_diff_rel)
						{
							is_duplicate_contour = true;
							break;
						}
					}
					if (is_duplicate_contour)
						break;
					
					if (quad_in_quad(approx, squares[j])) {
						is_inside_another = true;
						break;
					}
					if (quad_in_quad(squares[j], approx)) {
						squares[j] = approx;
						is_inside_another = true;
						break;
					}
				}	
			}
			
			if(!is_duplicate_contour && !is_inside_another)
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
int find_cards(cv::Mat input, vector<Card>* cards)
{
	cv::Mat found = input.clone();
	vector<vector<cv::Point> > squares;

	for (int thresh = thresh_lower; thresh < thresh_upper; thresh += thresh_increment)
	{
		find_squares(found, squares, thresh);
	}

	if(squares.size() < 1)
		return 1; // No cards found

	std::vector<cv::Point2f> quad_pts;
	std::vector<cv::Point2f> corners;

	for (int i = 0; i < squares.size(); ++i)
	{
		// Define the destination image
		cv::Mat quad = cv::Mat::zeros(Card::HEIGHT, Card::WIDTH, CV_8UC3);

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

		Card card(quad);

		// Check whiteness of whole image
		float whiteness = (float) cv::countNonZero(card.mat_bin) / Card::AREA;

		//printf("Whiteness in card %d: %.2f\n", i, whiteness);
		
		if (whiteness < 0.5F)
			continue;

		// Check whiteness of both corners
		whiteness = ((float) cv::countNonZero(card.mat_bin(Card::TOP_CORNER_RECT)) + (float) cv::countNonZero(card.mat_bin(Card::BOTTOM_CORNER_RECT))) / (Card::CORNER_AREA<<1);
		
		printf("Whiteness in card %d corners: %.2f\n", i, whiteness);

		if (whiteness > 0.8F)
		{
			cv::Mat quad_rot;
			rotate(&corners[0], &corners[1], &corners[4]);
			cv::Mat transmtx = cv::getPerspectiveTransform(corners, quad_pts);
			cv::warpPerspective(input, quad_rot, transmtx, quad.size());

			Card card_rot(quad_rot);

			// Compare against old corner whiteness
			if (whiteness >= ((float) cv::countNonZero(card_rot.mat_bin(Card::TOP_CORNER_RECT)) + (float) cv::countNonZero(card_rot.mat_bin(Card::BOTTOM_CORNER_RECT))) / (Card::CORNER_AREA<<1)) {
				printf("Card %d orientation correction overridden due to new corners being whiter\n", i);
				card = card_rot;
			}
		}

		// Draw contours
		const cv::Point* p = &squares[i][0];
		int n = (int)squares[i].size();
		cv::polylines(found, &p, &n, 1, true, cv::Scalar(0,255,0), 1, CV_AA);
		
		cards->push_back(card);
	}

	printf("%lu cards found.\n", cards->size());

	cv::imshow("Cards Found", found);

	return 0;
}

/*
 * Perform crop by XOR
 *
 * Arguments:
 * cv::Mat mat1: First matrix
 * cv::Mat mat2: Second matrix
 *
 * Returns:
 * cv::Rect: Rect of cropped area
 */
cv::Rect xor_crop(cv::Mat mat1, cv::Mat mat2)
{
	cv::Mat xord = mat1 ^ mat2;
	
	std::vector<cv::Point> points;

	uchar *in_data = (uchar*)xord.data;
	for(int y = 0; y < Card::TOP_CORNER_RECT.height; y++)
	{
		for(int x = 0; x < Card::TOP_CORNER_RECT.width; x++)
		{
			if(in_data[y*xord.step + x])
			{
				points.push_back(cv::Point(x, y));
			}
		}
	}

	return cv::boundingRect(points);
}

/*
 * Find the rank and suit symbols and isolate to matrices
 *
 * Arguments:
 * Card* card: Pointer to card matrix to examine
 */
void find_symbols(Card *card) {
	std::vector<cv::Mat> blobs;

	cv::Mat card_bin = card->mat_bin.clone();

	card_bin = binary_operation(card_bin, MODE_BINARY_EROSION, 3);

	cv::Rect last_aabb;

	bool blob_found = false;

	//For each pixel...
	uchar *in_data = (uchar*)card_bin.data;
	for(int y = Card::TOP_CORNER_RECT.y; y < Card::TOP_CORNER_RECT.height; y++)
	{
		for(int x = Card::TOP_CORNER_RECT.x; x < Card::TOP_CORNER_RECT.width; x++)
		{
			// Count blob then remove by flood-filling it out
			if(!in_data[y*card_bin.step + x])
			{
				cv::Mat clone = card_bin.clone();
				cv::floodFill(card_bin, cv::Point(x, y), cv::Scalar(1));

				card->_last_aabb = xor_crop(card_bin, clone);

				if (!blob_found)
				{
					card->_rank_aabb = card->_last_aabb;
					card->mat_rank = card->mat(card->_rank_aabb);
					blob_found = true;
				}
			}
		}
	}
	
	if (blob_found)
		card->mat_sym = card->mat(card->_last_aabb);
}