/***************************************************************************
                          wave_stream.cpp  -  description
                             -------------------
    begin                : 2003
    copyright            : (C) 2003-2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/
#include <string.h>
#include "myio.h"
#include "wave_stream.h"
#include <algorithm>
#include <errno.h>
#include "useful.h"

//------------------------------------------------------------------------------
WaveStream::WaveStream(void)
: m_file(NULL)
, m_header_length(0)
{
}

//------------------------------------------------------------------------------
WaveStream::~WaveStream(void)
{
    close();
}

//------------------------------------------------------------------------------
void WaveStream::close(void)
{
    if(m_file)
    {
        if(get_mode() & F_WRITE)
        {
            write_header();
        }
        fclose(m_file);
        m_file = NULL;
    }
}

//------------------------------------------------------------------------------
int WaveStream::open_read(const char *p_filename)
{
    m_file = fopen(p_filename, "rb");
    if(!m_file)
    {
        return -1;
    }
    set_mode(F_READ);
    setPos(0);
    return read_header();
}

//------------------------------------------------------------------------------
int WaveStream::read_header(void)
{
    if(!m_file || !(get_mode() & F_READ))
    {
        return -1;
    }

    int l_len;
    int l_data_type;
    char l_buffer[12];

    // check RIFF header
    if(fread(l_buffer, 1, 12, m_file) != 12)
    {
        goto missing_data;
    }
    if(memcmp(l_buffer, "RIFF", 4) || memcmp(l_buffer+8, "WAVE", 4))
    {
        fprintf(stderr, "WaveStream: Not a wave file\n");
        return -1;
    }
    //read format chunk
    if(fread(l_buffer, 1, 4, m_file) != 4)
    {
        goto missing_data;
    }
    if (memcmp(l_buffer, "fmt ", 4) != 0)
    {
        fprintf(stderr, "WaveStream: Format chunk not found\n");
        return -1;
    }

    // read chunk length
    l_len = igetl(m_file);
  
    // should be 1 for PCM data
    l_data_type = igetw(m_file);

    if(l_data_type != 1)
    {
        fprintf(stderr, "WaveStream: Not PCM data\n");
        return -1;
    }
    // TODO: 3 = float data (32bit usually)
  
    // mono or stereo data
    set_channels(igetw(m_file));
    fprintf(stderr, "File: channels=%d, ", get_channels());

    // sample frequency
    set_frequency(igetl(m_file));
    fprintf(stderr, "freq=%d, ", get_frequency());

    // skip six bytes
    igetl(m_file);
    igetw(m_file);

    set_bits(igetw(m_file));
    fprintf(stderr, "bits=%d\n", get_bits());
    //TODO: add support for 12bit etc
    if((get_bits() % 8) != 0 )
    {
        fprintf(stderr, "WaveStream: Not 8, 16, 24, or 32bit data\n");
        return -1;
    }

    //we have just read 16 bytes
    l_len -= 16;
    m_header_length = 36;
  
    //read though the rest of the chunk
    for(int l_j = 0; l_j < l_len; l_j++)
    {
        fgetc(m_file);
        m_header_length++;
    }

    while(true)
    {
        //read data chunk
        if(fread(l_buffer, 1, 4, m_file) != 4)
        {
            goto missing_data;
        }
        // read chunk length
        l_len = igetl(m_file);
        m_header_length += 8;
        if(memcmp(l_buffer, "data", 4) == 0)
        {
            break;
        }
        if(feof(m_file))
        {
            fprintf(stderr, "WaveStream: Data chunk not found\n");
            return -1;
        }
        for(int l_j = 0; l_j < l_len; l_j++)
        {
            if(fgetc(m_file) == EOF)
            {
                fprintf(stderr, "WaveStream: Data chunk missing\n");
                return -1;
            }
        }
    }
#ifdef PRINTF_DEBUG
    printf("header_length=%d\n", m_header_length);
#endif // PRINTF_DEBUG
    set_total_frames(l_len / frame_size());

    return 0;

    missing_data:
    fprintf(stderr, "Error reading header\n");
    return -1;
}

//------------------------------------------------------------------------------
long WaveStream::read_bytes( void *p_data
                           , long p_length
                           )
{
    if(!m_file || !(get_mode() & F_READ))
    {
        return 0;
    }

    long l_read = fread(p_data, 1, p_length, m_file);
    setPos(get_pos() + l_read / frame_size());
    if(pos() > totalFrames())
    {
        set_total_frames(pos());
    }
    return l_read;
}

//------------------------------------------------------------------------------
long WaveStream::read_frames( void *p_data
                            , long p_length
                            )
{
    if(!m_file || !(get_mode() & F_READ))
    {
        return 0;
    }

    long l_read = fread(p_data, frame_size(), p_length, m_file);
    setPos(get_pos() + l_read);
    if(pos() > totalFrames())
    {
        set_total_frames(pos());
    }
    return l_read;
}

//------------------------------------------------------------------------------
int WaveStream::open_write(const char *p_filename, int p_freq, int p_channels, int p_bits)
{
    set_frequency(p_freq);
    set_channels(p_channels);
    set_bits(p_bits);
    setPos(0);
    set_total_frames(0);
    m_file = fopen(p_filename, "wb");
    if(!m_file)
    {
        return -1;
    }
    set_mode(F_WRITE);
    write_header();
    return 0;
}

//------------------------------------------------------------------------------
void WaveStream::write_header(void)
{
    if(!m_file || !(get_mode() & F_WRITE))
    {
        return;
    }

    rewind(m_file);

    fputs("RIFF", m_file);                 /* RIFF header */
    iputl(36+data_length(), m_file);         /* size of RIFF chunk */
  
    fputs("WAVE", m_file);                 /* WAV definition */
    fputs("fmt ", m_file);                 /* format chunk */
    iputl(16, m_file);                     /* size of format chunk */
    iputw(1, m_file);                      /* PCM data 1=two's compliment int*/
    iputw(get_channels(), m_file);               /* number of channels */
    iputl(get_frequency(), m_file);                   /* sample frequency */
    long l_byte_rate = get_frequency() * get_channels() * sample_size();
    iputl(l_byte_rate, m_file);              /* avg. bytes per sec */
    int l_block_align = get_channels() * sample_size();
    iputw(l_block_align, m_file);             /* block alignment */
    iputw(get_bits(), m_file);          /* bits per sample */
    fputs("data", m_file);                 /* data chunk */
    iputl(data_length(), m_file);            /* actual data length */
}

//------------------------------------------------------------------------------
long WaveStream::write_bytes( void * p_data
                            , long p_length
                            )
{
    if(!m_file || !(get_mode() & F_WRITE))
    {
        return 0;
    }

    long l_written = fwrite(p_data, 1, p_length, m_file);
    setPos( get_pos() + l_written/frame_size());
    if(pos() > totalFrames())
    {
        set_total_frames(pos());
    }
    return l_written;
}

//------------------------------------------------------------------------------
long WaveStream::write_frames( void *p_data
                             , long p_length
                             )
{
    if(!m_file || !(get_mode() & F_WRITE))
    {
        return 0;
    }

    long l_written = fwrite(p_data, frame_size(), p_length, m_file);
    setPos(get_pos() + l_written);
    if(pos() > totalFrames())
    {
        set_total_frames(pos());
    }
    return l_written;
}

//------------------------------------------------------------------------------
void WaveStream::jump_to_frame(int p_frame)
{
    p_frame = bound(p_frame, 0, totalFrames());
    if(fseek(m_file, m_header_length + p_frame*frame_size(), SEEK_SET))
    {
        fprintf(stderr, "error seeking, %d\n", errno);
    }
    setPos(p_frame);
}

//------------------------------------------------------------------------------
void WaveStream::jump_back(int p_frames)
{
    jump_to_frame(pos() - p_frames);
}

//------------------------------------------------------------------------------
void WaveStream::jump_forward(int p_frames)
{
    jump_to_frame(pos() + p_frames);
}

// EOF
