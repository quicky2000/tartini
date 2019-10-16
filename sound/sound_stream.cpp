/***************************************************************************
                          sound_stream.cpp  -  description
                             -------------------
    begin                : Sat Nov 27 2004
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

#include "sound_stream.h"
#include <stdio.h>

const double g_v8=0x7F, g_v16=0x7FFF, g_v32=0x7FFFFFFF;

int SoundStream::writeFloats(float **p_channel_data, int p_length, int p_ch)
{
    int l_written;
    int l_c;

    if(m_bits == 8) {
        unsigned char *l_temp = new unsigned char[p_length * m_channels];
        unsigned char *l_pos1 = l_temp;
        float *l_pos, *l_end;
        for(l_c=0; l_c < m_channels; l_c++) {
            l_end = p_channel_data[l_c % p_ch] + p_length;
            l_pos1 = l_temp + l_c;
            for(l_pos = p_channel_data[l_c % p_ch]; l_pos < l_end; l_pos++, l_pos1+=m_channels)
                *l_pos1 = (unsigned char)((*l_pos * g_v8) + g_v8);
        }
        l_written = write_bytes(l_temp, p_length * m_channels);
        delete[] l_temp;
        return l_written / m_channels;
    } else if(m_bits == 16) {
        short *l_temp = new short[p_length * m_channels * sizeof(short)];
        short *l_pos1 = l_temp;
        float *l_pos, *l_end;
        for(l_c=0; l_c < m_channels; l_c++) {
            l_end = p_channel_data[l_c % p_ch] + p_length;
            l_pos1 = l_temp + l_c;
            for(l_pos = p_channel_data[l_c % p_ch]; l_pos < l_end; l_pos++, l_pos1+=m_channels) {
                *l_pos1 = (short)(*l_pos * g_v16);
//#ifdef Q_OS_MACX //reverse endieness
#ifdef MACX //reverse endieness
                *l_pos1 = ((*l_pos1 & 0xFF00) >> 8) | ((*l_pos1 & 0x00FF) << 8);
#endif
            }
        }
        l_written = write_bytes(l_temp, p_length * m_channels * sizeof(short));
        delete[] l_temp;
        return l_written / m_channels / sizeof(short);
    }
    fprintf(stderr, "%d bit data is not suported yet\n", m_bits);
    return 0;
}

int SoundStream::readFloats(float **p_channel_data, int p_length, int p_ch)
{
    int l_read;
    int l_c;
    if(m_bits == 8) {
        unsigned char *l_temp = new unsigned char[p_length * m_channels];
        l_read = read_bytes(l_temp, p_length * m_channels);
        unsigned char *l_pos1 = l_temp;
        float *l_pos, *l_end;
        for(l_c=0; l_c < m_channels; l_c++) {
            l_end = p_channel_data[l_c % p_ch] + p_length;
            l_pos1 = l_temp + l_c;
            for(l_pos = p_channel_data[l_c % p_ch]; l_pos < l_end; l_pos++, l_pos1+=m_channels)
                *l_pos = float((double(*l_pos1) - g_v8) / g_v8);
        }
        delete[] l_temp;
        return l_read / m_channels;
    } else if(m_bits == 16) {
        short *l_temp = new short[p_length * m_channels * sizeof(short)];
        l_read = read_bytes(l_temp, p_length * m_channels * sizeof(short));
        short *l_pos1 = l_temp;
        float *l_pos, *l_end;
        for(l_c=0; l_c < m_channels; l_c++) {
            l_end = p_channel_data[l_c % p_ch] + p_length;
            l_pos1 = l_temp + l_c;
            for(l_pos = p_channel_data[l_c % p_ch]; l_pos < l_end; l_pos++, l_pos1+=m_channels) {
//#ifdef Q_OS_MACX //reverse endieness
#ifdef MACX //reverse endieness
                *l_pos1 = ((*l_pos1 & 0xFF00) >> 8) | ((*l_pos1 & 0x00FF) << 8);
#endif
                *l_pos = float(double(*l_pos1) / g_v16);
            }
        }
        delete[] l_temp;
        return l_read / m_channels / sizeof(short);
    } else if(m_bits == 32) {
        long *l_temp = new long[p_length * m_channels * sizeof(long)];
        l_read = read_bytes(l_temp, p_length * m_channels * sizeof(long));
        long *l_pos1 = l_temp;
        float *l_pos, *l_end;
        for(l_c=0; l_c < m_channels; l_c++) {
            l_end = p_channel_data[l_c % p_ch] + p_length;
            l_pos1 = l_temp + l_c;
            for(l_pos = p_channel_data[l_c % p_ch]; l_pos < l_end; l_pos++, l_pos1+=m_channels) {
//#ifdef Q_OS_MACX //reverse endieness
//                *l_pos1 = ((*l_pos1 & 0xFF00) >> 8) | ((*l_pos1 & 0x00FF) << 8);
//#endif
                *l_pos = float(double(*l_pos1) / g_v16);
            }
        }
        delete[] l_temp;
        return l_read / m_channels / sizeof(long);
    }
    //TODO: 24bit
    fprintf(stderr, "%d bit data is not suported yet\n", m_bits);
    return 0;
}

int SoundStream::writeReadFloats(float **p_out_channel_data, int p_out_ch, float **p_in_channel_data, int p_in_ch, int p_length)
{
  writeFloats(p_out_channel_data, p_length, p_out_ch);
  return readFloats(p_in_channel_data, p_length, p_in_ch);
  //not implimented yet.
  //return 0;
}
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
