#include "../include/stdafx.h"

//Include guard
#ifndef PREPROCESSING_H
#define PREPROCESSING_H

//Function prototypes
cv::Mat binary_threshold(cv::Mat input, float threshold_value, uchar min, uchar max);
cv::Mat make_background_black(cv::Mat input, int white_level);
cv::Mat filter_red_channel(cv::Mat input, uchar new_value);
bool is_red_suit_by_corners(cv::Mat input, int base_threshold, int target_regions, float perc_red);

#endif