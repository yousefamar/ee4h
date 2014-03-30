#include "../include/stdafx.h"

//Include guard
#ifndef ISOLATION_H
#define ISOLATION_H

int find_cards(cv::Mat input, std::vector<Card>* cards);
void find_symbol(Card *card);

#endif