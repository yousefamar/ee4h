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

#ifndef PREPROCESSING_H
#define PREPROCESSING_H

cv::Mat binary_threshold(cv::Mat input, float threshold_value, int min, int max);

cv::Mat make_background_black(cv::Mat input, int white_level);

cv::Mat filter_red_channel(cv::Mat input, int new_value);

bool is_red_suit(cv::Mat input, int base_threshold);

bool is_red_suit_by_corners(cv::Mat input, float horiz_margin_perc, float vert_margin_perc, int base_threshold, int target_regions);

#endif