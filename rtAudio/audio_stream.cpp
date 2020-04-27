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
#include "myassert.h"
#include <QThread>

//------------------------------------------------------------------------------
AudioStream::AudioStream()
{
    m_buffer_size = 1024;
    m_num_buffers = 0;
    m_audio = NULL;
    m_in_buffer.setAutoGrow(true);
    m_out_buffer.setAutoGrow(true);
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
    printf("AudioStream::close()\n");
    
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
    printf("AudioStream::open(p_mode = %d, p_freq = %d, p_channels = %d, p_buffer_size = %d\n",
           p_mode, p_freq, p_channels, p_buffer_size);
    
    set_mode(p_mode);
    set_frequency(p_freq);
    set_channels(p_channels);
    //bits_; //ignored, just use floats and let rtAudio do the conversion
    set_bits(32);
    m_buffer_size = p_buffer_size;
    m_num_buffers = 4; // not used (also ignored in the settings dialog?)

    RtAudio::StreamParameters *inputParamPtr = NULL;
    RtAudio::StreamParameters *outputParamPtr = NULL;
    
    RtAudio::StreamParameters inputParameters;
    if(get_mode() == F_READ || get_mode() == F_RDWR)
    {
        QStringList l_in_names = getInputDeviceNames();
        const std::string l_audio_input = g_data->getSettingsValue("Sound/soundInput", std::string("Default Input"));
        m_in_device = getDeviceNumber(l_audio_input.c_str());
        
        inputParameters.deviceId = m_in_device;
        inputParameters.nChannels = get_channels();
        inputParamPtr = &inputParameters;
        
        fprintf(stderr, "Input Device %d: %s\n", m_in_device, l_audio_input.c_str());
    }

    RtAudio::StreamParameters outputParameters;
    if(get_mode() == F_WRITE || get_mode() == F_RDWR)
    {
        QStringList l_out_names = getOutputDeviceNames();
        const std::string l_audio_output = g_data->getSettingsValue("Sound/soundOutput", std::string("Default Output"));
        m_out_device = getDeviceNumber(l_audio_output.c_str());
        
        outputParameters.deviceId = m_out_device;
        outputParameters.nChannels = get_channels();
        outputParamPtr = &outputParameters;

        fprintf(stderr, "Output Device %d: %s\n", m_out_device, l_audio_output.c_str());
    }

    if(!inputParamPtr && !outputParamPtr)
    {
        fprintf(stderr, "No mode selected\n");
        return -1;
    }

    try
    {
        // Just leave api as UNSPECIFIED.
        m_audio = new RtAudio();

        // TODO: Could pass m_num_buffers via RtAudio::StreamOptions, but the docs say it is usually ignored anyway.
        m_audio->openStream(outputParamPtr, inputParamPtr, RTAUDIO_FLOAT32, get_frequency(), (unsigned int *)&m_buffer_size, callback, this);
    }
    catch (RtAudioError &error)
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
        // The callback function will be called (on another thread) when data is available or required.
        m_audio->startStream();
    }
    catch (RtAudioError &error)
    {
        error.printMessage();
        delete m_audio;
        return -1;
    }
    return 0;
}

//------------------------------------------------------------------------------
// This static function is called when data is availble or required.  The userData is a pointer to the AudioStream object.
int AudioStream::callback( void *outputBuffer
                         , void *inputBuffer
                         , unsigned int nBufferFrames
                         , double streamTime
                         , RtAudioStreamStatus status
                         , void *userData
                         )
{
    AudioStream * self = (AudioStream *)userData;
    
    // Call the callback instance method on "this" object.
    return self->callback(outputBuffer, inputBuffer, nBufferFrames, streamTime, status);
}

//------------------------------------------------------------------------------
// This instance method is called when data is availble or required.
int AudioStream::callback( void *outputBuffer
                         , void *inputBuffer
                         , unsigned int nBufferFrames
                         , double streamTime
                         , RtAudioStreamStatus status
                         )
{
    printf("AudioStream::callback(nBufferFrames=%d, streamTime=%f, m_out_buffer.size()=%d, m_in_buffer.size()=%d)\n",
           nBufferFrames, streamTime, m_out_buffer.size(), m_in_buffer.size());

    // unsigned int i, j;
    float *outBuffer = (float *) outputBuffer;
    float *inBuffer = (float *) inputBuffer;

    if (status == RTAUDIO_INPUT_OVERFLOW)
    {
        printf("Stream input overflow!\n");
    }
    else if (status == RTAUDIO_OUTPUT_UNDERFLOW)
    {
        printf("Stream output underflow!\n");
    }

    unsigned int i;
    if ( inBuffer )
    {
        // Copy nBufferFrames of data from the audio input to the input RingBuffer.
        // FIXME: Need a mutex around access to m_in_buffer.
        for(i = 0; i < nBufferFrames * get_channels(); i++)
        {
            bool success = m_in_buffer.put(inBuffer[i]);
            myassert(success);
        }
    }
    
    if ( outBuffer )
    {
        // Block until there is enough output data to send.
        while (m_out_buffer.size() < (int)nBufferFrames * get_channels())
        {
            printf(".");
            QThread::msleep(1);
            if(!m_audio->isStreamRunning())
            {
                printf("Stream not running\n");
                return 0;
            }
        }

        // Copy nBufferFrames of data from the output RingBuffer to the audio output.
        // FIXME: Need a mutex around access to m_out_buffer.
        // This could be faster with:  m_out_buffer.get(outBuffer, nBufferFrames * p_ch);
        for(i = 0; i < nBufferFrames * get_channels(); i++)
        {
            bool success = m_out_buffer.get(&outBuffer[i]);
            myassert(success);
        }
    }
    
    return 0;
}

//------------------------------------------------------------------------------
int AudioStream::writeFloats( float ** p_channel_data
                            , int p_length
                            , int p_ch
                            )
{
    printf("AudioStream::writeFloats(p_length=%d, p_ch=%d, m_out_buffer.size()=%d)\n", p_length, p_ch, m_out_buffer.size());

    myassert(p_ch == get_channels());
    
    int l_c;
    int l_j;
    // Copy p_length frames of data to the output RingBuffer.
    // FIXME: Need a mutex around access to m_out_buffer.
    for(l_j = 0; l_j < p_length; l_j++)
    {
        for(l_c = 0; l_c < p_ch; l_c++)
        {
            bool success = m_out_buffer.put(p_channel_data[l_c][l_j]);
            myassert(success);
        }
    }

    // Block until all of the output data has been sent.
    while (m_out_buffer.size() > 0)
    {
        printf(">");
        QThread::msleep(1);
    }
    
    return p_length;
}

//------------------------------------------------------------------------------
int AudioStream::readFloats( float ** p_channel_data
                           , int p_length
                           , int p_ch
                           )
{
    myassert(p_ch == get_channels());
    
    int l_c;
    int l_j;
    
    printf("AudioStream::readFloats(p_length=%d, p_ch=%d, m_in_buffer.size()=%d)\n", p_length, p_ch, m_in_buffer.size());

    // Block until there is enough input data to read.
    while (m_in_buffer.size() < p_length * p_ch)
    {
        printf("<");
        QThread::msleep(1);
    }

    // Copy p_length frames of data from the input RingBuffer.
    // FIXME: Need a mutex around access to m_in_buffer.
    for(l_j = 0; l_j < p_length; l_j++)
    {
        for(l_c = 0; l_c < p_ch; l_c++)
        {
            bool success = m_in_buffer.get(&(p_channel_data[l_c][l_j]));
            myassert(success);
        }
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
    printf("AudioStream::writeReadFloats(p_out_channel=%d, p_in_channel=%d, p_length=%d, m_out_buffer.size()=%d, m_in_buffer.size()=%d)\n",
           p_out_channel, p_in_channel, p_length, m_out_buffer.size(), m_in_buffer.size());

    myassert(p_out_channel == get_channels());
    myassert(p_in_channel == get_channels());

    int l_c;
    int l_j;
    // Copy p_length frames of data to the output RingBuffer.
    // FIXME: Need a mutex around access to m_out_buffer.
    for(l_j = 0; l_j < p_length; l_j++)
    {
        for(l_c = 0; l_c < p_out_channel; l_c++)
        {
            bool success = m_out_buffer.put(p_out_channel_data[l_c][l_j]);
            myassert(success);
        }
    }

    // Block until there is enough input data to read.
    while (m_in_buffer.size() < p_length * p_in_channel)
    {
        printf("<");
        QThread::msleep(1);
    }

    // Copy p_length frames of data from the input RingBuffer.
    // FIXME: Need a mutex around access to m_in_buffer.
    for(l_j = 0; l_j < p_length; l_j++)
    {
        for(l_c = 0; l_c < p_in_channel; l_c++)
        {
            bool success = m_in_buffer.get(&(p_in_channel_data[l_c][l_j]));
            myassert(success);
        }
    }

    // Block until all of the output data has been sent.
    while (m_out_buffer.size() > 0)
    {
        printf(">");
        QThread::msleep(1);
    }
    
    return p_length;
}

//------------------------------------------------------------------------------
QStringList AudioStream::getInputDeviceNames()
{
    printf("AudioStream::getInputDeviceNames()\n");

    QStringList l_to_return;
    l_to_return << "Default Input";

    RtAudio * l_temp_audio = NULL;
    try
    {
        l_temp_audio = new RtAudio();
    }
    catch (RtAudioError &l_error)
    {
        l_error.printMessage();
        return l_to_return;
    }

    // Determine the number of devices available
    int l_num_devices = l_temp_audio->getDeviceCount();
    printf("num devices = %d\n", l_num_devices);

    // Scan through devices for various capabilities
    RtAudio::DeviceInfo l_info;
    for(int l_i = 0; l_i < l_num_devices; l_i++)
    {
        try
        {
            printf("Probing device %d\n", l_i);
            l_info = l_temp_audio->getDeviceInfo(l_i);
        }
        catch (RtAudioError &l_error)
        {
            l_error.printMessage();
            break;
        }
        if (l_info.probed == false)
        {
            printf("Probe failed on device %d\n", l_i);
            continue;
        }
        printf("Device %d: %s\n", l_i, l_info.name.c_str());
        printf("  outs %d, ins: %d, duplex: %d, default out: %d, default in: %d, preferred rate: %d\n",
               l_info.outputChannels, l_info.inputChannels, l_info.duplexChannels, l_info.isDefaultOutput, l_info.isDefaultInput, l_info.preferredSampleRate);
        if(l_info.inputChannels > 0)
        {
            printf("  found input device\n");
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
    printf("AudioStream::getOutputDeviceNames()\n");

    QStringList l_to_return;
    l_to_return << "Default Output";

    RtAudio * l_temp_audio = NULL;
    try
    {
        l_temp_audio = new RtAudio();
    }
    catch (RtAudioError &l_error)
    {
        l_error.printMessage();
        return l_to_return;
    }

    // Determine the number of devices available
    int l_num_devices = l_temp_audio->getDeviceCount();
    printf("num devices = %d\n", l_num_devices);

    // Scan through devices for various capabilities
    RtAudio::DeviceInfo l_info;
    for(int l_i = 0; l_i < l_num_devices; l_i++)
    {
        try
        {
            printf("Probing device %d\n", l_i);
            l_info = l_temp_audio->getDeviceInfo(l_i);
        }
        catch (RtAudioError &l_error)
        {
            l_error.printMessage();
            break;
        }

        if (l_info.probed == false)
        {
            printf("Probe failed on device %d\n", l_i);
            continue;
        }
        printf("Device %d: %s\n", l_i, l_info.name.c_str());
        printf("  outs %d, ins: %d, duplex: %d, default out: %d, default in: %d, preferred rate: %d\n",
               l_info.outputChannels, l_info.inputChannels, l_info.duplexChannels, l_info.isDefaultOutput, l_info.isDefaultInput, l_info.preferredSampleRate);
        if(l_info.outputChannels > 0)
        {
            printf("  found output device\n");
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
    printf("AudioStream::getDeviceNumber(p_device_name=\"%s\")\n", p_device_name);

    int l_device_number = -1;

    RtAudio * l_temp_audio = NULL;
    try
    {
        l_temp_audio = new RtAudio();
    }
    catch (RtAudioError &l_error)
    {
        l_error.printMessage();
        return -1;
    }

    if(strcmp("Default Input", p_device_name) == 0)
    {
        l_device_number = l_temp_audio->getDefaultInputDevice();
    }
    else if(strcmp("Default Output", p_device_name) == 0)
    {
        l_device_number = l_temp_audio->getDefaultOutputDevice();
    }
    else
    {
        // Determine the number of devices available
        int l_num_devices = l_temp_audio->getDeviceCount();
        printf("num devices = %d\n", l_num_devices);

        // Scan through devices for various capabilities
        RtAudio::DeviceInfo l_info;
        for(int l_i = 0; l_i < l_num_devices; l_i++)
        {
            try
            {
                printf("Probing device %d\n", l_i);
                l_info = l_temp_audio->getDeviceInfo(l_i);
            }
            catch (RtAudioError &l_error)
            {
                l_error.printMessage();
                break;
            }
            if (l_info.probed == false)
            {
                printf("Probe failed on device %d\n", l_i);
                continue;
            }
            printf("Device %d: %s\n", l_i, l_info.name.c_str());
            printf("  outs %d, ins: %d, duplex: %d, default out: %d, default in: %d, preferred rate: %d\n",
                   l_info.outputChannels, l_info.inputChannels, l_info.duplexChannels, l_info.isDefaultOutput, l_info.isDefaultInput, l_info.preferredSampleRate);
            if(strcmp(l_info.name.c_str(), p_device_name) == 0)
            {
                printf("  found matching device\n");
                l_device_number = l_i;
                break;
            }
        }
    }
    // Clean up
    delete l_temp_audio;
    return l_device_number;
}

// EOF
