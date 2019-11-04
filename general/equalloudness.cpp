/***************************************************************************
                           equalloudness.cpp
                           -------------------
   Interpolates an equal-loudness lookup table to get the loudness of a sound in phons
   at the given frequencies and intensities (given in deci-Bells)
    
    begin                : Nov 2004
    copyright            : (C) 2004-2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
    copyright            : (C) 2016 by Julien Thevenon
    email                : julien_thevenon at yahoo.fr
   
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/

//   Reference:
//   Extraperlated from the equal loudness curves at
//   http://hyperphysics.phy-astr.gsu.edu/hbase/sound/eqloud.html

#include "equalloudness.h"
#include "useful.h"
#include <algorithm>

//These are the sampling intensities in out lookup table (in dB)
double g_i0[14] = {0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 110, 120, 130};

//the best hearing is at 3700Hz. And another max at 13000Hz
//These are the sampling frequencies in out lookup table
double g_f0[23] =                  {  20,  30,  40,  50,  70, 100,    200, 300, 400, 500, 700, 1000, 1600, 2000, 3000, 3700, 5000, 7000, 10000, 12000, 13000, 14000, 16000};

double g_loudness_table[16][23] = {{ -30, -30, -30, -30, -30, -30,    -30, -30, -25, -22, -20,  -20,  -20,  -20,  -20,  -15,  -17,  -30,   -30,   -30,   -30,   -30,   -30}, //-20dB
                                   { -30, -30, -30, -30, -30, -30,    -30, -20, -15, -12, -10,  -10,   -8,   -8,   -5,   -2,   -6,  -20,   -30,   -30,   -30,   -30,   -30}, //-10dB
                                   { -30, -30, -30, -30, -30, -30,    -20, -10,  -5,  -2,   0,    0,    2,    2,    6,    9,    5,  -10,   -20,   -25,   -22,   -30,   -30}, // 0dB
                                   { -30, -30, -30, -30, -30, -27,      0,   6,   9,  10,  10,   10,   10,   12,   17,   18,   15,    4,    -1,     2,     0,    -5,   -30}, //10dB
                                   { -30, -30, -30, -30, -20,  -5,     12,  18,  20,  20,  20,   20,   20,   23,   27,   28,   25,    5,     9,    12,    12,     5,   -30}, //20dB
                                   { -30, -30, -28, -20,   0,  11,     25,  30,  31,  32,  32,   30,   31,   33,   37,   38,   36,   25,    21,    24,    30,    18,   -30}, //30dB
                                   { -30, -25, -10,   0,  15,  23,     38,  42,  44,  45,  44,   40,   41,   43,   48,   50,   47,   37,    31,    36,    42,    36,   -30}, //40dB
                                   { -30, -10,   4,  14,  28,  38,     50,  52,  54,  55,  52,   50,   51,   53,   57,   59,   57,   48,    44,    47,    52,    48,   -30}, //50dB
                                   { -15,   5,  20,  30,  43,  50,     60,  63,  65,  65,  62,   60,   61,   64,   68,   70,   58,   58,    54,    59,    64,    60,   -20}, //60dB
                                   {   0,  22,  40,  48,  58,  62,     72,  75,  76,  76,  73,   70,   72,   75,   79,   81,   78,   67,    66,    68,    74,    70,   -10}, //70dB
                                   {  18,  41,  55,  61,  70,  75,     72,  84,  85,  85,  82,   80,   81,   85,   90,   91,   88,   78,    75,    78,    82,    80,     0}, //80dB
                                   {  38,  60,  71,  75,  82,  86,     92,  94,  94,  93,  92,   90,   92,   96,  101,  103,  100,   89,    86,    88,    92,    90,    10}, //90dB
                                   {  58,  75,  83,  88,  93,  97,    101, 103, 103, 103, 102,  100,  103,  107,  113,  115,  110,  100,    95,    96,    97,    95,    20}, //100dB
                                   {  75,  88,  95,  99, 103, 107,    110, 111, 111, 112, 111,  110,  112,  117,  124,  125,  120,  109,   105,   105,   105,   100,    30}, //110dB
                                   {  89, 100, 106, 110, 113, 115,    119, 120, 120, 120, 120,  120,  122,  127,  134,  133,  128,  118,   113,   111,   109,   105,    40}, //120dB
                                   { 102, 112, 117, 120, 122, 124,    128, 129, 130, 130, 130,  130,  132,  135,  142,  140,  137,  131,   128,   121,   111,   104,    50}//130dB
                                  };

//------------------------------------------------------------------------------
double lookupLoudnessTable(double p_row, double p_col)
{
  p_row += 2; //0dB is second row in table
  int l_irow = int(floor(p_row)); //floor to ensure -ve number are still rounded down
  int l_irow2 = l_irow + 1;
  double l_rrow = p_row - double(l_irow); //fractional remainder
  if(l_irow < 0)
    {
      return -30.0; //{ l_irow = 0; l_irow2 = 0; l_rrow = 0.0; }
    }
  if(l_irow >= 13)
    {
        l_irow = 13;
        l_irow2 = 13;
        l_rrow = 0.0;
    }
  
  int l_icol = int(floor(p_col));
  int l_icol2 = l_icol + 1;
  double l_rcol = p_col - double(l_icol); //fractional remainder
#ifdef DEBUG_PRINTF
  printf("l_rcol = %lf, %lf, %lf\n", p_col, double(p_col), l_rcol);
#endif // DEBUG_PRINTF
  if(l_icol < 0)
    {
        l_icol = 0;
        l_icol2 = 0;
        l_rcol = 0.0;
    }
  if(l_icol >= 22)
    {
        l_icol = 22;
        l_icol2 = 22;
        l_rcol = 0.0;
    }
#ifdef DEBUG_PRINTF
  printf("l_icol=%d, %d, %lf\n", l_icol, l_icol2, l_rcol);
#endif // DEBUG_PRINTF
  
  //[l_a l_b] -> [l_e]
  //[l_c l_d] -> [l_f]
  double &l_a = g_loudness_table[l_irow][l_icol];
  double &l_b = g_loudness_table[l_irow][l_icol2];
  double &l_c = g_loudness_table[l_irow2][l_icol];
  double &l_d = g_loudness_table[l_irow2][l_icol2];

  double l_e = l_a + (l_b - l_a) * l_rcol;
  double l_f = l_c + (l_d - l_c) * l_rcol;

  return l_e + (l_f - l_e) * l_rrow;
}

//------------------------------------------------------------------------------
double dbToPhons(const double & p_freq, const double & p_intensity)
{
  //Todo: Finish writing this function
  double l_row = p_intensity / 10.0;
  double *l_p_col = std::upper_bound(g_f0, g_f0 + 23, p_freq);
  int l_icol = int(l_p_col - g_f0);
#ifdef DEBUG_PRINTF
  printf("%d\n", l_icol);
#endif // DEBUG_PRINTF
  if(l_icol <= 0)
    {
      return lookupLoudnessTable(l_row, 0.0);
    }
  else if(l_icol >= 23)
    {
      return lookupLoudnessTable(l_row, 23.0);
    }
  else
    {
      int l_icol0 = MAX(0, l_icol - 1);
      double &l_a = g_f0[l_icol0];
      double &l_b = g_f0[l_icol];
      double l_col = double(l_icol0) + (p_freq - l_a) / (l_b - l_a);
#ifdef DEBUG_PRINTF
      printf("%lf, %lf, %lf, %lf, %lf\n", l_a, l_b, p_freq, (p_freq - l_a) / (l_b - l_a), l_col);
#endif // DEBUG_PRINTF
      return lookupLoudnessTable(l_row, l_col);
    }
}

//EOF
