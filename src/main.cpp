/************************************************************************\
| Main code file for the EE4H Assignment (Playing card recognition)      |
|																		 |
| Authors: Yousef Amar and Chris Lewis									 |
| Last Modified: 12/02/2014												 |
|																		 |
| Dependencies: OpenCV-2.4.8											 |
|				- opencv_core248.dll									 |
|				- opencv_imgproc248.dll									 |
|				- opencv_highgui248.dll									 |
|				- tbb.dll (Built for Intel x64)							 |
\************************************************************************/

#include "../include/stdafx.h"

using namespace std;

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
			//Show image details
			cout << "'" << argv[1] << "' is " << input_size.width << " by " << input_size.height << " pixels." << endl << endl;
			//cv::imshow("Input", input);

			//Find card in image
			find_cards(input);

			//Show results until key press
			//cv::imshow("Results", working);
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