/************************************************************************\
| Main code file for the EE4H Assignment (Playing card recognition)      |
|																		 |
| Authors: Yousef Amar and Chris Lewis									 |											 
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

			//Hack to deal with super large, hi-res images
			if (input_size.width > 1000)
				cv::resize(input, input, cv::Size(1000, 1000*input_size.height/input_size.width));

			//Find cards in image
			vector<cv::Mat> cards;
			find_cards(input, &cards);

			vector<Results> resultss;

			for(size_t i = 0; i < cards.size(); i++)
			{
				cv::Mat card = cards[i];

				cv::Mat grey8(card.size(), CV_8U);
				cv::cvtColor(card, grey8, CV_BGR2GRAY);
				
				//CLAHE (Contrast Limited Adaptive Histogram Equalization)
				cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE(2.0, cv::Size(8, 8));
				clahe->apply(grey8, grey8);
				cv::blur(grey8, grey8, cv::Size(4, 4));

				cv::Mat card_bin = grey8 >= 140;
				
				Results results;
				results.init();

				cv::Size card_size = card.size();

				//Detect suit colour
				find_colour(card, &results);

				//Get card value
				find_value(card_bin, &results);
				
				//Try and find suit
				//find_suit_scaled(card_bin, 0.9F, &results);

				//Show regions searched on output window
				int region_width = (int) (corner_h_perc * (float) card_size.width);
				int region_height = (int) (corner_v_perc * (float) card_size.height);
				cv::Point start = cv::Point(0, 0);
				cv::Point finish = cv::Point(region_width, region_height);
				cv::rectangle(card, start, finish, line_colour, 1, 8, 0);	//Top left
				start = cv::Point(card_size.width - region_width, card_size.height - region_height);
				finish = cv::Point(card_size.width, card_size.height);
				cv::rectangle(card, start, finish, line_colour, 1, 8, 0);	//Bottom right

				resultss.push_back(results);
			}

			show_cascade(cards, resultss);

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