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
#ifdef DEBUG_PRINTF
    std::cout << "AudioStream::close()" << std::endl;
#endif // DEBUG_PRINTF
    
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
#ifdef DEBUG_PRINTF
    std::cout << "AudioStream::open(p_mode = " << p_mode << ", p_freq = " << p_freq << ", p_channels = " << p_channels << ", p_buffer_size = " << p_buffer_size << std::endl;
#endif // DEBUG_PRINTF
    
    set_mode(p_mode);
    set_frequency(p_freq);
    set_channels(p_channels);
    //bits_; //ignored, just use floats and let rtAudio do the conversion
    set_bits(32);
    m_buffer_size = p_buffer_size;
    m_num_buffers = 4; // not used (also ignored in the settings dialog?)

    RtAudio::StreamParameters * l_input_param_ptr = NULL;
    RtAudio::StreamParameters * l_output_param_ptr = NULL;
    
    RtAudio::StreamParameters l_input_parameters;
    if(get_mode() == F_READ || get_mode() == F_RDWR)
    {
        QStringList l_in_names = getInputDeviceNames();
        const std::string l_audio_input = g_data->getSettingsValue("Sound/soundInput", std::string("Default"));
        int l_in_device = getDeviceNumber(l_audio_input.c_str(), true);

        l_input_parameters.deviceId = l_in_device;
        l_input_parameters.nChannels = get_channels();
        l_input_param_ptr = &l_input_parameters;
        
        std::cerr << "Input Device " << l_in_device << ": " << l_audio_input.c_str() << std::endl;
    }

    RtAudio::StreamParameters l_output_parameters;
    if(get_mode() == F_WRITE || get_mode() == F_RDWR)
    {
        QStringList l_out_names = getOutputDeviceNames();
        const std::string l_audio_output = g_data->getSettingsValue("Sound/soundOutput", std::string("Default"));
        int l_out_device = getDeviceNumber(l_audio_output.c_str(), false);

        l_output_parameters.deviceId = l_out_device;
        l_output_parameters.nChannels = get_channels();
        l_output_param_ptr = &l_output_parameters;

        std::cerr << "Output Device " << l_out_device << ": " << l_audio_output.c_str() << std::endl;
    }

    if(!l_input_param_ptr && !l_output_param_ptr)
    {
        std::cerr << "No mode selected" << std::endl;
        return -1;
    }

    try
    {
        // Just leave api as UNSPECIFIED.
        m_audio = new RtAudio();

        // TODO: Could pass m_num_buffers via RtAudio::StreamOptions, but the docs say it is usually ignored anyway.
        m_audio->openStream(l_output_param_ptr, l_input_param_ptr, RTAUDIO_FLOAT32, get_frequency(), (unsigned int *)&m_buffer_size, callback, this);
    }
    catch (RtAudioError & l_error)
    {
        l_error.printMessage();
        m_audio = NULL;
        return -1;
    }

    if(m_buffer_size != p_buffer_size)
    {
        std::cerr << "Warning: Got buffer_size of " << m_buffer_size << " instead of " << p_buffer_size << std::endl;
    }

    try
    {
        // The callback function will be called (on another thread) when data is available or required.
        m_audio->startStream();
    }
    catch (RtAudioError & l_error)
    {
        l_error.printMessage();
        delete m_audio;
        return -1;
    }
    return 0;
}

//------------------------------------------------------------------------------
int AudioStream::callback( void * p_output_buffer
                         , void * p_input_buffer
                         , unsigned int p_n_buffer_frames
                         , double p_stream_time
                         , RtAudioStreamStatus p_status
                         , void * p_user_data
                         )
{
    AudioStream * l_self = (AudioStream *) p_user_data;
    
    // Call the callback instance method on "this" object.
    return l_self->callback(p_output_buffer, p_input_buffer, p_n_buffer_frames, p_stream_time, p_status);
}

//------------------------------------------------------------------------------
int AudioStream::callback( void * p_output_buffer
                         , void * p_input_buffer
                         , unsigned int p_n_buffer_frames
                         , double p_stream_time
                         , RtAudioStreamStatus p_status
                         )
{
#ifdef DEBUG_PRINTF
    std::cout << "AudioStream::callback(nBufferFrames=" << p_n_buffer_frames << ", streamTime=" << p_stream_time << ", m_out_buffer.size()=" << m_out_buffer.size() <<", m_in_buffer.size()=" << m_in_buffer.size() << std::endl;
#endif // DEBUG_PRINTF

    // unsigned int i, j;
    float * l_out_buffer = (float *) p_output_buffer;
    float * l_in_buffer = (float *) p_input_buffer;

    if (p_status == RTAUDIO_INPUT_OVERFLOW)
    {
        std::cout << "Stream input overflow!" << std::endl;
    }
    else if (p_status == RTAUDIO_OUTPUT_UNDERFLOW)
    {
        std::cout << "Stream output underflow!" << std::endl;
    }

    int l_num_floats = p_n_buffer_frames * get_channels();
    if ( l_in_buffer )
    {
        // Copy nBufferFrames of data from the input device to the input RingBuffer.
        QMutexLocker l_mutex_locker(&m_in_mutex);
        int l_result = m_in_buffer.put(l_in_buffer, l_num_floats);
        myassert(l_result == l_num_floats);
    }
    
    if ( l_out_buffer )
    {
        // Block until there is enough output data to send.
        while (m_out_buffer.size() < l_num_floats)
        {
#ifdef DEBUG_PRINTF
            std::cout << ".";
#endif // DEBUG_PRINTF
            QThread::msleep(1);
            if(!m_audio->isStreamRunning())
            {
                std::cout << "Stream not running" << std::endl;
                return 0;
            }
        }

        // Copy nBufferFrames of data from the output RingBuffer to the output device.
        {
            QMutexLocker l_mutex_locker(&m_out_mutex);
            int l_result = m_out_buffer.get(l_out_buffer, l_num_floats);
            myassert(l_result == l_num_floats);
        }
}
    
    return 0;
}

//------------------------------------------------------------------------------
// Writes data to the output RingBuffer, blocking until the data has been sent to the output device.
int AudioStream::writeFloats( float ** p_channel_data
                            , int p_length
                            , int p_ch
                            )
{
#ifdef DEBUG_PRINTF
    std::cout << "AudioStream::writeFloats(p_length=" << p_length << ", p_ch=" << p_ch << ", m_out_buffer.size()=" << m_out_buffer.size() << std::endl;
#endif // DEBUG_PRINTF

    myassert(p_ch == get_channels());
    
    int l_c;
    int l_j;
    // Copy p_length frames of data to the output RingBuffer.
    {
        QMutexLocker l_mutex_locker(&m_out_mutex);
        for(l_j = 0; l_j < p_length; l_j++)
        {
            for(l_c = 0; l_c < p_ch; l_c++)
            {
                bool l_success = m_out_buffer.put(p_channel_data[l_c][l_j]);
                myassert(l_success);
            }
        }
    }

    // Block until all of the output data has been sent.
    while (m_out_buffer.size() > 0)
    {
#ifdef DEBUG_PRINTF
        std::cout << ">";
#endif // DEBUG_PRINTF
        QThread::msleep(1);
    }
    
    return p_length;
}

//------------------------------------------------------------------------------
// Reads data from the input RingBuffer, blocking until the data is available from the input device.
int AudioStream::readFloats( float ** p_channel_data
                           , int p_length
                           , int p_ch
                           )
{
    myassert(p_ch == get_channels());
    
    int l_c;
    int l_j;
    
#ifdef DEBUG_PRINTF
    std::cout << "AudioStream::readFloats(p_length=" << p_length << ", p_ch=" << p_ch << ", m_in_buffer.size()=" << m_in_buffer.size() << std::endl;
#endif // DEBUG_PRINTF

    // Block until there is enough input data to read.
    while (m_in_buffer.size() < p_length * p_ch)
    {
#ifdef DEBUG_PRINTF
        std::cout << "<";
#endif // DEBUG_PRINTF
        QThread::msleep(1);
    }

    // Copy p_length frames of data from the input RingBuffer.
    {
        QMutexLocker l_mutex_locker(&m_in_mutex);
        for(l_j = 0; l_j < p_length; l_j++)
        {
            for(l_c = 0; l_c < p_ch; l_c++)
            {
                bool l_success = m_in_buffer.get(&(p_channel_data[l_c][l_j]));
                myassert(l_success);
            }
        }
    }
    
    return p_length;
}

//------------------------------------------------------------------------------
// Writes data to the output RingBuffer and reads data from the input RingBuffer.
// Blocks until the input data is available from the input device and the output data has been sent to the output device.
int AudioStream::writeReadFloats( float ** p_out_channel_data
                                , int p_out_channel
                                , float ** p_in_channel_data
                                , int p_in_channel
                                , int p_length
                                )
{
#ifdef DEBUG_PRINTF
    std::cout << "AudioStream::writeReadFloats(p_out_channel=" << p_out_channel << ", p_in_channel=" << p_in_channel << ", p_length=" << p_length << ", m_out_buffer.size()=" << m_out_buffer.size() << ", m_in_buffer.size()=" << m_in_buffer.size() << std::endl;
#endif // DEBUG_PRINTF

    myassert(p_out_channel == get_channels());
    myassert(p_in_channel == get_channels());

    int l_c;
    int l_j;
    // Copy p_length frames of data to the output RingBuffer.
    {
        QMutexLocker l_mutex_locker(&m_out_mutex);
        for(l_j = 0; l_j < p_length; l_j++)
        {
            for(l_c = 0; l_c < p_out_channel; l_c++)
            {
                bool l_success = m_out_buffer.put(p_out_channel_data[l_c][l_j]);
                myassert(l_success);
            }
        }
    }
    
    // Block until there is enough input data to read.
    while (m_in_buffer.size() < p_length * p_in_channel)
    {
#ifdef DEBUG_PRINTF
        std::cout << "<";
#endif // DEBUG_PRINTF
        QThread::msleep(1);
    }

    // Copy p_length frames of data from the input RingBuffer.
    {
        QMutexLocker l_mutex_locker(&m_in_mutex);
        for(l_j = 0; l_j < p_length; l_j++)
        {
            for(l_c = 0; l_c < p_in_channel; l_c++)
            {
                bool l_success = m_in_buffer.get(&(p_in_channel_data[l_c][l_j]));
                myassert(l_success);
            }
        }
    }

    // Block until all of the output data has been sent.
    while (m_out_buffer.size() > 0)
    {
#ifdef DEBUG_PRINTF
        std::cout << ">";
#endif // DEBUG_PRINTF
        QThread::msleep(1);
    }
    
    return p_length;
}

//------------------------------------------------------------------------------
QStringList AudioStream::getInputDeviceNames()
{
#ifdef DEBUG_PRINTF
    std::cout << "AudioStream::getInputDeviceNames()" << std::endl;
#endif // DEBUG_PRINTF

    QStringList l_to_return;
    l_to_return << "Default";

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
#ifdef DEBUG_PRINTF
    std::cout << "num devices = " << l_num_devices << std::endl;
#endif // DEBUG_PRINTF

    // Scan through devices for various capabilities
    RtAudio::DeviceInfo l_info;
    for(int l_i = 0; l_i < l_num_devices; l_i++)
    {
        try
        {
#ifdef DEBUG_PRINTF
            std::cout << "Probing device " << l_i << std::endl;
#endif // DEBUG_PRINTF
            l_info = l_temp_audio->getDeviceInfo(l_i);
        }
        catch (RtAudioError &l_error)
        {
            l_error.printMessage();
            break;
        }
        if (l_info.probed == false)
        {
            std::cout << "Probe failed on device " << l_i << std::endl;
            continue;
        }
#ifdef DEBUG_PRINTF
        std::cout << "Device " << l_i << ": " << l_info.name << std::endl;
        std::cout << "  outs " << l_info.outputChannels << ", ins: " << l_info.inputChannels << ", duplex: " << l_info.duplexChannels << ", default out: " << l_info.isDefaultOutput << ", default in: " << l_info.isDefaultInput << ", preferred rate: " << l_info.preferredSampleRate << std::endl;
#endif // DEBUG_PRINTF
        if(l_info.inputChannels > 0)
        {
#ifdef DEBUG_PRINTF
            std::cout << "  found input device" << std::endl;
#endif // DEBUG_PRINTF
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
#ifdef DEBUG_PRINTF
    std::cout << "AudioStream::getOutputDeviceNames()" << std::endl;
#endif // DEBUG_PRINTF

    QStringList l_to_return;
    l_to_return << "Default";

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
#ifdef DEBUG_PRINTF
    std::cout << "num devices = " << l_num_devices << std::endl;
#endif // DEBUG_PRINTF

    // Scan through devices for various capabilities
    RtAudio::DeviceInfo l_info;
    for(int l_i = 0; l_i < l_num_devices; l_i++)
    {
        try
        {
#ifdef DEBUG_PRINTF
            std::cout << "Probing device " << l_i << std::endl;
#endif // DEBUG_PRINTF
            l_info = l_temp_audio->getDeviceInfo(l_i);
        }
        catch (RtAudioError &l_error)
        {
            l_error.printMessage();
            break;
        }

        if (l_info.probed == false)
        {
            std::cout << "Probe failed on device " << l_i << std::endl;
            continue;
        }
#ifdef DEBUG_PRINTF
        std::cout << "Device " << l_i << ": " << l_info.name << std::endl;
        std::cout << "  outs " << l_info.outputChannels << ", ins: " << l_info.inputChannels << ", duplex: " << l_info.duplexChannels << ", default out: " << l_info.isDefaultOutput << ", default in: " << l_info.isDefaultInput << ", preferred rate: " << l_info.preferredSampleRate << std::endl;
#endif // DEBUG_PRINTF
        if(l_info.outputChannels > 0)
        {
#ifdef DEBUG_PRINTF
            std::cout << "  found output device" << std::endl;
#endif // DEBUG_PRINTF
            l_to_return << l_info.name.c_str();
        }
    }
    // Clean up
    delete l_temp_audio;
    return l_to_return;
}

//------------------------------------------------------------------------------
int AudioStream::getDeviceNumber(const char * p_device_name, bool p_input)
{
#ifdef DEBUG_PRINTF
    std::cout << "AudioStream::getDeviceNumber(p_device_name=\"" << p_device_name << "\", p_input=" << p_input << ")" << std::endl;
#endif // DEBUG_PRINTF

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

    // Determine the number of devices available
    int l_num_devices = l_temp_audio->getDeviceCount();
    #ifdef DEBUG_PRINTF
    printf("num devices = %d\n", l_num_devices);
    #endif // DEBUG_PRINTF

    // Scan through devices for various capabilities
    RtAudio::DeviceInfo l_info;
    for(int l_i = 0; l_i < l_num_devices; l_i++)
    {
        try
        {
#ifdef DEBUG_PRINTF
            std::cout << "Probing device " << l_i << std::endl;
#endif // DEBUG_PRINTF
            l_info = l_temp_audio->getDeviceInfo(l_i);
        }
        catch (RtAudioError &l_error)
        {
            l_error.printMessage();
            break;
        }
        if (l_info.probed == false)
        {
            std::cout << "Probe failed on device " << l_i << std::endl;
            continue;
        }
#ifdef DEBUG_PRINTF
        std::cout << "Device " << l_i << ": " << l_info.name << std::endl;
        std::cout << "  outs " << l_info.outputChannels << ", ins: " << l_info.inputChannels << ", duplex: " << l_info.duplexChannels << ", default out: " << l_info.isDefaultOutput << ", default in: " << l_info.isDefaultInput << ", preferred rate: " << l_info.preferredSampleRate << std::endl;
#endif // DEBUG_PRINTF
        if(strcmp(l_info.name.c_str(), p_device_name) == 0)
        {
#ifdef DEBUG_PRINTF
            std::cout << "  found matching device" << std::endl;
#endif // DEBUG_PRINTF
            l_device_number = l_i;
            break;
        }
    }
    if (l_device_number == -1)
    {
        if (p_input)
        {
            l_device_number = l_temp_audio->getDefaultInputDevice();
            if(strcmp("Default", p_device_name) != 0)
            {
                printf("Device name \"%s\" not found.  Using default input device: %d\n", p_device_name, l_device_number);
            }
        }
        else
        {
            l_device_number = l_temp_audio->getDefaultOutputDevice();
            if(strcmp("Default", p_device_name) != 0)
            {
                printf("Device name \"%s\" not found.  Using default output device: %d\n", p_device_name, l_device_number);
            }
        }
    }
    // Clean up
    delete l_temp_audio;
    return l_device_number;
}

// EOF
