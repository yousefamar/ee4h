#include "../include/stdafx.h"

//Include guard
#ifndef PROCESSING_H
#define PROCESSING_H

//Array position identifiers
#define CLUB 0
#define DIAMOND 1
#define HEART 2
#define SPADE 3
#define JACK 0
#define QUEEN 1
#define KING 2

//Function prototypes
cv::Mat canny_thresh(cv::Mat input, int low_thresh, int ratio, int kernel_size);
cv::Mat morph_gradient(cv::Mat input);
int count_blobs(cv::Mat input, int level);
cv::Mat hit_or_miss(cv::Mat input, cv::Mat struct_elem, float minimum_perc);
void detect_colour(Card *card);
void detect_type(Card *card);
void detect_value_number(Card *card);
void detect_value_picture(Card *card);
void find_suit_sym(Card *card);
float hit_or_miss_score(cv::Mat img, cv::Mat se_image);

#endif