/***************************************************************************
                          audio_stream.cpp  -  joining to rtAudio audio routines
                             -------------------
    begin                : 2006
    copyright            : (C) 2006-2006 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
    copyright            : (C) 2016 by Julien Thevenon
    email                : julien_thevenon at yahoo.fr
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/
#include "audio_stream.h"
#include "gdata.h"

//------------------------------------------------------------------------------
AudioStream::AudioStream()
{
    m_buffer_size = 1024;
    m_num_buffers = 0;
    m_audio = NULL;
    m_buffer = NULL;
    m_flow_buffer.setAutoGrow(true);
    m_in_device = m_out_device = 0;
}

//------------------------------------------------------------------------------
AudioStream::~AudioStream()
{
    close();
    if(m_audio) delete m_audio;
}

//------------------------------------------------------------------------------
void AudioStream::close()
{
    if(m_audio)
    {
        m_audio->stopStream();
        m_audio->closeStream();
    }
}

//------------------------------------------------------------------------------
int AudioStream::open( int p_mode
                     , int p_freq
                     , int p_channels
                     , int /*bits_*/
                     , int p_buffer_size
                     )
{
    set_mode(p_mode);
    set_frequency(p_freq);
    set_channels(p_channels);
    //bits_; //ignored, just use floats and let rtAudio do the conversion
    set_bits(32);
    m_buffer_size = p_buffer_size;
    m_num_buffers = 4;

    QStringList l_in_names = getInputDeviceNames();
    const char * l_audio_input = g_data->getSettingsValue("Sound/soundInput", QString("Default")).toStdString().c_str();
    m_in_device = getDeviceNumber(l_audio_input);

    QStringList l_out_names = getOutputDeviceNames();
    const char * l_audio_output = g_data->getSettingsValue("Sound/soundOutput", QString("Default")).toStdString().c_str();
    m_out_device = getDeviceNumber(l_audio_output);

    fprintf(stderr, "Input Device %d: %s\n", m_in_device, l_audio_input);
    fprintf(stderr, "Output Device %d: %s\n", m_out_device, l_audio_output);

    try
    {
        if(get_mode() == F_READ)
        {
            m_audio = new RtAudio(0, 0, m_in_device, get_channels(), RTAUDIO_FLOAT32, get_frequency(), &m_buffer_size, m_num_buffers);
        }
        else if(get_mode() == F_WRITE)
        {
            m_audio = new RtAudio(m_out_device, get_channels(), 0, 0, RTAUDIO_FLOAT32, get_frequency(), &m_buffer_size, m_num_buffers);
        }
        else if(get_mode() == F_RDWR)
        {
            m_audio = new RtAudio(m_out_device, get_channels(), m_in_device, get_channels(), RTAUDIO_FLOAT32, get_frequency(), &m_buffer_size, m_num_buffers);
        }
        else
        {
            fprintf(stderr, "No mode selected\n");
            return -1;
        }
    }
    catch (RtError &error)
    {
        error.printMessage();
        m_audio = NULL;
        return -1;
    }

    if(m_buffer_size != p_buffer_size)
    {
        fprintf(stderr, "Warning: Got buffer_size of %d instead of %d\n", m_buffer_size, p_buffer_size);
    }

    try
    {
        // Get a pointer to the stream buffer
        m_buffer = (float *) m_audio->getStreamBuffer();
        m_audio->startStream();
    }
    catch (RtError &error)
    {
        error.printMessage();
        delete m_audio;
        return -1;
    }
    return 0;
}

//------------------------------------------------------------------------------
int AudioStream::writeFloats( float ** p_channel_data
                            , int p_length
                            , int p_ch
                            )
{
    float * l_buffer_ptr = m_buffer;
    int l_c;
    int l_j;
    if(p_length == m_buffer_size)
    {
        for(l_j = 0; l_j < p_length; l_j++)
        {
            for(l_c = 0; l_c < p_ch; l_c++)
            {
                *l_buffer_ptr++ = p_channel_data[l_c][l_j];
            }
        }

        // Trigger the output of the data buffer
        try
        {
            m_audio->tickStream();
        }
        catch (RtError &error)
        {
            error.printMessage();
            return 0;
        }
    }
    else
    {
        for(l_j = 0; l_j < p_length; l_j++)
        {
            for(l_c = 0; l_c < p_ch; l_c++)
            {
                m_flow_buffer.put(p_channel_data[l_c][l_j]);
            }
        }
        while(m_flow_buffer.size() >= m_buffer_size * p_ch)
        {
            int l_received = m_flow_buffer.get(m_buffer, m_buffer_size * p_ch);
            if(l_received != m_buffer_size * p_ch)
            {
                fprintf(stderr, "AudioStream::writeFloats: Error l_received != buffer_size*p_ch\n");
            }
            // Trigger the output of the data buffer
            try
            {
                m_audio->tickStream();
            }
            catch (RtError &l_error)
            {
                l_error.printMessage();
                return 0;
            }
        }
    }
    return p_length;
}

//------------------------------------------------------------------------------
int AudioStream::readFloats( float ** p_channel_data
                           , int p_length
                           , int p_ch
                           )
{
    float * l_buffer_ptr = m_buffer;
    int l_c;
    int l_j;
    if(p_length == m_buffer_size)
    {
        // Trigger the input of the data buffer
        try
        {
            m_audio->tickStream();
        }
        catch (RtError &l_error)
        {
            l_error.printMessage();
            return 0;
        }
        for(l_j = 0; l_j < p_length; l_j++)
        {
            for(l_c = 0; l_c < p_ch; l_c++)
            {
                p_channel_data[l_c][l_j] = *l_buffer_ptr++;
            }
        }
    }
    else
    {
        fprintf(stderr, "Error reading floats\n");
    }
    return p_length;
}

//------------------------------------------------------------------------------
int AudioStream::writeReadFloats( float ** p_out_channel_data
                                , int p_out_channel
                                , float ** p_in_channel_data
                                , int p_in_channel
                                , int p_length
                                )
{
    float * l_buffer_ptr = m_buffer;
    int l_c;
    int l_j;

    //put the p_out_channel_data into the Audio buffer to be written out
    if(p_length == m_buffer_size)
    {
        for(l_j = 0; l_j < p_length; l_j++)
        {
            for(l_c = 0; l_c < p_out_channel; l_c++)
            {
                *l_buffer_ptr++ = p_out_channel_data[l_c][l_j];
            }
        }
        // Trigger the input/output of the data buffer
        try
        {
            m_audio->tickStream();
        }
        catch (RtError &l_error)
        {
            l_error.printMessage();
            return 0;
        }

        //get the new data from the Audio buffer and put it in p_in_channel_data
        l_buffer_ptr = m_buffer;
        for(l_j = 0; l_j < p_length; l_j++)
        {
            for(l_c = 0; l_c < p_in_channel; l_c++)
            {
                p_in_channel_data[l_c][l_j] = *l_buffer_ptr++;
            }
        }
    }
    else
    {
        fprintf(stderr, "Error: audio buffer length is a different size than the data chunk buffer\n");
        fprintf(stderr, "Not supported on Read/Write yet. Try changing the buffer size in preferences\n");
    }
    return p_length;
}

//------------------------------------------------------------------------------
QStringList AudioStream::getInputDeviceNames()
{
    QStringList l_to_return;
    l_to_return << "Default";

    RtAudio * l_temp_audio = NULL;
    try
    {
        l_temp_audio = new RtAudio();
    }
    catch (RtError &l_error)
    {
        l_error.printMessage();
        return l_to_return;
    }

    // Determine the number of devices available
    int l_num_devices = l_temp_audio->getDeviceCount();

    // Scan through devices for various capabilities
    RtAudioDeviceInfo l_info;
    for(int l_i = 1; l_i <= l_num_devices; l_i++)
    {
        try
        {
            l_info = l_temp_audio->getDeviceInfo(l_i);
        }
        catch (RtError &l_error)
        {
            l_error.printMessage();
            break;
        }
        if(l_info.inputChannels > 0)
        {
            l_to_return << l_info.name.c_str();

        }
    }

    // Clean up
    delete l_temp_audio;
    return l_to_return;
}

//------------------------------------------------------------------------------
QStringList AudioStream::getOutputDeviceNames()
{
    QStringList l_to_return;
    l_to_return << "Default";

    RtAudio * l_temp_audio = NULL;
    try
    {
        l_temp_audio = new RtAudio();
    }
    catch (RtError &l_error)
    {
        l_error.printMessage();
        return l_to_return;
    }

    // Determine the number of devices available
    int l_num_devices = l_temp_audio->getDeviceCount();

    // Scan through devices for various capabilities
    RtAudioDeviceInfo l_info;
    for(int l_i = 1; l_i <= l_num_devices; l_i++)
    {
        try
        {
            l_info = l_temp_audio->getDeviceInfo(l_i);
        }
        catch (RtError &l_error)
        {
            l_error.printMessage();
            break;
        }

        if(l_info.outputChannels > 0)
        {
            l_to_return << l_info.name.c_str();
        }
    }
    // Clean up
    delete l_temp_audio;
    return l_to_return;
}

//------------------------------------------------------------------------------
int AudioStream::getDeviceNumber(const char * p_device_name)
{
    int l_device_number = -1;
    if(strcmp("Default", p_device_name) == 0)
    {
        return 0;
    }

    RtAudio * l_temp_audio = NULL;
    try
    {
        l_temp_audio = new RtAudio();
    }
    catch (RtError &l_error)
    {
        l_error.printMessage();
        return -1;
    }

    // Determine the number of devices available
    int l_num_devices = l_temp_audio->getDeviceCount();

    // Scan through devices for various capabilities
    RtAudioDeviceInfo l_info;
    for(int l_i = 1; l_i <= l_num_devices; l_i++)
    {
        try
        {
            l_info = l_temp_audio->getDeviceInfo(l_i);
        }
        catch (RtError &l_error)
        {
            l_error.printMessage();
            break;
        }
        if(strcmp(l_info.name.c_str(), p_device_name) == 0)
        {
            l_device_number = l_i;
            break;
        }
    }
    // Clean up
    delete l_temp_audio;
    return l_device_number;
}

// EOF
