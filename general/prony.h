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
class PronyData
{
  public:
    inline PronyData();
  
    inline void reset();

    /**
       @param p_dt is the time between samples in the input.
    */
    inline double freqHz(double p_dt);

    inline void set_omega(const double & p_omega);
    inline void set_y_offset(const double & p_y_offset);
    inline void set_amplitude(const double & p_amplitude);
    inline void set_phase(const double & p_phase);
    inline void set_error(const double & p_error);

    inline const double & get_amplitude() const;
    inline const double & get_phase() const;
    inline const double & get_omega() const;
    inline const double & get_y_offset() const;
    inline const double & get_error() const;

  private:

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

};

/**
   Prony Spectral Line Extimation (see S. Kay "Spectrum Analysis - A Modern Perspective" Pro. IEEE, Vol 69, #11, Nov 1981)
   @param p_result Where the results of the calculation are stored
   @param p_x The data to process
   @param p_length The number of elements in x
   @param p_gap The number samples used as a delay size (best is around 1/4 of a cycle)
   @return true if the prony fit was a success. else false 
*/
bool pronyFit( PronyData * p_result
             , const float * p_x
             , int p_length
             , int p_gap
             , bool p_allow_offset
             );

//-----------------------------------------------------------------------------
void PronyData::set_omega(const double & p_omega)
{
    m_omega = p_omega;
}

//-----------------------------------------------------------------------------
PronyData::PronyData()
{
    reset();
}

//-----------------------------------------------------------------------------
void PronyData::reset()
{
    m_amp = 0.0;
    m_phase = 0.0;
    m_omega = 0.0;
    m_y_offset = 0.0;
    m_error = 0.0;
}

//-----------------------------------------------------------------------------
double PronyData::freqHz(double p_dt)
{
    return m_omega / (p_dt * g_two_pi);
}

//-----------------------------------------------------------------------------
void
PronyData::set_y_offset(const double & p_y_offset)
{
    m_y_offset = p_y_offset;
}

//-----------------------------------------------------------------------------
void
PronyData::set_amplitude(const double & p_amplitude)
{
    m_amp = p_amplitude;
}

//-----------------------------------------------------------------------------
void
PronyData::set_phase(const double & p_phase)
{
    m_phase = p_phase;
}

//-----------------------------------------------------------------------------
void
PronyData::set_error(const double & p_error)
{
    m_error = p_error;
}

//-----------------------------------------------------------------------------
const double &
PronyData::get_amplitude() const
{
    return m_amp;
}

//-----------------------------------------------------------------------------
const double &
PronyData::get_phase() const
{
    return m_phase;
}

//-----------------------------------------------------------------------------
const double &
PronyData::get_y_offset() const
{
    return m_y_offset;
}

//-----------------------------------------------------------------------------
const double &
PronyData::get_error() const
{
    return m_error;
}

//-----------------------------------------------------------------------------
const double &
PronyData::get_omega() const
{
    return m_omega;
}

#endif // PRONY_H
// EOF
