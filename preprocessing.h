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

bool is_red_suit(cv::Mat input, int base_threshold);

#endif