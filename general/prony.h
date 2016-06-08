/***************************************************************************
                          prony.h
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
#ifndef PRONY_H
#define PRONY_H

const double Pi = 3.1415926535897932384;
const double TwoPi = 3.1415926535897932384 * 2.0;
const double HalfPi = 3.1415926535897932384 * 0.5;

/**
   fitted parameters
*/
struct PronyData
{ 
  /**
     amplitude
  */
  double amp;

  /**
     phase in radians
   */
  double phase;

  /**
     frequency in radians per unit
  */
  double omega;

  double yOffset;

  /**
     squared error of the fit
  */
  double error;   

  PronyData(void)
  {
    reset();
  }
  
  void reset(void)
  {
    amp = phase = omega = yOffset = error = 0.0;
  }

  /**
     @param dt is the time between samples in the input.
  */
  double freqHz(double dt)
  {
    return omega / (dt * TwoPi);
  }
};

/**
   Prony Spectral Line Extimation (see S. Kay "Spectrum Analysis - A Modern Perspective" Pro. IEEE, Vol 69, #11, Nov 1981)
   @param result Where the results of the calculation are stored
   @param x The data to process
   @param length The number of elements in x
   @param gap The number samples used as a delay size (best is around 1/4 of a cycle)
   @return true if the prony fit was a success. else false 
*/
bool pronyFit(PronyData *result, const float *x, int length, int gap, bool allowOffset);

#endif // PRONY_H
// EOF
