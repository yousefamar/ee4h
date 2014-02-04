/************************************************************************\
| Pre-processing file for EE4H Assignment						         |
|																		 |
| Authors: Yousef Amar and Chris Lewis									 |
| Last Modified: 03/02/2014												 |
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

// TODO: Do not also colour them
int count_blobs(cv::Mat input)
{
	int count = 0;

	//Get size
	cv::Size input_size = input.size();

	//Create output
	cv::Mat output(input_size.height, input_size.width, CV_8UC3);

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
			int b = in_data[(y)*input.step + (x)*input_channels + 0];
			int g = in_data[(y)*input.step + (x)*input_channels + 1];
			int r = in_data[(y)*input.step + (x)*input_channels + 2];

			// Separate blobs by flood-filling them
			if(b == 0 && g == 0 && r == 255)
			{
				++count;
				cv::floodFill(input, cv::Point(x, y), cv::Scalar(rand()%256, rand()%256, rand()%256));
			}
		}
	}

	return count;
}