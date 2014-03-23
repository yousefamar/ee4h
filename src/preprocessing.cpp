/************************************************************************\
| Pre-processing file for EE4H Assignment						         |
|																		 |
| Authors: Yousef Amar and Chris Lewis									 |
| Last Modified: 03/02/2014												 |
|																		 |
\************************************************************************/

#include "../include/stdafx.h"

using namespace std;

/**
  * Create a binary image by thresholding an image
  *
  * Arguments
  * cv::Mat input:		     Image matrix
  *     int threshold_value: Threshold value to use
  *     int min:			 Background colour
  *     int max:			 Foreground colour
  *
  * Returns
  * cv::Mat: A binary thresholded image matrix
  */
cv::Mat binary_threshold(cv::Mat input, float threshold_value, int min, int max)
{
	//Get size
	cv::Size input_size = input.size();

	//Create output
	cv::Mat output(input_size.height, input_size.width, CV_8UC1);

	//Convert to grey-scale
//	cv::cvtColor(input, output, CV_BGR2GRAY);

	//Pointers to data - fastest access method
	uchar *in_data = (uchar*)input.data;
	uchar *out_data = (uchar*)output.data;

	//Additional image info gathered once for speed
	int input_channels = input.channels();
	int output_channels = output.channels();

	//For all pixels...
	for(int y = 0; y < input_size.height; y++)
	{
		for(int x = 0; x < input_size.width; x++)
		{
			//Do thresholding
			int b = in_data[(y)*input.step + (x)*input_channels + 0];
			int g = in_data[(y)*input.step + (x)*input_channels + 1];
			int r = in_data[(y)*input.step + (x)*input_channels + 2];

			//Get average
			float avg = (float) ((r + b + g) / 3);

			//Set output pixel
			if(avg > threshold_value)
			{
				out_data[(y)*output.step + (x)*output_channels + 0] = max;
			}
			else
			{
				out_data[(y)*output.step + (x)*output_channels + 0] = min;
			}
		}
	}

	//Finally
	return output;
}

/**
  * Make a card's background black to allow red detection to work against white
  *
  * Arguments
  * cv::Mat input:       Card input image matrix
  *     int white_level: White level of the card. TODO: MAY VARY WITH ILLUMINATION
  *
  * Returns
  * cv::Mat: Output image matrix with black in place of white
  */
cv::Mat make_background_black(cv::Mat input, int white_level)
{
	//Get size
	cv::Size input_size = input.size();

	//Create output
	cv::Mat output = input.clone();

	//Pointer to data
	uchar *in_data = (uchar*)input.data;
	uchar *out_data = (uchar*)output.data;

	//Additional image info gathered once for speed
	int input_channels = input.channels();
	int output_channels = output.channels();

	//For all pixels...
	for(int y = 0; y < input_size.height; y++)
	{
		for(int x = 0; x < input_size.width; x++)
		{
			//Get BGR values
			int b = in_data[(y)*input.step + (x)*input_channels + 0];
			int g = in_data[(y)*input.step + (x)*input_channels + 1];
			int r = in_data[(y)*input.step + (x)*input_channels + 2];

			//Set output pixel
			if(b > white_level && g > white_level && r > white_level)
			{
				//Make it black
				out_data[(y)*output.step + (x)*output_channels + 0] = 0;
				out_data[(y)*output.step + (x)*output_channels + 1] = 0;
				out_data[(y)*output.step + (x)*output_channels + 2] = 0;
			}
			else
			{
				//Leave pixel as it is
			}
		}
	}

	//Finally
	return output;
}

/**
  * Filter an image and retain only the red channel
  *
  * Arugments
  * cv::Mat input:     The card image matrix
  *     int new_value: The value to set the green and blue channels to
  *
  * Returns
  * cv::Mat: An image matrix with only the red channel remaining
  */
cv::Mat filter_red_channel(cv::Mat input, int new_value)
{
	//Get size
	cv::Size input_size = input.size();

	//Create output from input
	cv::Mat output = input.clone();

	//Pointer to data
	uchar *out_data = (uchar*)output.data;

	//Additional image info gathered once for speed
	int output_channels = output.channels();

	//For all pixels...
	for(int y = 0; y < input_size.height; y++)
	{
		for(int x = 0; x < input_size.width; x++)
		{
			//Set output pixel				
			out_data[(y)*output.step + (x)*output_channels + 0] = new_value;	//B
			out_data[(y)*output.step + (x)*output_channels + 1] = new_value;	//G	
		}
	}

	//Finally
	return output;
}

/**
  * Find out whether a card is a red suit card by the corners
  *
  * Arguments
  * cv::Mat input:             Input card image matrix
  *   float horiz_margin_perc: Percentage of the width of the card from each edge to include
  *   float  vert_margin_perc: Percentage of the height of the card from each edge to include
  *     int    base_threshold: Minimum pixel value (combat black)
  *     int    target_regions: Minimum number of red regions to classify out of 2
  *   float          perc_red: Percentage of the check pixels that must be red to count as a red region
  *
  * Returns
  * bool: True if the card is detected to be a red suit card
  */
bool is_red_suit_by_corners(cv::Mat input, float horiz_margin_perc, float vert_margin_perc, int base_threshold, int target_regions, float perc_red)
{
	bool debug_this = false;

	//Get size of whole image
	cv::Size input_size = input.size();

	//Store dominant colour totals
	int red = 0, blue = 0, green = 0;
	bool regions_red[2] = {false, false};

	//Get four corner regions
	int region_width = (int) (horiz_margin_perc * (float) input_size.width);
	int region_height = (int) (vert_margin_perc * (float) input_size.height);

	//Create region matrices, top left, bottom right...
	cv::Mat regions[2] = {
		input(cv::Rect(0, 0, region_width, region_height)),
		input(cv::Rect(input_size.width - region_width, input_size.height - region_height, region_width, region_height))
	};

	//For all regions...
	for(int i = 0; i < 2; i++)
	{
		//Reset totals
		red = 0; green = 0; blue = 0;

		//Pointer to data
		uchar *data = (uchar*)regions[i].data;

		//Get region channels once for speed
		int region_channels = regions[i].channels();

		//For all pixels...
		for(int y = 0; y < region_height; y++)
		{
			for(int x = 0; x < region_width; x++)
			{
				//Get values
				int b = data[(y)*regions[i].step + (x)*region_channels + 0];
				int g = data[(y)*regions[i].step + (x)*region_channels + 1];
				int r = data[(y)*regions[i].step + (x)*region_channels + 2];

				if(max(r, g, b) == r && r > base_threshold)
				{
					red++;
				}
				else if(max(r, g, b) == g && g > base_threshold)
				{
					green++;
				}
				else if(max(r, g, b) == b && b > base_threshold)
				{
					blue++;
				}
				else {
					//No action
				}
			}
		}

		//Compute result
		float total = (float)region_width * (float) region_height;
		float match_perc = (float) red / total;
		regions_red[i] = (red > blue && red > green) && match_perc > perc_red;

		if(debug_this == true)
		{
			cout << "is_suit_red_by_corners: Region " << i << " totals (RGB): " << red << ", " << green << ", " << blue << endl;
			cout << "is_suit_red_by_corners: match_perc: " << match_perc << "/" << total << endl;
		}
	}

	//Count number of red regions
	int count = 0;
	for(int c = 0; c < 2; c++)
	{
		if(regions_red[c] == true)
		{
			count++;
		}
	}

	if(debug_this == true)
	{
		cout << "is_suit_red_by_corners: count = " << count << "/" << target_regions << endl;
		cout << "is_suit_red_by_corners: top left: 0, 0, " << region_width << " x " << region_height << endl;
		cout << "is_suit_red_by_corners: bottom right: " << (input_size.width - region_width) << ", " << (input_size.height - region_height) << ", " << region_width << " x " << region_height << endl;
	}

	return count >= target_regions;
}

/**
  * Find out whether a card is a red suit card (hearts or diamonds)
  *
  * Arguments
  * cv::Mat input:			The card image matrix
  *     int base_threshold: Minimum pixel value (combat black)
  *
  * Returns 
  * bool: True if there is a clear red dominance
  */
bool is_red_suit(cv::Mat input, int base_threshold)
{
	//Get size
	cv::Size input_size = input.size();

	//Pointer to data
	uchar *in_data = (uchar*)input.data;

	//Store dominant colour totals
	int red = 0, blue = 0, green = 0;

	//Fetch image matrix data once
	int input_channels = input.channels();

	//For all pixels...
	for(int y = 0; y < input_size.height; y++)
	{
		for(int x = 0; x < input_size.width; x++)
		{
			//Get values
			int b = in_data[(y)*input.step + (x)*input_channels + 0];
			int g = in_data[(y)*input.step + (x)*input_channels + 1];
			int r = in_data[(y)*input.step + (x)*input_channels + 2];

			if(max(r, g, b) == r && r > base_threshold)
			{
				red++;
			}
			else if(max(r, g, b) == g && g > base_threshold)
			{
				green++;
			}
			else if(max(r, g, b) == b && r > base_threshold)
			{
				blue++;
			}
			else {
				//No action
			}
		}
	}

	//Compute result
	return (red > blue && red > green);
}