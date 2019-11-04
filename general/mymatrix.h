/***************************************************************************
                          mymatrix.h
                             -------------------
    begin                : Unknown
    copyright            : Unknown
    email                : Unknown

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   Please read LICENSE.txt for details.
 ***************************************************************************/
#ifndef MYMATRIX_H
#define MYMATRIX_H

void print2x2matrix(const double * p_x);
void print3x3matrix(const double * p_x);

/**
   Matrix inverse assuming the array follows this index pattern.
   [0  1]
   [2  3]
   @return true if the matrix was inverted. false if no solution.
*/
bool invert2x2matrix( const double * p_input
                    , double * p_output
                    );

/**
   Matrix inverse assuming the array follows this index pattern.
   [0 1 2]
   [3 4 5]
   [6 7 8]
   @param p_x matrix to invert
   @param p_y stores the result in y
   @return true if the matrix was inverted. false if no solution.
*/
bool invert3x3matrix( const double * p_x
                    , double * p_y
                    );

/**
   Moore-Penrose Matrix-inverse (or pseudo-inverse) such that X*alpha1 - Y is minimised using least squares
   Ref: http://mathworld.wolfram.com/Moore-PenroseMatrixInverse.html
   @param p_x1 The first (and only) column of X. An array of length n to fit to y
   @param p_y The first (and only) column of Y. Y is the matrix to be fitted to, using least squares
   @param p_n the number of items in x1 and y
   @param p_alpha Array length 1. The result is stored here. The scaler value for x1
   @return true if there is a psudeo-inverse, else false
*/
bool pinv( const float * p_x1
         , const float * p_y
         , int p_n
         , double * p_alpha
         );

/**
   Moore-Penrose Matrix-inverse (or pseudo-inverse) such that X*alpha1 - Y is minimised using least squares
   Ref: http://mathworld.wolfram.com/Moore-PenroseMatrixInverse.html
   @param p_x1 The first column of X or NULL for a column of 1s. An array of length n to fit to y
   @param p_x2 The second column of X. An array of length n to fit to y
   @param p_y The first (and only) column of Y. Y is the matrix to be fitted to, using least squares
   @param p_n the number of items in x1 and y
   @param p_alpha Array length 2. The result is stored here. The scaler values for x1 and x2 respectively
   @return true if there is a psudeo-inverse, else false
*/
bool pinv( const float * p_x1
         , const float * p_x2
         , const float * p_y
         , int p_n
         , double * p_alpha
         );

/**
   Moore-Penrose Matrix-inverse (or pseudo-inverse) such that X*alpha1 - Y is minimised using least squares
   Ref: http://mathworld.wolfram.com/Moore-PenroseMatrixInverse.html
   @param p_x1 The first column of X or NULL for a column of 1s. An array of length n to fit to y
   @param p_x2 The second column of X. An array of length n to fit to y
   @param p_x3 The third column of X. An array of length n to fit to y
   @param p_y The first (and only) column of Y. Y is the matrix to be fitted to, using least squares
   @param p_n the number of items in x1 and y
   @param p_alpha Array length 3. The result is stored here. The scaler values for x1, x2 and x3 respectively
   @return true if there is a psudeo-inverse, else false
*/
bool pinv( const float * p_x1
         , const float * p_x2
         , const float * p_x3
         , const float * p_y
         , int p_n
         , double * p_alpha
         );

/**
   Finds the determinant of a 2x2 matrix
   [a b]
   [c d]
*/
inline double determinant2x2( double p_a
                            , double p_b
                            , double p_c
                            , double p_d
                            );

/**
   Finds the determinant of a 3x3 matrix
   [0 1 2]
   [3 4 5]
   [6 7 8]
*/
inline double determinant3x3(const double * p_x);

#include "mymatrix.hpp"

#endif // MYMATRIX_H
//EOF
