/***************************************************************************
                          freqpair.h  -  description
                             -------------------
    begin                : 2004
    copyright            : (C) 2004-2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
    copyright            : (C) 2019 by Julien Thevenon
    email                : julien_thevenon at yahoo.fr

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/
#ifndef FREQPAIR_H
#define FREQPAIR_H

#include <vector>

#include <qmutex.h>

#include <stdio.h>

class FreqPair {
public:
  float m_freq, m_amp, m_phase;
  FreqPair *m_prev, *m_next;
  FreqPair() { m_prev=NULL; m_next=NULL; }
  FreqPair(float p_freq, float p_amp) { m_freq=p_freq; m_amp=p_amp; m_phase=0.0; m_prev=NULL; m_next=NULL; }
  FreqPair(float p_freq, float p_amp, float p_phase) { m_freq=p_freq; m_amp=p_amp; m_phase=p_phase; m_prev=NULL; m_next=NULL; }
  void unlink() {
      //if(m_prev) { m_prev->next = NULL; printf("removed one\n"); }
      if(m_next) { m_next = NULL; /*printf("removed one\n");*/ }
  }
  //void attachPrev(FreqPair *prev_) { m_prev = prev_; m_prev->next = this; }
  //void attachNext(FreqPair *next_) { next = next_; next->m_prev = this; }
  void attachNext(FreqPair *p_next) { m_next = p_next; }
};

class FreqHistory {
 public:
    std::vector<FreqPair> * m_data;
    int m_num;
    int m_end;
    QMutex *m_mutex;

    FreqHistory() {
	m_num = 128;
	m_end = 0;
	m_mutex = new QMutex();
	m_data = new std::vector<FreqPair>[m_num];
    }
    FreqHistory(const FreqHistory &p_f) {
	m_num = p_f.m_num;
	m_end = p_f.m_end;
	m_mutex = new QMutex();
	m_data = new std::vector<FreqPair>[m_num];
    }
    virtual ~FreqHistory() {
	delete[] m_data;
	delete m_mutex;
    }

    std::vector<FreqPair> *curData() { return &m_data[m_end]; }
    std::vector<FreqPair> *prevData() { return &m_data[((m_end>0)?m_end-1:m_num-1)]; }
    void lock() { m_mutex->lock(); }
    void unlock() { m_mutex->unlock(); }
    void move() {
	//for(unsigned int k=0; k<data[end].size(); k++) data[end][k].unlink();
	m_end++;
	if(m_end >= m_num) m_end -= m_num;
    }
    void connectToPrev() {
	std::vector<FreqPair> &l_prev = *prevData();
	std::vector<FreqPair> &l_cur = *curData();
	unsigned int l_prev_pos=0, l_cur_pos=0;
	float l_freq_threshold = 10.0;
	float l_amp_threshold = 10.0;
	while((l_prev_pos < l_prev.size()) && (l_cur_pos < l_cur.size())) {
	    //make connection between two consecutive (freq,m_amp) points if within thresholds
	    if(l_prev[l_prev_pos].m_freq > l_cur[l_cur_pos].m_freq - l_freq_threshold &&
           l_prev[l_prev_pos].m_freq < l_cur[l_cur_pos].m_freq + l_freq_threshold &&
           l_prev[l_prev_pos].m_amp > l_cur[l_cur_pos].m_amp - l_amp_threshold &&
           l_prev[l_prev_pos].m_amp < l_cur[l_cur_pos].m_amp + l_amp_threshold) {
		l_prev[l_prev_pos].attachNext(&l_cur[l_cur_pos]);
	    }

	    //increment the one with the smallest freq (because they are orded by freq)
	    if(l_prev[l_prev_pos].m_freq < l_cur[l_cur_pos].m_freq) l_prev_pos++;
	    else l_cur_pos++;
	}
    }
};
    
#endif

