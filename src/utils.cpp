/************************************************************************\
| Utils file for EE4H Assignment						         		 |
|																		 |
| Authors: Yousef Amar and Chris Lewis									 |
| Last Modified: 03/02/2014												 |
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
  * TODO: Docs
  */
int dist_manhattan(int x1, int x2, int y1, int y2)
{
  // TODO: Check abs' speed
  return abs(x2-x1 + y2-y1);
}