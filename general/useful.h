/***************************************************************************
                          useful.h  -  Generic useful functions
                             -------------------
    begin                : 2002
    copyright            : (C) 2002-2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/
#ifndef USEFUL_H
#define USEFUL_H

#include <stdlib.h>

#if (defined (WIN32) || defined (_WIN32))
  #include  <math.h>
  /*  Win32 doesn't seem to have the lrint or lrintf functions.
  **  Therefore implement inline versions of these functions here.  */
  __inline long int 
  lrint (double flt)
  { int intgr;
    _asm { fld flt
      fistp intgr
      } ;
    return intgr ;
  } 
  
  __inline long int 
  lrintf (float flt)
  { int intgr;
    _asm
    { fld flt
      fistp intgr
      } ;
    return intgr ;
  }
#else

//to get the lrint and lrintf functions we must use the C99 defines
#define _ISOC9X_SOURCE  1
#define _ISOC99_SOURCE  1
#define __USE_ISOC9X  1
#define __USE_ISOC99  1
#include  <math.h>
#endif /*(defined (WIN32) || defined (_WIN32))*/

#define MIN(x,y)    ((x)<(y) ? (x) : (y))
#define MAX(x,y)    ((x)>(y) ? (x) : (y))
//#define MID(x,y,z)  MAX(MIN(x,y),z)
#define ABS(x)      ((x)<0 ? -(x) : (x))
#define SIGN(x)     (((x) == 0) ? 0 : (((x)>0) ? 1 : -1))

#define PI  3.14159265358979323846
#define twoPI 6.28318530717958647692

//return a pointer to a 2d array with each element of size size
void **malloc2d(const int row, const int col, const int size);
//like malloc2d but changes an existing 2d array
void **realloc2d(void **ptr, const int row, const int col, const int old_rol, const int old_col, const int size);
//frees the memory created by malloc2d
void free2d(void **ptr, const int row);

//returns x squared
inline double sq(const double x)
{
  return x*x;
}

//return x to the power of integer y
//works for all values of x and y
double powi(const double x, const int y);

double nearestPowerOf2(double x);

#ifndef pow10
inline double pow10(double x) { return pow(10.0, x); }
#endif

#ifndef pow2
inline double pow2(double x) { return pow(2.0, x); }
#endif

#ifdef WINDOWS
//From log rules  log_b(x) = log_a(x) / log_a(b)
inline double log2(double x) { return log(x) / 0.69314718055994530941723212145818; }
#endif

//return a random number between 0 and 1 (inclusive)
inline double prand()
{
  return double(rand()) / double(RAND_MAX);
}

/*cycle is like mod except it deals with negative numbers nicely*/
inline double cycle(const double a, const double b)
{
    if(a >= 0.0) return fmod(a, b);
    else return b + fmod(a, b);
}

inline int cycle(const int a, const int b)
{
    if(a >= 0) return a % b;
    else return b + ((a+1) % b) - 1;
}

inline double myround(const double x)
{
  return floor(x + 0.5);
}

inline int toInt(const float x)
{
  return lrintf(x);
  //return int(floor(x + 0.5));
}

inline int toInt(const double x)
{
  return lrint(x);
  //return int(floor(x + 0.5));
}

/*Round x up to the nearest multiple of multiple*/
inline int roundUp(const int x, const int multiple)
{
  int remainder = x % multiple;
  return (remainder == 0) ? x : x - remainder + multiple;
}

/** Returns true if value1 is within plusMinus of value2 (inclusive)
*/
template<class T>
inline bool within(T plusMinus, T value1, T value2)
{
  return (value1 >= value2-plusMinus) ? (value1 <= value2+plusMinus) : false;
}

//makes  lowerBound <= var <= upperBound
template<class T>
T bound(T var, T lowerBound, T upperBound)
{
  if(var < lowerBound) var = lowerBound;
  if(var > upperBound) var = upperBound;
  return var;
}

/** Returns the x position of the min/max of a parabola
  @param a, b and c are the values of the function at -1, 0 and 1
  @return The x position, or zero if no solution.
*/
inline double parabolaTurningPoint(double a, double b, double c)
{
  double bottom = (2*(c + a - 2*b));
  return (bottom == 0.0) ? 0.0 : ((a - c) / bottom);
}

/** Calculate the x position and the y position of the min/max of a parabola at the same time
  @param y_1, y0 and y1 are the values of the function at xOffset-1, xOffset and xOffset+1
  @param xOffset the x value at which y0 is a function of
  @param x The x result will be put in here
  @param y The y result will be put in here
*/
template<class T>
inline void parabolaTurningPoint2(T y_1, T y0, T y1, T xOffset, T *x, T *y)
{
  T yTop = y_1 - y1;
  T yBottom = y1 + y_1 - 2 * y0;
  if(yBottom != 0.0) {
    *x = xOffset + yTop / (2 * yBottom);
    *y = y0 - (sq(yTop) / (8 * yBottom));
  } else {
    *x = xOffset;
    *y = y0;
  }
}

/** Returns the fractional note (from the midi scale)
@param freq The frequency in Hz
@return The note in fractional part semitones from the midi scale.
*/
/*
inline double freq2note(double freq)
{
  return log10(freq/27.5) / 0.025085832972;
}
*/

/** Converts the frequencies freq (in hertz) into their note number on the midi scale
    i.e. the number of semi-tones above C0
    Note: The note will contain its fractional part
    Reference = http://www.borg.com/~jglatt/tutr/notenum.htm
  @param freq The frequency in Hz
  @return The note in fractional part semitones from the midi scale.
*/
inline double freq2note(double freq)
{
#ifdef log2
	//return 69 + 12*log2(freq / 440);
	//From log rules  log(x/y) = log(x) - log(y)
	//return 69 + 12*(log2(freq) - log2(440));
	return -36.3763165622959152488 + 12.0*log2(freq);
#else
	//From log rules  log_b(x) = log_a(x) / log_a(b)
	//return 69 + 39.8631371386483481*log10(freq / 440);
	return -36.3763165622959152488 + 39.8631371386483481*log10(freq);
#endif
}

/** Does the opposite of the function above
*/
inline double note2freq(double note)
{
  return pow10((note + 36.3763165622959152488) / 39.8631371386483481);
}

char* noteName(int note);
inline char* noteName(double note) { return noteName(toInt(note)); }

int noteOctave(int note);
inline int noteOctave(double note) { return noteOctave(toInt(note)); }

int noteValue(int note);
inline int noteValue(double note) { return noteValue(toInt(note)); }

bool isBlackNote(int note);
inline bool isBlackNote(double note) { return isBlackNote(toInt(note)); }

/*
struct MinMax
{
    MinMax::MinMax() : min(0.0) , max(0.0) {};
    MinMax::MinMax(float min_, float max_) : min(min_) , max(max_) {};
    float min;
    float max;
};
*/
struct MinMax
{
    MinMax::MinMax() 
	{
		min = 0.0;
		max = 0.0;
	};
    MinMax::MinMax(float min_, float max_) 
	{
		min = min_;
		max = max_;
	};
    float min;
    float max;
};
/* Returns the minimum & maximum values between two pointers */
MinMax minMax(float *begin, float *end);
/* Returns the minimum & maximum values between two pointers, ignoring anything outside the two bounds (exclusive) */
MinMax minMax(float *begin, float *end, float lowBound, float highBound);
/* Returns the average value between two pointers */
float average(float *begin, float *end);
/* Returns the average value between two pointers ignoring anything outside the two bounds (exclusive) */
float average(float *begin, float *end, float lowBound, float highBound);
int calcIndex(double frameTime, double baseX, int size);


/*//makes  lowerBound <= var <= upperBound
template<class T>
void bound(T *var, T lowerBound, T upperBound)
{
  if(*var < lowerBound) *var = lowerBound;
  if(*var > upperBound) *var = upperBound;
}
*/



#include <utility>

/** minMaxElement returns an iterator to the min and max values between __first and __last, in a pair
    Note: This is like the std::min_element and std::max_element functions, but performs both at the
    same time, so should be more efficient
    @return a std::pair of which .first is an iterator to the min, and .second to the max
*/
//template<typename _ForwardIter, typename _Compare1, typename _Compare2>
//std::pair<_ForwardIter, _ForwardIter> minMaxElement(_ForwardIter __first, _ForwardIter __last, _Compare1 __lessComp, _Compare2 __greaterComp)
template<typename _ForwardIter, typename _Compare>
std::pair<_ForwardIter, _ForwardIter> minMaxElement(_ForwardIter __first, _ForwardIter __last, _Compare __lessComp)
{
  // concept requirements
  //__glibcpp_function_requires(_ForwardIteratorConcept<_ForwardIter>)
  //__glibcpp_function_requires(_BinaryPredicateConcept<_Compare,
  //typename iterator_traits<_ForwardIter>::value_type,
  //typename iterator_traits<_ForwardIter>::value_type>)

  std::pair<_ForwardIter, _ForwardIter> __result(__first, __first);
  if (__first == __last) return __result;
  while (++__first != __last) {
	  if (__lessComp(*__first, *__result.first)) __result.first = __first;
    //if (__greaterComp(*__first, *__result.second)) __result.second = __first;
    if (__lessComp(*__result.second, 
    *__first))
     __result.second = __first;
  }
  return __result;
}

#include <functional>

struct absoluteLess : public std::binary_function<float, float, float>
{
  float operator()(float &x, float &y) const { return fabs(x) < fabs(y); }
};

bool copyFile(const char *src, const char *dest);
bool moveFile(const char *src, const char *dest);

#endif
