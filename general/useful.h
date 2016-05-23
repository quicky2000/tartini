/***************************************************************************
                          useful.h  -  Generic useful functions
                             -------------------
    begin                : 2002
    copyright            : (C) 2002-2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
    copyright            : (C) 2016 by Julien Thevenon
    email                : julien_thevenon at yahoo.fr
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/
#ifndef USEFUL_H
#define USEFUL_H

#include <stdlib.h>

//to get the lrint and lrintf functions we must use the C99 defines
#define _ISOC9X_SOURCE  1
#define _ISOC99_SOURCE  1
#define __USE_ISOC9X  1
#define __USE_ISOC99  1
#include  <math.h>

#define MIN(x,y)    ((x)<(y) ? (x) : (y))
#define MAX(x,y)    ((x)>(y) ? (x) : (y))
#define ABS(x)      ((x)<0 ? -(x) : (x))
#define SIGN(x)     (((x) == 0) ? 0 : (((x)>0) ? 1 : -1))

#define PI  3.14159265358979323846
#define twoPI 6.28318530717958647692

/**
   return a pointer to a 2d array with each element of size size
*/
void **malloc2d(const int row, const int col, const int size);

/**
   like malloc2d but changes an existing 2d array
*/
void **realloc2d(void **ptr, const int row, const int col, const int old_rol, const int old_col, const int size);

/**
   frees the memory created by malloc2d
*/
void free2d(void **ptr, const int row);

/**
   returns x squared
*/
inline double sq(const double x);

/**
   return x to the power of integer y
   works for all values of x and y
*/
double powi(const double x, const int y);

double nearestPowerOf2(double x);

#ifndef pow10
//rasises 10^x
inline double pow10(double x);
#endif // pow10

#ifndef pow2
//rasises 2^x
inline double pow2(double x);
#endif // POW2

#ifdef WINDOWS
//From log rules  log_b(x) = log_a(x) / log_a(b)
inline double log2(double x);
#endif // WINDOWS

inline double logBaseN(double baseN, double x);

/**
   return a random number between 0 and 1 (inclusive)
*/
inline double prand(void);

/**
   cycle is like mod except it deals with negative numbers nicely
*/
inline double cycle(const double a, const double b);

inline int cycle(const int a, const int b);

inline double myround(const double x);

inline int toInt(const float x);

inline int toInt(const double x);

inline int intFloor(const float x);

inline int intFloor(const double x);

/**
   Round x up to the nearest multiple of multiple
*/
inline int roundUp(const int x, const int multiple);

/**
   Returns true if value1 is within plusMinus of value2 (inclusive)
*/
template<class T>
inline bool within(T plusMinus, T value1, T value2);

/**
   Returns true if x is between lowerBound and upperBound (inclusive)
*/
template<class T>
inline bool between(T x, T lowerBound, T upperBound);

/**
   makes  lowerBound <= var <= upperBound
*/
template<class T>
inline T bound(T var, T lowerBound, T upperBound);

/**
   Returns the x position of the min/max of a parabola
   @param a, b and c are the values of the function at -1, 0 and 1
   @return The x position, or zero if no solution.
*/
inline double parabolaTurningPoint(double a, double b, double c);

/**
   Calculate the x position and the y position of the min/max of a parabola at the same time
   @param y_1, y0 and y1 are the values of the function at xOffset-1, xOffset and xOffset+1
   @param xOffset the x value at which y0 is a function of
   @param x The x result will be put in here
   @param y The y result will be put in here
*/
template<class T>
inline void parabolaTurningPoint2(T y_1, T y0, T y1, T xOffset, T *x, T *y);

struct MinMax
{
  inline MinMax(void);
  inline MinMax(float min_, float max_);
  float min;
  float max;
};

/**
   Returns the minimum & maximum values between two pointers
*/
MinMax minMax(float *begin, float *end);

/**
   Returns the minimum & maximum values between two pointers, ignoring anything outside the two bounds (exclusive)
*/
MinMax minMax(float *begin, float *end, float lowBound, float highBound);

/**
   Returns the average value between two pointers
*/
float average(float *begin, float *end);

/**
   Returns the average value between two pointers ignoring anything outside the two bounds (exclusive)
*/
float average(float *begin, float *end, float lowBound, float highBound);

/**
   This guarantees consistent blocks of indices for a given baseX value.
   @param size the array's size
 **/
int calcIndex(const double & frameTime, const double & baseX, int size);

/**
   Return the index with the maximum value in an array.
   If more than one value with the maximum, the first is returned.
   @param aFirst Pointer to the beginning of the array
   @param length The length of the array
   @return The index of the maximum
*/
template <class ForwardIterator>
inline int maxIndex(ForwardIterator aFirst, int length);

/**
   Return the index with the minimum value in an array.
   If more than one value with the minimum, the first is returned.
   @param aFirst Pointer to the beginning of the array
   @param length The length of the array
   @return The index of the minimum
*/
template <class ForwardIterator>
inline int minIndex(ForwardIterator aFirst, int length);

#include <utility>

/**
   minMaxElement returns an iterator to the min and max values between __first and __last, in a pair
   Note: This is like the std::min_element and std::max_element functions, but performs both at the
   same time, so should be more efficient
   @return a std::pair of which .first is an iterator to the min, and .second to the max
*/
template<typename _ForwardIter, typename _Compare>
inline std::pair<_ForwardIter, _ForwardIter> minMaxElement(_ForwardIter __first, _ForwardIter __last, _Compare __lessComp);

/**
   Add sequence 'B' element-wise to sequence 'A' and store result in 'A'
*/
template <class ForwardIterator>
inline void addElements(ForwardIterator aFirst, ForwardIterator aLast, ForwardIterator bFirst);

/**
   Add a scaled sequence 'B' element-wise to sequence 'A' and store result in 'A'
*/
template <class ForwardIterator, class ElementType>
inline void addElements(ForwardIterator aFirst, ForwardIterator aLast, ForwardIterator bFirst, ElementType scaler);

/**
   Copy element-wise 'A' into 'B' scaling the scaler
*/
template <class ForwardIterator, class ElementType>
inline void copyElementsScale(ForwardIterator aFirst, ForwardIterator aLast, ForwardIterator bFirst, ElementType scaler);


/**
   Copy element-wise 'A' into 'B' dividing by div
*/
template <class ForwardIterator, class ElementType>
  void copyElementsDivide(ForwardIterator aFirst, ForwardIterator aLast, ForwardIterator bFirst, ElementType div);

#include <functional>

template <class T>
inline T absolute(const T &x);

template <class T>
struct absoluteLess : public std::binary_function<T, T, bool>
{
  inline bool operator()(const T &x, const T &y)const;
};

template <class T>
struct absoluteGreater : public std::binary_function<T, T, bool>
{
  inline bool operator()(T &x, T &y)const;
};


/**
   Copies the file src to dest.
   If dest already exists it will be overwitten
   @return true on success, false on error
*/
bool copyFile(const char *src, const char *dest);

/**
   Moves the file src to the file dest.
   If src is on the same file system as dest it is just relinked.
   Otherwise it is copied, and the original removed.
   @return true on success, false on error
*/
bool moveFile(const char *src, const char *dest);

/**
   Returns the power of 2 greater or equal to x
*/
int nextPowerOf2(int x);

#include <algorithm>

/**
   Given an ordered sequence, 'A', return an iterator to the closest element to value
*/
template <class ForwardIterator, class ElementType>
inline ForwardIterator binary_search_closest(ForwardIterator aFirst, ForwardIterator aLast, const ElementType &value);

#include <iostream>

/**
   Print out a range of elements to cout. eg [1 2 3 4]
*/
template <class ForwardIterator>
inline ForwardIterator print_elements(ForwardIterator aFirst, ForwardIterator aLast);

#include "useful.hpp"

#endif // USEFUL_H
//EOF
