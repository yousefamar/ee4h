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

/*
 * Process an image and look for cards to classify
 *
 * Arguments:
 * cv::Mat input: Whole image to find cards
 *
 * Returns:
 * int: Any error code, else 0 (exit success)
 */
int process_image(cv::Mat input)
{
	cv::Size input_size = input.size();

	//Check size is greater than zero
	if(input_size.width > 0 && input_size.height > 0)
	{
		//Show image details
		cout << "Image is " << input_size.width << " by " << input_size.height << " pixels." << endl << endl;

		//Deal with super large, hi-res images
		if (input_size.width > 1000)
			cv::resize(input, input, cv::Size(1000, 1000*input_size.height/input_size.width));
		if (input_size.height > 500)
			cv::resize(input, input, cv::Size(500*input_size.width/input_size.height, 500));

		//Find cards in image and populate cards vector
		vector<Card> cards;
		if (find_cards(input, &cards)) {
			cerr << "Could not find any cards!" << endl;
			return -4; //No cards found
		}

		for(size_t i = 0; i < cards.size(); i++)
		{
			Card *card = &cards[i];

			//Detect suit colour
			detect_colour(card);

			//Detect suit type (number/picture)
			detect_type(card);

			//Find symbols
			find_symbols(card);

			//Get card value
			if (!card->is_picture_card)
			{
				detect_value_number(card);
			}
			else
			{
				detect_value_picture(card);
			}

			//Find suit
			find_suit_sym(card);
		}

		//Show results until key press
		show_cascade(cards);

	}
	else
	{
		cerr << "Image dimensions must be > 0!" << endl;
		return -2;	//Image size zero code
	}

	//Finally
	cout << "Processing finished successfully!" << endl;
	return 0;	//No error code
}


/*
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
	if(argc < 2)
	{
		cout << "Arguments error. Check image path/format? Did you mean to use --cam?" << endl;
		return -1;	//Incorrect arguments code
	}

	cv::Mat input;

	//Get from webcam?
	bool from_cam = !strcmp(argv[1], "--cam"), should_quit = false;
	
	if (argc > 2) {
		cout << "Multi mode activated!" << endl;
		multi_mode = !strcmp(argv[2], "--multi");
	}
	
	//Image aquisition loop
	do
	{
		if(!from_cam)
		{
			input = cv::imread(argv[1], CV_LOAD_IMAGE_COLOR);
		}
		else
		{
			cv::VideoCapture cap(CV_CAP_ANY);
			cv::waitKey(1000);

			if(!cap.isOpened())
			{
				cerr << "Unable to access webcam" << endl;
				return -3;	//Incorrect arguments code
			}

			cout << "Press space to take a photo" << endl;

			char key;
			do
			{
				key = cvWaitKey(10);

				cap >> input;

				cv::imshow("Webcam", input);

				// Break out of loop if Space key is pressed
			} while (char(key) != 32);

			cv::destroyWindow("Webcam");
		}

		//Card classification!
		if (process_image(input))
			return EXIT_FAILURE;

		if (!from_cam)
		{
			cout << "Press any key to quit" << endl;
		} else {
			cout << "Press Esc to quit, any other key to try again" << endl;
		}

		should_quit = (from_cam && cv::waitKey(0)==27) || (!from_cam && cv::waitKey(0));

	} while(!should_quit);

	return EXIT_SUCCESS;
}