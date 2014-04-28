#include "../include/stdafx.h"

using namespace std;

//Include guard
#ifndef EXTRAS_H
#define EXTRAS_H

//Function prototypes
void log(char *message);
void log(string message);
int max(int a, int b);
int max(int a, int b, int c);
int max(int a, int b, int c, int d);
int min(int a, int b);
int dist_manhattan(int x1, int x2, int y1, int y2);
bool is_in_image(int x, int y, int width, int height);
double round(double d);

#endif