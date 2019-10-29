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

const double g_pi = 3.1415926535897932384;
const double g_two_pi = 3.1415926535897932384 * 2.0;
const double g_half_pi = 3.1415926535897932384 * 0.5;

/**
   fitted parameters
*/
struct PronyData
{ 
  /**
     amplitude
  */
  double m_amp;

  /**
     phase in radians
   */
  double m_phase;

  /**
     frequency in radians per unit
  */
  double m_omega;

  double m_y_offset;

  /**
     squared error of the fit
  */
  double m_error;

  PronyData(void)
  {
    reset();
  }
  
  void reset(void)
  {
      m_amp = m_phase = m_omega = m_y_offset = m_error = 0.0;
  }

  /**
     @param p_dt is the time between samples in the input.
  */
  double freqHz(double p_dt)
  {
    return m_omega / (p_dt * g_two_pi);
  }
};

/**
   Prony Spectral Line Extimation (see S. Kay "Spectrum Analysis - A Modern Perspective" Pro. IEEE, Vol 69, #11, Nov 1981)
   @param p_result Where the results of the calculation are stored
   @param p_x The data to process
   @param p_length The number of elements in x
   @param p_gap The number samples used as a delay size (best is around 1/4 of a cycle)
   @return true if the prony fit was a success. else false 
*/
bool pronyFit(PronyData *p_result, const float *p_x, int p_length, int p_gap, bool p_allow_offset);

#endif // PRONY_H
// EOF
