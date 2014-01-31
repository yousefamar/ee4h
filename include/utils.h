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

#ifndef EXTRAS_H
#define EXTRAS_H

using namespace std;

void log(char *message);

void log(string message);

int max(int a, int b);

int max(int a, int b, int c);

#endif