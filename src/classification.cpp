/************************************************************************\
| Classification file for EE4H Assignment						         |
|																		 |
| Authors: Yousef Amar and Chris Lewis									 |
|																		 |
\************************************************************************/

#include "../include/stdafx.h"

using namespace std;

cv::Mat canny_thresh(cv::Mat input, int low_thresh, int ratio, int kernel_size)
{
	cv::Mat grey, edges;

	//Get size
	cv::Size input_size = input.size();

	//Create output
	cv::Mat output(input_size.height, input_size.width, CV_8UC3);

	//Convert to grey-scale
	cv::cvtColor(input, grey, CV_BGR2GRAY);

	// Reduce noise
	cv::blur(grey, edges, cv::Size(kernel_size, kernel_size));

	// Canny detector
	cv::Canny(edges, edges, low_thresh, low_thresh*ratio, kernel_size);

	// Using Canny's output as a mask, we display our result

	cv::Scalar::all(0);
	input.copyTo(output, edges);

	return output;
}

cv::Mat morph_gradient(cv::Mat input)
{
	cv::Mat output;

	cv::Mat element = getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3), cv::Point(1,1));  
	cv::morphologyEx(input, output, cv::MORPH_GRADIENT, element);

	return output;
}

int count_blobs(cv::Mat input)
{
	if (input.channels() > 1)
	{
		cerr << "Error: Cannot count blobs on an image with more than 1 channel." << endl;
		return -1;
	}

	int count = 0;
	input = input.clone();

	//Get size
	cv::Size input_size = input.size();

	//Pointer to data - fastest access method
	uchar *in_data = (uchar*)input.data;

	//For all pixels...
	for(int y = 0; y < input_size.height; y++)
	{
		for(int x = 0; x < input_size.width; x++)
		{
			// Count blob then remove by flood-filling it out
			if(in_data[y*input.step + x])
			{
				++count;
				cv::floodFill(input, cv::Point(x, y), cv::Scalar(0));
			}
		}
	}

	return count;
}

/*
 * Hit-or-miss transform
 *
 * Arguments
 * cv::Mat        input: Input card to search for suit symbol
 * cv::Mat  struct_elem: Image of the suit symbol structuring element
 *   float minimum_perc: The minimum percentage of matched pixels to count as a 'template found'
 *
 * Returns
 * cv::Mat: Image of locations of potential matches
 */
cv::Mat hit_or_miss(cv::Mat input, cv::Mat struct_elem, float minimum_perc)
{
	if(input.channels() == 1 && struct_elem.channels() == 1)
	{
		//Setup output
		cv::Mat output = cv::Mat(input.rows, input.cols, CV_8UC1, cv::Scalar(0));

		//Meta
		uchar 
			*in_data = (uchar*)input.data,
			*se_data = (uchar*)struct_elem.data,
			*out_data = (uchar*)output.data
		;
		int
			in_channels = input.channels(),
			se_channels = struct_elem.channels(),
			out_channels = output.channels()
		;

		//Origin at the center, not the topleft etc
		int h_half = (struct_elem.rows - 1) / 2;
		int w_half = (struct_elem.cols - 1) / 2;

		//Search all pixels
		for(int y = 0; y < input.rows; y++)
		{
			for(int x = 0; x < input.cols; x++)
			{
				//For each position, try and find a match
				int matched_pixels = 0;

				//Search whole SE
				for(int j = 0; j < struct_elem.rows; j++)
				{
					for(int i = 0; i < struct_elem.cols; i++)
					{
						//If within the image (to cater for near edges)
						if(is_in_image((x - w_half) + i, (y - h_half) + j, input.cols, input.rows))
						{
							//Input pixel value
							int in_p = in_data[((y - h_half) + j)*input.step + ((x - w_half) + i)*in_channels];
							int se_p = se_data[j * struct_elem.step + i * se_channels];

							//Minithresh - but should be binary anyway!
							in_p = in_p > 128 ? 255 : 0;
							se_p = se_p > 128 ? 255 : 0;

							if(in_p == se_p)
							{
								//Matching pixel found!
								matched_pixels++;
							}
						}
					}
				}

				//Was this a matched template?
				int total_pixels = struct_elem.rows * struct_elem.cols;
				float perc_match = (float)matched_pixels / (float)total_pixels;

				//Mark match
				out_data[y * output.step + x * out_channels] = perc_match > minimum_perc ? 255 : 0;
			}
		}

		//Finally
		return output;
	}
	else
	{
		cout << "Channels must be 1 for hit_or_miss()!" << endl;
		return cv::Mat(1, 1, CV_8UC1, cv::Scalar(0));
	}
}

int find_suit(cv::Mat card, float minimum_perc)
{
	if(card.channels() == 1)
	{
		cout << "Performing find_suit()" << endl;

		//HOM all suits at all scales at both normal and flip orientations
		int matches[4] = {0, 0, 0, 0};

		//Full scale
		cout << "Running full scale match...";
		matches[CLUB] += count_blobs(hit_or_miss(card, cv::imread("res/symbols/scale_full/club.png", CV_LOAD_IMAGE_GRAYSCALE), minimum_perc));	//GS for one channel
		matches[CLUB] += count_blobs(hit_or_miss(card, cv::imread("res/symbols/scale_full/club_flipped.png", CV_LOAD_IMAGE_GRAYSCALE), minimum_perc));
		matches[DIAMOND] += count_blobs(hit_or_miss(card, cv::imread("res/symbols/scale_full/diamond.png", CV_LOAD_IMAGE_GRAYSCALE), minimum_perc));	
		matches[DIAMOND] += count_blobs(hit_or_miss(card, cv::imread("res/symbols/scale_full/diamond_flipped.png", CV_LOAD_IMAGE_GRAYSCALE), minimum_perc));
		matches[HEART] += count_blobs(hit_or_miss(card, cv::imread("res/symbols/scale_full/heart.png", CV_LOAD_IMAGE_GRAYSCALE), minimum_perc));	
		matches[HEART] += count_blobs(hit_or_miss(card, cv::imread("res/symbols/scale_full/heart_flipped.png", CV_LOAD_IMAGE_GRAYSCALE), minimum_perc));
		matches[SPADE] += count_blobs(hit_or_miss(card, cv::imread("res/symbols/scale_full/spade.png", CV_LOAD_IMAGE_GRAYSCALE), minimum_perc));	
		matches[SPADE] += count_blobs(hit_or_miss(card, cv::imread("res/symbols/scale_full/spade_flipped.png", CV_LOAD_IMAGE_GRAYSCALE), minimum_perc));
		cout << "Scores (C/D/H/S): " << matches[CLUB] << "/" << matches[DIAMOND] << "/" << matches[HEART] << "/" << matches[SPADE] << endl;

		//Half scale
		cout << "Running half scale match...";
		matches[CLUB] += count_blobs(hit_or_miss(card, cv::imread("res/symbols/scale_50perc/club.png", CV_LOAD_IMAGE_GRAYSCALE), minimum_perc));	//GS for one channel
		matches[CLUB] += count_blobs(hit_or_miss(card, cv::imread("res/symbols/scale_50perc/club_flipped.png", CV_LOAD_IMAGE_GRAYSCALE), minimum_perc));
		matches[DIAMOND] += count_blobs(hit_or_miss(card, cv::imread("res/symbols/scale_50perc/diamond.png", CV_LOAD_IMAGE_GRAYSCALE), minimum_perc));	
		matches[DIAMOND] += count_blobs(hit_or_miss(card, cv::imread("res/symbols/scale_50perc/diamond_flipped.png", CV_LOAD_IMAGE_GRAYSCALE), minimum_perc));
		matches[HEART] += count_blobs(hit_or_miss(card, cv::imread("res/symbols/scale_50perc/heart.png", CV_LOAD_IMAGE_GRAYSCALE), minimum_perc));	
		matches[HEART] += count_blobs(hit_or_miss(card, cv::imread("res/symbols/scale_50perc/heart_flipped.png", CV_LOAD_IMAGE_GRAYSCALE), minimum_perc));
		matches[SPADE] += count_blobs(hit_or_miss(card, cv::imread("res/symbols/scale_50perc/spade.png", CV_LOAD_IMAGE_GRAYSCALE), minimum_perc));	
		matches[SPADE] += count_blobs(hit_or_miss(card, cv::imread("res/symbols/scale_50perc/spade_flipped.png", CV_LOAD_IMAGE_GRAYSCALE), minimum_perc));
		cout << "Scores (C/D/H/S): " << matches[CLUB] << "/" << matches[DIAMOND] << "/" << matches[HEART] << "/" << matches[SPADE] << endl;
		
		//30% scale
		cout << "Running 30% scale match...";
		matches[CLUB] += count_blobs(hit_or_miss(card, cv::imread("res/symbols/scale_30perc/club.png", CV_LOAD_IMAGE_GRAYSCALE), minimum_perc));	//GS for one channel
		matches[CLUB] += count_blobs(hit_or_miss(card, cv::imread("res/symbols/scale_30perc/club_flipped.png", CV_LOAD_IMAGE_GRAYSCALE), minimum_perc));
		matches[DIAMOND] += count_blobs(hit_or_miss(card, cv::imread("res/symbols/scale_30perc/diamond.png", CV_LOAD_IMAGE_GRAYSCALE), minimum_perc));	
		matches[DIAMOND] += count_blobs(hit_or_miss(card, cv::imread("res/symbols/scale_30perc/diamond_flipped.png", CV_LOAD_IMAGE_GRAYSCALE), minimum_perc));
		matches[HEART] += count_blobs(hit_or_miss(card, cv::imread("res/symbols/scale_30perc/heart.png", CV_LOAD_IMAGE_GRAYSCALE), minimum_perc));	
		matches[HEART] += count_blobs(hit_or_miss(card, cv::imread("res/symbols/scale_30perc/heart_flipped.png", CV_LOAD_IMAGE_GRAYSCALE), minimum_perc));
		matches[SPADE] += count_blobs(hit_or_miss(card, cv::imread("res/symbols/scale_30perc/spade.png", CV_LOAD_IMAGE_GRAYSCALE), minimum_perc));	
		matches[SPADE] += count_blobs(hit_or_miss(card, cv::imread("res/symbols/scale_30perc/spade_flipped.png", CV_LOAD_IMAGE_GRAYSCALE), minimum_perc));
		cout << "Scores (C/D/H/S): " << matches[CLUB] << "/" << matches[DIAMOND] << "/" << matches[HEART] << "/" << matches[SPADE] << endl;

		//Find which suit was matched most
		if(max(matches[CLUB], matches[DIAMOND], matches[HEART], matches[SPADE]) == matches[CLUB])
		{
			cout << "Suit may be CLUBS!" << endl;
			return CLUB;
		}
		else if(max(matches[CLUB], matches[DIAMOND], matches[HEART], matches[SPADE]) == matches[DIAMOND])
		{
			cout << "Suit may be DIAMONDS!" << endl;
			return DIAMOND;
		}
		else if(max(matches[CLUB], matches[DIAMOND], matches[HEART], matches[SPADE]) == matches[HEART])
		{
			cout << "Suit may be HEARTS!" << endl;
			return HEART;
		}
		else if(max(matches[CLUB], matches[DIAMOND], matches[HEART], matches[SPADE]) == matches[SPADE])
		{
			cout << "Suit may be SPADES!" << endl;
			return SPADE;
		}
		else
		{
			cout << "No winner! UNKOWN SUIT" << endl;
			return -1;
		}
	}
	else
	{
		cout << "Channels must be 1 for find_suit()!" << endl;
		return -2;
	}
}

int find_suit_scaled(cv::Mat card, float minimum_perc)
{
	if(card.channels() == 1)
	{
		cout << "Performing find_suit_scaled()" << endl;

		//HOM all suits at all scales at both normal and flip orientations
		int matches[4] = {0, 0, 0, 0};

		//Load original SEs
		cv::Mat se_symbols[4] = {
			cv::imread("../../../gitsrc/res/symbols/scale_full/club.png", CV_LOAD_IMAGE_GRAYSCALE),
			cv::imread("../../../gitsrc/res/symbols/scale_full/diamond.png", CV_LOAD_IMAGE_GRAYSCALE),
			cv::imread("../../../gitsrc/res/symbols/scale_full/heart.png", CV_LOAD_IMAGE_GRAYSCALE),
			cv::imread("../../../gitsrc/res/symbols/scale_full/spade.png", CV_LOAD_IMAGE_GRAYSCALE)
		};

		//Do ten scales - 1/10 to 10/10
		for(int i = 1; i < 11; i++)
		{
			//For reach symbol
			for(int j = 0; j < 4; j++)
			{
				//Calculate new size
				int width = (int) round(((float)i / 10.0F) * (float)se_symbols[j].cols);
				int height = (int) round(((float)i / 10.0F) * (float)se_symbols[j].rows);
				cv::Size size(width, height);

				//Resize SE
				cv::Mat temp;
				cv::resize(se_symbols[j].clone(), temp, size);

				//Do match
				cout << "Suit " << (j + 1) << "/4. Scale: " << i << "/10..." << endl;
				matches[j] += count_blobs(hit_or_miss(card, temp, minimum_perc));
			}
		}

		cout << "Scores (C/D/H/S): " << matches[CLUB] << "/" << matches[DIAMOND] << "/" << matches[HEART] << "/" << matches[SPADE] << endl;

		//Find which suit was matched most
		if(max(matches[CLUB], matches[DIAMOND], matches[HEART], matches[SPADE]) == matches[CLUB])
		{
			cout << "Suit may be CLUBS!" << endl;
			return CLUB;
		}
		else if(max(matches[CLUB], matches[DIAMOND], matches[HEART], matches[SPADE]) == matches[DIAMOND])
		{
			cout << "Suit may be DIAMONDS!" << endl;
			return DIAMOND;
		}
		else if(max(matches[CLUB], matches[DIAMOND], matches[HEART], matches[SPADE]) == matches[HEART])
		{
			cout << "Suit may be HEARTS!" << endl;
			return HEART;
		}
		else if(max(matches[CLUB], matches[DIAMOND], matches[HEART], matches[SPADE]) == matches[SPADE])
		{
			cout << "Suit may be SPADES!" << endl;
			return SPADE;
		}
		else
		{
			cout << "No winner! UNKOWN SUIT" << endl;
			return -1;
		}
	}
	else
	{
		cout << "Channels must be 1 for find_suit()!" << endl;
		return -2;
	}
}