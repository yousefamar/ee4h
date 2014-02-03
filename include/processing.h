#include "../include/stdafx.h"

//Include guard
#ifndef PROCESSING_H
#define PROCESSING_H

cv::Mat canny_thresh(cv::Mat input, int low_thresh, int ratio, int kernel_size);

#endif