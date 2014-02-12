/************************************************************************\
| Card isolation file for EE4H Assignment						         |
|																		 |
| Authors: Yousef Amar and Chris Lewis									 |
| Last Modified: 12/02/2014												 |
|																		 |
\************************************************************************/

#include "../include/stdafx.h"

using namespace std;

/**
  * Find and perspective transform a card in an image
  *
  * Arguments
  * cv::Mat input:		     Image matrix
  *
  * Returns
  * cv::Mat: A perspective-corrected image of a card
  */
cv::Mat find_card(cv::Mat input)
{
	return input.clone();
}