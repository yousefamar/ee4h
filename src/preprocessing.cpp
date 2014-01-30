/************************************************************************\
| Pre-processing file for EE4H Assignment						         |
|																		 |
| Authors: Yousef Amar and Chris Lewis									 |
| Last Modified: 30/01/2014												 |
|																		 |
\************************************************************************/

//Process includes only once
#ifndef INCLUDES_SEEN
#define INCLUDES_SEEN
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "../include/preprocessing.h"
#include "../include/utils.h"

#include <iostream>
#include <string>
#include <strstream>
#endif

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
	cv::Mat output(input_size.height, input_size.width, CV_8UC3);

	//Convert to grey-scale
	cv::cvtColor(input, output, CV_BGR2GRAY);

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
				out_data[(y)*output.step + (x)*output_channels + 1] = max;
				out_data[(y)*output.step + (x)*output_channels + 2] = max;
			}
			else
			{
				out_data[(y)*output.step + (x)*output_channels + 0] = min;
				out_data[(y)*output.step + (x)*output_channels + 1] = min;
				out_data[(y)*output.step + (x)*output_channels + 2] = min;
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
	cv::Mat output(input);

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
	cv::Mat output(input);

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

	cout << "Totals: R:" << red << " G:" << green << " B:" << blue << endl;

	//Compute result
	return (red > blue && red > green);
}