#include "../include/stdafx.h"

//Include guard
#ifndef PROCESSING_H
#define PROCESSING_H

#define CLUB 0
#define DIAMOND 1
#define HEART 2
#define SPADE 3

// TODO: Capitalise
#define corner_h_perc 0.15F
#define corner_v_perc 0.25F

cv::Mat canny_thresh(cv::Mat input, int low_thresh, int ratio, int kernel_size);
cv::Mat morph_gradient(cv::Mat input);
int count_blobs(cv::Mat input);
cv::Mat hit_or_miss(cv::Mat input, cv::Mat struct_elem, float minimum_perc);
void find_colour(cv::Mat card, Results* results);
void find_value(cv::Mat card_bin, Results* results);
int find_suit(cv::Mat card, float minimum_perc);
int find_suit_scaled(cv::Mat card, float minimum_perc, Results* results);

#endif