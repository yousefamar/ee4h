/*
 * Extra functions using OpenCV 2.4.8
 * Author: Chris Lewis
 */

#include "../include/stdafx.h"

#ifndef CL_OWN_H
#define CL_OWN_H

#define MODE_BINARY_DILATION 6
#define MODE_BINARY_EROSION 7
#define MODE_BINARY_OPENING 14
#define MODE_BINARY_CLOSING 15

cv::Mat binary_opening(cv::Mat input, int element_size);
cv::Mat binary_closing(cv::Mat input, int element_size);
cv::Mat binary_operation(cv::Mat input, int mode, int element_size);
bool is_in_range(int lower, int subject, int upper, bool limit_inclusive);

#endif