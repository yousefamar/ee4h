/************************************************************************\
| Utils file for EE4H Assignment						         		 |
|																		 |
| Authors: Yousef Amar and Chris Lewis									 |
|																		 |
\************************************************************************/

#include "../include/stdafx.h"

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

/**
  *	Max of four ints
  * 
  * Arguments
  * int a: Operand 1
  * int b: Operand 2
  * int c: Operand 3
  * int d: Operand 4
  *
  * Returns
  * int: greater of the four numbers
  */
int max(int a, int b, int c, int d)
{
	return max(max(a, max(b, c)), d);
}

/**
  * Min of two ints
  * 
  * Arguments
  * int a: Operand 1
  * int b: Operand 2
  *
  * Returns
  * int: lesser of the two numbers
  */
int min(int a, int b)
{
  return a < b ? a : b;
}

/*
 * Manhattan Distance calculation
 *
 * Arguments
 * int x1: Point 1 X co-ordinate
 * int x2: Point 2 X co-ordinate
 * int y1: Point 1 Y co-ordinate
 * int y2: Point 2 Y co-ordinate
 *
 * Returns:
 * int: Distance between points 1 and 2
 */
int dist_manhattan(int x1, int x2, int y1, int y2)
{
  return abs(x2-x1 + y2-y1);
}

/*
 * Return true if a point is in the image
 * 
 * Arguments
 * int      x: X co-ordinate
 * int      y: Y co-ordinate
 * int  width: Width of bounds
 * int height: Height of bounds
 */
bool is_in_image(int x, int y, int width, int height)
{
	return x > 0 && x < width && y > 0 && y < height;
}

/*
 * Round to nearest integer
 *
 * Arguments:
 * double d: Value to round
 *
 * Returns:
 * double: Nearest integer in double format
 */
double round(double d)
{
  return floor(d + 0.5);
}