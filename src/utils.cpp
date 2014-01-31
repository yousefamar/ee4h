/************************************************************************\
| Utils file for EE4H Assignment						         		 |
|																		 |
| Authors: Yousef Amar and Chris Lewis									 |
| Last Modified: 30/01/2014												 |
|																		 |
\************************************************************************/

//Process includes only once
#ifndef INCLUDES_SEEN
#define INCLUDES_SEEN
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "../include/preprocessing.h"
#include "../include/utils.h"

#include <iostream>
#include <string>
#include <strstream>
#endif

using namespace std;

/**
  * Custom breakpoint message
  *
  * Arguments
  * char *message: The message to display
  */
void log(char *message)
{
	cout << message << endl;
}

/**
  * Custom breakpoint message
  *
  * Arguments
  * string message: The message to display
  */
void log(string message)
{
	cout << message << endl;
}

/**
  *	Max of two ints
  * 
  * Arguments
  * int a: Operand 1
  * int b: Operand 2
  *
  * Returns
  * int: greater of the two numbers
  */
int max(int a, int b)
{
	return a > b ? a : b;
}

/**
  *	Max of three ints
  * 
  * Arguments
  * int a: Operand 1
  * int b: Operand 2
  * int c: Operand 3
  *
  * Returns
  * int: greater of the three numbers
  */
int max(int a, int b, int c)
{
	return max(a, max(b, c));
}