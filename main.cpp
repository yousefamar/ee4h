/************************************************************************\
| Main code file for the EE4H Assignment (Playing card recognition)      |
|																		 |
| Authors: Yousef Amar and Chris Lewis									 |
| Last Modified: 28/01/2014												 |
|																		 |
| Dependencies: OpenCV-2.4.2											 |
|				- opencv_core242.dll									 |
|				- opencv_imgproc242.dll									 |
|				- opencv_highgui242.dll									 |
|				- tbb.dll (for Intel x64)								 |
\************************************************************************/

//Process includes only once
#ifndef INCLUDES_SEEN
#define INCLUDES_SEEN
#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>

#include "preprocessing.h"
#include "utils.h"

#include <iostream>
#include <string>
#include <strstream>
#endif

//Include guards for this?
#include "preprocessing.h"

/**
  * Program entry point.
  *
  * Arguments
  *    int argc: Number of arguments
  * char** argv: Array of arguments: [1] - Image path to open
  *
  * Returns
  * int: Error code or 0 if no error occured (Define error codes? Wrap in cout func?)
  */
int main(int argc, char **argv)
{
	//Check image is provided
	if(argc > 1)
	{
		cv::Mat input = cv::imread(argv[1], CV_LOAD_IMAGE_COLOR);
		cv::Size input_size = input.size();

		//Check size is greater than zero
		if(input_size.width > 0 && input_size.height > 0)
		{
			//Show image
			cv::imshow("Input Image", input);

			//Make background black
			cv::Mat working = make_background_black(input, 250);
			cv::imshow("Black background", working);

			//Is red suit?
			cout << "Is red suit? " << (is_red_suit(working, 250) == true ? "true" : "false") << endl;

			cv::waitKey(0);

			//Finally
			return 0;	//No error code
		}
		else
		{
			return -2;	//Image size zero code
		}
	}
	else
	{
		return -1;	//Incorrect arguments code
	}
}