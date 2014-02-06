/************************************************************************\
| Classification file for EE4H Assignment						         |
|																		 |
| Authors: Yousef Amar and Chris Lewis									 |
| Last Modified: 05/02/2014												 |
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