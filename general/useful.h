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
void ** malloc2d( const int p_row
                , const int p_col
                , const int p_size
                );

/**
   like malloc2d but changes an existing 2d array
*/
void ** realloc2d( void ** p_ptr
                 , const int p_row
                 , const int p_col
                 , const int p_old_row
                 , const int p_old_col
                 , const int p_size
                 );

/**
   frees the memory created by malloc2d
*/
void free2d( void ** p_ptr
           , const int p_row
           );

/**
   returns x squared
*/
inline double sq(const double & p_x);

/**
   return x to the power of integer y
   works for all values of x and y
*/
double powi( const double p_x
           , const int p_y
           );

double nearestPowerOf2(double p_x);

#ifndef pow10
//rasises 10^x
inline double pow10(const double p_x);
#endif // pow10

#ifndef pow2
//rasises 2^x
inline double pow2(const double & p_x);
#endif // POW2

#ifdef WINDOWS
//From log rules  log_b(x) = log_a(x) / log_a(b)
inline double log2(const double & p_x);
#endif // WINDOWS

inline double logBaseN( const double & p_base_N
                      , const double & p_x
                      );

/**
   return a random number between 0 and 1 (inclusive)
*/
inline double prand();

/**
   cycle is like mod except it deals with negative numbers nicely
*/
inline double cycle( const double & p_a
                   , const double & p_b
                   );

inline int cycle( const int p_a
                , const int p_b
                );

inline double myround(const double & p_x);

inline int toInt(const float p_x);

inline int toInt(const double & p_x);

inline int intFloor(const float p_x);

inline int intFloor(const double & p_x);

/**
   Round x up to the nearest multiple of multiple
*/
inline int roundUp( const int p_x
                  , const int p_multiple
                  );

/**
   Returns true if value1 is within plusMinus of value2 (inclusive)
*/
template<class T>
inline bool within( T p_plus_minus
                  , T p_value_1
                  , T p_value_2
                  );

/**
   Returns true if x is between lowerBound and upperBound (inclusive)
*/
template<class T>
inline bool between( T p_x
                   , T p_lower_bound
                   , T p_upper_bound
                   );

/**
   makes  lowerBound <= var <= upperBound
*/
template<class T>
inline T bound( T p_var
              , T p_lower_bound
              , T p_upper_bound
              );

/**
   Returns the x position of the min/max of a parabola
   @param p_a, p_b and c are the values of the function at -1, 0 and 1
   @return The x position, or zero if no solution.
*/
inline double parabolaTurningPoint( const double & p_a
                                  , const double & p_b
                                  , const double & p_c
                                  );

/**
   Calculate the x position and the y position of the min/max of a parabola at the same time
   @param p_y_1, p_y0 and y1 are the values of the function at xOffset-1, xOffset and xOffset+1
   @param p_x_offset the x value at which y0 is a function of
   @param p_x The x result will be put in here
   @param p_y The y result will be put in here
*/
template<class T>
inline void parabolaTurningPoint2( T p_y_1
                                 , T p_y0
                                 , T p_y1
                                 , T p_x_offset
                                 , T * p_x
                                 , T * p_y
                                 );

class MinMax
{
  public:

    inline MinMax();
    inline MinMax( float p_min
                 , float p_max
                 );

    inline void set_min(float p_min);
    inline void set_max(float p_max);
    inline float get_min() const;
    inline float get_max() const;

  private:

    float m_min;
    float m_max;
};

/**
   Returns the minimum & maximum values between two pointers
*/
MinMax minMax( float * p_begin
             , float * p_end
             );

/**
   Returns the minimum & maximum values between two pointers, ignoring anything outside the two bounds (exclusive)
*/
MinMax minMax( float * p_begin
             , float * p_end
             , float p_low_bound
             , float p_high_bound
             );

/**
   Returns the average value between two pointers
*/
float average( float * p_begin
             , float * p_end
             );

/**
   Returns the average value between two pointers ignoring anything outside the two bounds (exclusive)
*/
float average( float * p_begin
             , float * p_end
             , float p_low_bound
             , float p_high_bound
             );

/**
   This guarantees consistent blocks of indices for a given baseX value.
   @param p_size the array's size
 **/
int calcIndex( const double & p_frame_time
             , const double & p_base_X
             , int p_size
             );

/**
   Return the index with the maximum value in an array.
   If more than one value with the maximum, the first is returned.
   @param p_attay_first Pointer to the beginning of the array
   @param p_length The length of the array
   @return The index of the maximum
*/
template <class ForwardIterator>
inline int maxIndex( ForwardIterator p_attay_first
                   , int p_length
                   );

/**
   Return the index with the minimum value in an array.
   If more than one value with the minimum, the first is returned.
   @param p_array_first Pointer to the beginning of the array
   @param p_length The length of the array
   @return The index of the minimum
*/
template <class ForwardIterator>
inline int minIndex( ForwardIterator p_array_first
                   , int p_length
                   );

#include <utility>

/**
   minMaxElement returns an iterator to the min and max values between __first and __last, in a pair
   Note: This is like the std::min_element and std::max_element functions, but performs both at the
   same time, so should be more efficient
   @return a std::pair of which .first is an iterator to the min, and .second to the max
*/
template<typename _ForwardIter, typename _Compare>
inline std::pair<_ForwardIter, _ForwardIter> minMaxElement( _ForwardIter p_first
                                                          , _ForwardIter p_last
                                                          , _Compare p_less_comp
                                                          );

/**
   Add sequence 'B' element-wise to sequence 'A' and store result in 'A'
*/
template <class ForwardIterator>
inline void addElements( ForwardIterator p_a_first
                       , ForwardIterator p_a_last
                       , ForwardIterator p_b_first
                       );

/**
   Add a scaled sequence 'B' element-wise to sequence 'A' and store result in 'A'
*/
template <class ForwardIterator, class ElementType>
inline void addElements( ForwardIterator p_a_first
                       , ForwardIterator p_a_last
                       , ForwardIterator p_b_first
                       , ElementType p_scaler
                       );

/**
   Copy element-wise 'A' into 'B' scaling the scaler
*/
template <class ForwardIterator, class ElementType>
inline void copyElementsScale( ForwardIterator p_a_first
                             , ForwardIterator p_a_last
                             , ForwardIterator p_b_first
                             , ElementType p_scaler
                             );


/**
   Copy element-wise 'A' into 'B' dividing by div
*/
template <class ForwardIterator, class ElementType>
  void copyElementsDivide( ForwardIterator p_a_first
                         , ForwardIterator p_a_last
                         , ForwardIterator p_b_first
                         , ElementType p_div
                         );

#include <functional>

template <class T>
inline T absolute(const T & p_x);

template <class T>
struct absoluteLess : public std::binary_function<T, T, bool>
{
  inline bool operator()( const T & p_x
                        , const T & p_y
                        )const;
};

template <class T>
struct absoluteGreater : public std::binary_function<T, T, bool>
{
  inline bool operator()( T & p_x
                        , T & p_y
                        )const;
};


/**
   Copies the file src to dest.
   If dest already exists it will be overwitten
   @return true on success, false on error
*/
bool copyFile( const char * p_src
             , const char * p_dest
             );

/**
   Moves the file src to the file dest.
   If src is on the same file system as dest it is just relinked.
   Otherwise it is copied, and the original removed.
   @return true on success, false on error
*/
bool moveFile( const char * p_src
             , const char * p_dest
             );

/**
   Returns the power of 2 greater or equal to x
*/
int nextPowerOf2(int p_x);

#include <algorithm>

/**
   Given an ordered sequence, 'A', return an iterator to the closest element to value
*/
template <class ForwardIterator, class ElementType>
inline ForwardIterator binary_search_closest( ForwardIterator p_a_first
                                            , ForwardIterator p_a_last
                                            , const ElementType & p_value
                                            );

#include <iostream>

/**
   Print out a range of elements to cout. eg [1 2 3 4]
*/
template <class ForwardIterator>
inline ForwardIterator print_elements( ForwardIterator p_a_first
                                     , ForwardIterator p_a_last
                                     );

#include "useful.hpp"

#endif // USEFUL_H
//EOF
