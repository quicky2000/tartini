/***************************************************************************
                          soundfile.h  -  description
                             -------------------
    begin                : Sat Jul 10 2004
    copyright            : (C) 2004-2005 by Philip McLeod
    email                : pmcleod@pmcleod.otago.ac.nz
    copyright            : (C) 2019 by Julien Thevenon
    email                : julien_thevenon at yahoo.fr

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/

#ifndef SOUND_FILE
#define SOUND_FILE

#include "array1d.h"
#include "sound_file_stream.h"
#include "gdata.h"
#include "mytransforms.h"
#include "mainwindow.h"

class Channel;

extern const double g_v8;
extern const double g_v16;
extern const double g_v32;

class SoundFile
{
  public:
    SoundFile();
    ~SoundFile();

    void setFilename(const std::string & p_filename);
    inline int numChannels() const;
    bool openRead(const std::string & p_filename);
    bool openWrite( const std::string & p_filename
                  , int p_rate
                  , int p_channels
                  , int p_bits
                  , int p_window_size
                  , int p_step_size)
                  ;

    /**
       Preprocess the whole sound file,
       by looping through and processing every chunk in the file.
       A progress bar is displayed in the toolbar, because this can
       be time consuming.
    */
    void preProcess();

    void rec2play();

    /**
       Lock the mutex's of all the channels
    */
    void lock();

    /**
       Unlock the mutex's of all the channels

    */
    void unlock();

    /**
       Plays one chunk of sound to the associated stream
       @return true on success. false for end of file or error
    */
    bool playChunk();

    void recordChunk(int p_n);

    /**
       Plays one chunk of sound to the associated stream
       @return true on success. false for end of file or error
    */
    static bool playRecordChunk( SoundFile * p_play_sound_file
                               , SoundFile * p_rec_sound_file
                               );

    /**
       Process the chunks for all the channels
       Increment the chunkNum.
    */
    // Check if really used
    void processNewChunk();
    // End of check

    inline const double & startTime() const;
    // Check if really used
    inline void setStartTime(const double & p_start_time);
    // End of check

    inline int currentStreamChunk() const;
    inline int currentRawChunk() const;
    inline int currentChunk() const;

    inline int offset() const;
    inline double timePerChunk() const;
    inline int chunkAtTime(const double & p_time) const;
    inline double chunkFractionAtTime(const double & p_time) const;
    inline double timeAtChunk(int p_chunk) const;
    inline double timeAtCurrentChunk() const;
    inline int chunkAtCurrentTime() const;
    void jumpToChunk(int p_chunk);
    inline void jumpToTime(const double & p_time);
  
    inline int rate() const;
    inline int bits() const;
    inline int framesPerChunk() const;
    int bufferSize() const;
    int totalChunks() const;

    inline bool saved() const;
    inline void setSaved(bool p_new_state);
    inline bool doingDetailedPitch() const;

    inline bool isFirstTimeThrough() const;
    inline void setFirstTimeThrough(bool);
    inline const std::string & getFileName() const;
    inline void calculateAnalysisData( int p_chunk
                                     , Channel *p_channel
                                     );
    inline int getChannelIndex(const Channel &)const;
    inline Channel & getChannel(int p_index);
    inline SoundFileStream & getStream();

  protected:

    friend bool playRecordChunk( SoundFile * p_play_sound_file
                               , SoundFile * p_rec_sound_file
                               , int p_n
                               );

  private:
    /**
       free up all the memory of everything used
    */
    void uninit();
    void setFilteredFilename(const std::string & p_filtered_filename);
    QString getNextTempFilename() const;

    // Check if really used
    void close();
    // End of check

    int readN(int p_n);
    bool setupPlayChunk();
    void finishRecordChunk(int p_n);
    void applyEqualLoudnessFilter(int p_n);

    /**
       Reads framesPerChunk frames from the SoundStream s
       into the end of the channel buffers. I.e losing framesPerChunk
       frames from the beginning of the channel buffers.
       If s is NULL (the defult) the the file stream is used
       @param s The SoundStream to read from or NULL (the default) the current file stream is used
       @return The number of frames actually read ( <= framesPerChunk() )
    */
    int readChunk(int p_n);

    inline void setCurrentChunk(int p_x);
    // Check if really used
    inline void incrementChunkNum();
    // End of check

    /**
       shift all the channels data left by n frames
    */
    // Check if really used
    void shift_left(int p_n);
    // End of check

    inline void setFramesPerChunk(int p_step_size);

    // Check if really used
    bool inFile() const; ///< Returns false if past end of file
    // End of check

    inline bool equalLoudness() const;

    /**
       Waits until there is n frames of data to read from s,
       Then puts the data in buffer.
       @param p_stream The SoundStream to read from
       @param p_buffer The chunk buffer to put the data into. (s->channels of at least n length)
       This is padded with zeros (up to n) if there is less than n frames read.
       @param p_n The number of frames to read
       @return The number of frames read.
    */
    int blockingRead( SoundStream * p_stream
                    , float ** p_buffer
                    , int p_n
                    ); //low level

    /**
       Writes n frames of data to s from buffer.
       @param p_stream The SoundStream to write to
       @param p_buffer The chunk buffer data to use. (s->channels of at least n length)
       @param p_n The number of frames to write
       @return The number of frames written.
    */
    int blockingWrite( SoundStream * p_stream
                     , float ** p_buffer
                     , int p_n
                     ); //low level

    /**
       Writes n frames of data to s and reads n frames of data from s
       If less than n frams are read, the remaining buffer is filled with zeros
       @param p_stream The SoundStream to write to then read from
       @param p_write_buffer The data that is to be written to the stream
       @param p_read_buffer The data that is read back from the stream
       @param p_n The amount of data in each of the buffers. Note: They must be the same size
       @param ch The number of channels
    */
    static int blockingWriteRead( SoundStream * p_stream
                                , float ** p_write_buffer
                                , int p_write_channel
                                , float ** p_read_buffer
                                , int p_read_channel
                                , int p_n
                                );
    void toChannelBuffers(int p_n); //low level

    /**
       @param p_c Channel number
       @param p_n Number of frames to copy from tempWindowBuffer into channel
    */
    void toChannelBuffer( int p_c
                        , int p_n
                        );

    std::string m_filename;
    std::string m_filtered_filename;
    SoundFileStream * m_stream; ///< Pointer to the file's SoundFileStream
    SoundFileStream * m_filtered_stream; ///< Pointer to the file's filtered SoundFileStream
    Array1d<Channel*> m_channels; ///< The actual sound data is stored seperately for each channel
    MyTransforms m_my_transforms;
    bool m_first_time_through;

    int m_chunk_num;
    int m_frames_per_chunk; ///< The number of samples to move every chunk
    float ** m_temp_window_buffer; //array is indexed from -16 !!
    float ** m_temp_window_buffer_filtered; //array is indexed from -16 !!
    double m_start_time;
    int m_offset;
    bool m_saved;
    QMutex * m_mutex;
    bool m_doing_detailed_pitch;
};

#include "soundfile.hpp"

#endif // SOUND_FILE
//EOF

