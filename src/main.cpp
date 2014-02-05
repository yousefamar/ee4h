/************************************************************************\
| Main code file for the EE4H Assignment (Playing card recognition)      |
|																		 |
| Authors: Yousef Amar and Chris Lewis									 |
| Last Modified: 05/02/2014												 |
|																		 |
| Dependencies: OpenCV-2.4.2											 |
|				- opencv_core242.dll									 |
|				- opencv_imgproc242.dll									 |
|				- opencv_highgui242.dll									 |
|				- tbb.dll (Built for Intel x64)							 |
\************************************************************************/

#include "../include/stdafx.h"

using namespace std;

//Configuration
float
	corner_h_perc = 0.36F,
	corner_v_perc = 0.25F
;

/**
  * Program entry point.
  *
  * Arguments
  *    int argc: Number of arguments
  * char** argv: Array of arguments: [1] - Image path to open
  *
  * Returns
  * int: Error code or 0 if no error occured
  */
int main(int argc, char **argv)
{
	cout << endl << "----------------------------------------------" << endl
				 << " EE4H Assignment - Recognising playing cards  " << endl
				 << " By Yousef Amar and Chris Lewis               " << endl 
				 << "----------------------------------------------" << endl << endl;
	
	//Check image is provided
	if(argc > 1)
	{
		cv::Mat input = cv::imread(argv[1], CV_LOAD_IMAGE_COLOR);
		cv::Size input_size = input.size();

		//Check size is greater than zero
		if(input_size.width > 0 && input_size.height > 0)
		{
			//Init Results
			Results results;
			results.init();
			
			//Show image details
			cout << "'" << argv[1] << "' is " << input_size.width << " by " << input_size.height << " pixels." << endl << endl;

			//Make background black
			cv::Mat working = make_background_black(input, 250);

			//Binary thresh
			cv::Mat grey;
			cv::cvtColor(input, grey, CV_BGR2GRAY);
			cv::Mat working_bin;
			cv::threshold(grey, working_bin, 250, 255, cv::THRESH_BINARY_INV);

			//Filter only red
			working = filter_red_channel(working, 0);

			//Is red suit?
			results.detected_colour = is_red_suit_by_corners(working, corner_h_perc, corner_v_perc, 250, 2) == true ? Results::RED : Results::BLACK;

			// Morphological Gradient
			working = morph_gradient(working);

			// Count symbols, -4 for corners, -1 for border
			results.detected_value = count_blobs(working_bin) - 4 - 1;

			//Show regions searched
			int region_width = (int) (corner_h_perc * (float) input_size.width);
			int region_height = (int) (corner_v_perc * (float) input_size.height);
			
			//Top left
			cv::Point start = cv::Point(0, 0);
			cv::Point finish = cv::Point(region_width, region_height);
			cv::rectangle(working, start, finish, line_colour, 1, 8, 0);

			//Bottom right
			start = cv::Point(input_size.width - region_width, input_size.height - region_height);
			finish = cv::Point(input_size.width, input_size.height);
			cv::rectangle(working, start, finish, line_colour, 1, 8, 0);

			//Show results window
			results.show();

			//Show results until key press
			cv::imshow("Results", working);
			cv::waitKey(0);

			//Finally
			cout << "Processing finished successfully!" << endl;
			return 0;	//No error code
		}
		else
		{
			cout << "Image dimensions must be > 0!" << endl;
			return -2;	//Image size zero code
		}
	}
	else
	{
		cout << "Arguments error. Check image path/format?" << endl;
		return -1;	//Incorrect arguments code
	}
}