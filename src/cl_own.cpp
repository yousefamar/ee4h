/*
 * Extra functions using OpenCV 2.4.8
 * Author: Chris Lewis
 */

#include "../include/stdafx.h"

/*
 * Check whether an integer is within a range
 */
bool is_in_range(int lower, int subject, int upper, bool limit_inclusive)
{
	if(limit_inclusive)
	{
		return subject >= lower && subject <= upper;
	}
	else
	{
		return subject > lower && subject < upper;
	}
}

/*
 * Performs binary opening
 */
cv::Mat binary_opening(cv::Mat input, int element_size)
{
	cout << "Performing binary opening..." << endl;

	cv::Mat working = input.clone();
	working = binary_operation(working, MODE_BINARY_EROSION, element_size);
	working = binary_operation(working, MODE_BINARY_DILATION, element_size);
	return working;
}

/*
 * Performs binary closing
 */
cv::Mat binary_closing(cv::Mat input, int element_size)
{
	cout << "Performing binary closing..." << endl;

	cv::Mat working = input.clone();
	working = binary_operation(working, MODE_BINARY_DILATION, element_size);
	working = binary_operation(working, MODE_BINARY_EROSION, element_size);
	return working;
}

/*
 * Perform binary dilation or erosion using a single channel input Matrix and square symmetrical structuring element
 * Returns a one channel image matrix
 */
cv::Mat binary_operation(cv::Mat input, int mode, int element_size)
{
	if(input.channels() == 1)
	{
		//Setup output
		cv::Mat output(input.rows, input.cols, CV_8UC1, cv::Scalar(0));
		
		cout << "Performing binary " << (mode == MODE_BINARY_DILATION ? "dilation" :  "erosion") 
			 << "..." << endl;

		//Get metadata
		uchar 
			*data = (uchar*)input.data,
			*out_data = (uchar*)output.data
		;
		int 
			input_channels = input.channels(),
			output_channels = output.channels()
		;

		//Middle element 5 - 1 = 4,  / 2 = 2,  + 1 = 3
		int half = (element_size - 1) / 2;	//See log book 10/2/14

		//For all X x Y pixels
		int result = 0;
		for(int y = 0; y < input.rows; y++)
		{
			for(int x = 0; x < input.cols; x++)
			{
				//Reset for new neightbourhood
				result = 0;
					
				//Get local values
				bool break_now = false;
				for(int j = 0; j < element_size; j++)
				{
					for(int i = 0; i < element_size; i++)
					{
						//If in the image
						if(is_in_image((x - half) + i, (y - half) + j, input.cols, input.rows))
						{
							//Channel values
							int p = data[((y - half) + j) * input.step + ((x - half) + i) * input_channels];

							//Save brightest of that area
							if(mode == MODE_BINARY_DILATION)
							{
								if(p  > 128.0F)	//Assuming binary input
								{
									result = 255;
									break_now = true;
									break;
								}
							}
							else
							{
								if(p < 128.0F)
								{
									result = 0;
									break_now = true;
									break;
								}
								else
								{
									result = 255;
								}
							}
						}
					}

					if(break_now == true)	//This is used for speed. Eg for erosion, if any are black, all become black
					{
						break;
					}
				}
			
				//Save output Mat data
				out_data[y * output.step + x * output_channels] = result;
			}
		}

		return output;
	}
	else
	{
		cout << "Input is not a 1 channel image!" << endl;
		return cv::Mat(1, 1, CV_8UC1, cv::Scalar(0));
	}
}