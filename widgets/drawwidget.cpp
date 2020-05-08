/***************************************************************************
                          drawwidget.cpp  -  description
                             -------------------
    begin                : Fri Dec 10 2004
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
 
#include <QThread>
#include <QPolygon>
#include <utility>
#include <algorithm>
#include <QPixmap>

#include "drawwidget.h"
#include "channel.h"
#include "view.h"
#include "gdata.h"
#include "analysisdata.h"
#include "useful.h"
#include "conversions.h"
#include "myqt.h"

#ifndef CERTAIN_THRESHOLD
#define CERTAIN_THRESHOLD 0.9
#endif

int DrawWidget::m_line_width = 3;
int DrawWidget::m_line_top_half_width = 2;
int DrawWidget::m_line_bottom_half_width = 1;

//------------------------------------------------------------------------------
DrawWidget::DrawWidget(QWidget * p_parent, const char * p_name):
  QWidget(p_parent)
{
  setObjectName(p_name);
#ifdef SHARED_DRAWING_BUFFER
  m_buffer = gdata->drawingBuffer();
  m_buffer->resize(m_buffer->size().expandedTo(size()));
  m_paint_device = m_buffer;
#elif SINGLE_DRAWING_BUFFER
  m_paint_device = this;
#else // SINGLE_DRAWING_BUFFER
  m_buffer = new QPixmap(size());
  m_paint_device = m_buffer;
#endif // SHARED_DRAWING_BUFFER
  
  setLineWidth(3);
  
  // With double buffering, we don't want QT to clear the display all the time
  setAttribute(Qt::WA_OpaquePaintEvent);
}

//------------------------------------------------------------------------------
DrawWidget::~DrawWidget()
{
#ifdef SHARED_DRAWING_BUFFER
#elif SINGLE_DRAWING_BUFFER
#else // SINGLE_DRAWING_BUFFER
  delete m_buffer;
#endif // SHARED_DRAWING_BUFFER
}

//------------------------------------------------------------------------------
void DrawWidget::beginDrawing(bool p_clear_background)
{
  checkSize();
  m_painter.begin(m_paint_device);
#ifndef SINGLE_DRAWING_BUFFER
  m_painter.initFrom(this);
#endif // SINGLE_DRAWING_BUFFER
  if(p_clear_background)
    {
      clearBackground();
    }
}

//------------------------------------------------------------------------------
void DrawWidget::endDrawing(bool p_draw_to_screen)
{
  m_painter.end();
  if(p_draw_to_screen)
    {
      drawToScreen();
    }
}

//------------------------------------------------------------------------------
void DrawWidget::drawToScreen()
{
#ifndef SINGLE_DRAWING_BUFFER
  bitBlt(this, 0, 0, m_buffer, 0, 0, width(), height());
#endif // SINGLE_DRAWING_BUFFER
}

//------------------------------------------------------------------------------
void DrawWidget::clearBackground()
{
  m_painter.fillRect(0, 0, width(), height(), g_data->backgroundColor());
}

//------------------------------------------------------------------------------
void DrawWidget::fillBackground(const QColor & p_color)
{
  m_painter.fillRect(0, 0, width(), height(), p_color);
}

//------------------------------------------------------------------------------
void DrawWidget::checkSize()
{
#ifdef SHARED_DRAWING_BUFFER
  if(width() > m_buffer->width() || height() > m_buffer->height())
    {
      m_buffer->resize(m_buffer->size().expandedTo(size()));
    }
#elif SINGLE_DRAWING_BUFFER
#else // SINGLE_DRAWING_BUFFER
  if(m_buffer->size() != size())
    {
      //resize the local double buffer
      m_buffer->resize(size());
    }
#endif // SHARED_DRAWING_BUFFER
}

//------------------------------------------------------------------------------
void DrawWidget::setLineWidth(int p_width)
{
  m_line_width = p_width;
  m_line_top_half_width = m_line_width / 2 + (m_line_width % 2);
  m_line_bottom_half_width = m_line_width / 2;
}

//------------------------------------------------------------------------------
void DrawWidget::drawChannel(QPaintDevice & p_paint_device,
			     Channel * p_channel,
			     QPainter & p_painter,
			     double p_left_time,
			     double p_current_time,
			     double p_zoom_X,
			     double p_view_bottom,
			     double p_zoom_Y,
			     int p_view_type
			     )
{
  ZoomLookup * l_zoom_lookup;
  if(p_view_type == DRAW_VIEW_SUMMARY)
    {
      l_zoom_lookup = & p_channel->get_summary_zoom_lookup();
    }
  else
    {
      l_zoom_lookup = & p_channel->get_normal_zoom_lookup();
    }
  ChannelLocker l_channel_locker(p_channel);

  QColor l_current_color = p_channel->get_color();
  p_painter.setPen(l_current_color);

  int l_view_bottom_offset = toInt(p_view_bottom / p_zoom_Y);
  printf("l_view_bottom_offset=%d, %f, %f\n", l_view_bottom_offset, p_view_bottom, p_zoom_Y);

  // l_base_X is the no. of chunks a pixel must represent.
  double l_base_X = p_zoom_X / p_channel->timePerChunk();

  l_zoom_lookup->setZoomLevel(l_base_X); 
  double l_current_chunk = p_channel->chunkFractionAtTime(p_current_time);
  double l_left_frame_time = l_current_chunk - ((p_current_time - p_left_time) / p_channel->timePerChunk());
  

  double l_frame_time = l_left_frame_time;
  int l_n = 0;
  int l_base_element = int(floor(l_frame_time / l_base_X));
  if(l_base_element < 0)
    {
      l_n -= l_base_element;
      l_base_element = 0;
    }
  int l_last_base_element = int(floor(double(p_channel->totalChunks()) / l_base_X));
  
  QPolygon pointArray(p_paint_device.width()*2);
      
  if (l_base_X > 1)
    {
      // More samples than pixels
      int l_the_width = p_paint_device.width();
      if(l_last_base_element > l_zoom_lookup->size())
	{
	  l_zoom_lookup->setSize(l_last_base_element);
	}
      for(; l_n < l_the_width && l_base_element < l_last_base_element; l_n++, l_base_element++)
	{
	  myassert(l_base_element >= 0);
	  ZoomElement & l_zoom_element = l_zoom_lookup->at(l_base_element);
	  if(!l_zoom_element.isValid())
	    {
	      if(calcZoomElement(p_channel, l_zoom_element, l_base_element, l_base_X))
		{
		  continue;
		}
	    }
     
	  if(l_zoom_element.high() != 0.0f && l_zoom_element.high() - l_zoom_element.low() < 1.0)
	    {
	      //if range is closer than one semi-tone then draw a line between them
	      p_painter.setPen(l_zoom_element.color());
	      p_painter.drawLine(l_n, p_paint_device.height() - m_line_top_half_width - toInt(l_zoom_element.high() / p_zoom_Y) + l_view_bottom_offset, l_n, p_paint_device.height() + m_line_bottom_half_width - toInt(l_zoom_element.low() / p_zoom_Y) + l_view_bottom_offset);
	    }
	}
    }
  else
    {
      // More pixels than samples
      float l_err = 0.0, l_pitch = 0.0, l_prev_pitch = 0.0, vol;

      // Integer version of frame time
      int l_int_chunk = static_cast<int>(floor(l_frame_time));
      if(l_int_chunk < 0)
	{
	  l_int_chunk = 0;
	}

      // So we skip some pixels
      double l_step_size = 1.0 / l_base_X;
      int l_x = 0, l_y;

      double l_start = (double(l_int_chunk) - l_frame_time) * l_step_size;
      double l_stop = p_paint_device.width() + (2 * l_step_size);

      //make it an odd number
      int l_square_size = (int(sqrt(l_step_size)) / 2) * 2 + 1;
      int l_half_square_size = l_square_size / 2;
      int l_pen_X = 0, l_pen_Y = 0;

      for (double l_n = l_start; l_n < l_stop && l_int_chunk < static_cast<int>(p_channel->totalChunks()); l_n += l_step_size, l_int_chunk++)
	{
	  myassert(l_int_chunk >= 0);
	  AnalysisData * l_data = p_channel->dataAtChunk(l_int_chunk);
	  l_err = l_data->getCorrelation();
	  vol = dB2Normalised(l_data->getLogRms(), p_channel->get_rms_ceiling(), p_channel->get_rms_floor());
	  if(g_data->pitchContourMode() == 0)
	    {
	      if(p_view_type == DRAW_VIEW_PRINT)
		{
		  p_painter.setPen(QPen(colorBetween(QColor(255, 255, 255), p_channel->get_color(), l_err * vol), m_line_width));
		}
	      else
		{
		  p_painter.setPen(QPen(colorBetween(g_data->backgroundColor(), p_channel->get_color(), l_err * vol), m_line_width));
		}
	    }
	  else
	    {
	      p_painter.setPen(QPen(p_channel->get_color(), m_line_width));
	    }
      
	  l_x = toInt(l_n);
	  l_pitch = (p_channel->isVisibleChunk(l_data)) ? l_data->getPitch() : 0.0f;
	  myassert(l_pitch >= 0.0 && l_pitch <= g_data->topPitch());
	  l_y = p_paint_device.height() - 1 - toInt(l_pitch / p_zoom_Y) + l_view_bottom_offset;
	  if(l_pitch > 0.0f)
	    {
	      if(fabs(l_prev_pitch - l_pitch) < 1.0 && l_n != l_start)
		{
		  //if closer than one semi-tone from previous then draw a line between them
		  p_painter.drawLine(l_pen_X, l_pen_Y, l_x, l_y);
		  l_pen_X = l_x;
		  l_pen_Y = l_y;
		}
	      else
		{
		  p_painter.drawPoint(l_x, l_y);
		  l_pen_X = l_x;
		  l_pen_Y = l_y;
		}
	      if(l_step_size > 10)
		{
		  //draw squares on the data points
		  p_painter.setBrush(Qt::NoBrush);
		  p_painter.drawRect(l_x - l_half_square_size, l_y - l_half_square_size, l_square_size, l_square_size);
		}
	    }
	  l_prev_pitch = l_pitch;
	}
    }
}

//------------------------------------------------------------------------------
void DrawWidget::drawChannelFilled(Channel * p_channel,
				   QPainter & p_painter,
				   double p_left_time,
				   double p_current_time,
				   double p_zoom_X,
				   double p_view_bottom,
				   double p_zoom_Y,
				   int p_view_type
				   )
{
  ZoomLookup * l_zoom_lookup;
  if(p_view_type == DRAW_VIEW_SUMMARY)
    {
      l_zoom_lookup = & p_channel->get_summary_zoom_lookup();
    }
  else
    {
      l_zoom_lookup = &p_channel->get_normal_zoom_lookup();
    }

  ChannelLocker l_channel_locker(p_channel);

  QColor l_current = p_channel->get_color();
  p_painter.setPen(l_current);

  int l_view_bottom_offset = toInt(p_view_bottom / p_zoom_Y);

  // l_base_X is the no. of chunks a pixel must represent.
  double l_base_X = p_zoom_X / p_channel->timePerChunk();

  l_zoom_lookup->setZoomLevel(l_base_X);

  double l_current_chunk = p_channel->chunkFractionAtTime(p_current_time);
  double l_left_frame_time = l_current_chunk - ((p_current_time - p_left_time) / p_channel->timePerChunk());

  double l_frame_time = l_left_frame_time;
  int l_n = 0;
  int l_base_element = int(floor(l_frame_time / l_base_X));
  if(l_base_element < 0)
    {
      l_n -= l_base_element;
      l_base_element = 0;
    }
  int l_last_base_element = int(floor(double(p_channel->totalChunks()) / l_base_X));
  
  int l_first_N = l_n;
  int l_last_N = l_first_N;

  QPolygon l_bottom_points(width() * 2);
  QPolygon l_even_mid_points(width() * 2);
  QPolygon l_odd_mid_points(width() * 2);
  QPolygon l_even_mid_points2(width() * 2);
  QPolygon l_odd_mid_points2(width() * 2);
  std::vector<QRect> l_note_rect(width() * 2);
  std::vector<QRect> l_note_rect2(width() * 2);
  std::vector<bool> l_is_note_rect_even(width() * 2);

  int l_point_index = 0;
  int l_even_mid_point_index = 0;
  int l_odd_mid_point_index = 0;
  int l_even_mid_point_index2 = 0;
  int l_odd_mid_point_index2 = 0;
  int l_rect_index = 0;
  int l_rect_index2 = 0;

  if (l_base_X > 1)
    {
      // More samples than pixels
      int l_the_width = width();
      if(l_last_base_element > l_zoom_lookup->size())
	{
	  l_zoom_lookup->setSize(l_last_base_element);
	}
      for(; l_n < l_the_width && l_base_element < l_last_base_element; l_n++, l_base_element++)
	{
	  myassert(l_base_element >= 0);
	  ZoomElement & l_zoom_element = l_zoom_lookup->at(l_base_element);
	  if(!l_zoom_element.isValid())
	    {
	      if(!calcZoomElement(p_channel, l_zoom_element, l_base_element, l_base_X))
		{
		  continue;
		}
	    }
     
	  int l_y = height() - 1 - toInt(l_zoom_element.high() / p_zoom_Y) + l_view_bottom_offset;
	  int l_y2, l_y3;
	  if(l_zoom_element.noteIndex() != -1 && p_channel->dataAtChunk(l_zoom_element.midChunk())->getNoteIndex() != -1)
	    {
	      myassert(l_zoom_element.noteIndex() >= 0);
	      myassert(l_zoom_element.noteIndex() < int(p_channel->get_note_data().size()));
	      myassert(p_channel->isValidChunk(l_zoom_element.midChunk()));
	      AnalysisData * l_data = p_channel->dataAtChunk(l_zoom_element.midChunk());

	      if(g_data->showMeanVarianceBars())
		{
		  //longTermMean bars
		  l_y2 = height() - 1 - toInt((l_data->getLongTermMean() + l_data->getLongTermDeviation()) / p_zoom_Y) + l_view_bottom_offset;
		  l_y3 = height() - 1 - toInt((l_data->getLongTermMean() - l_data->getLongTermDeviation()) / p_zoom_Y) + l_view_bottom_offset;
		  if(l_zoom_element.noteIndex() % 2 == 0)
		    {
		      l_even_mid_points.setPoint(l_even_mid_point_index++, l_n, l_y2);
		      l_even_mid_points.setPoint(l_even_mid_point_index++, l_n, l_y3);
		    }
		  else
		    {
		      l_odd_mid_points.setPoint(l_odd_mid_point_index++, l_n, l_y2);
		      l_odd_mid_points.setPoint(l_odd_mid_point_index++, l_n, l_y3);
		    }
  
		  //shortTermMean bars
		  l_y2 = height() - 1 - toInt((l_data->getShortTermMean() + l_data->getShortTermDeviation()) / p_zoom_Y) + l_view_bottom_offset;
		  l_y3 = height() - 1 - toInt((l_data->getShortTermMean() - l_data->getShortTermDeviation()) / p_zoom_Y) + l_view_bottom_offset;
		  if(l_zoom_element.noteIndex() % 2 == 0)
		    {
		      l_even_mid_points2.setPoint(l_even_mid_point_index2++, l_n, l_y2);
		      l_even_mid_points2.setPoint(l_even_mid_point_index2++, l_n, l_y3);
		    }
		  else
		    {
		      l_odd_mid_points2.setPoint(l_odd_mid_point_index2++, l_n, l_y2);
		      l_odd_mid_points2.setPoint(l_odd_mid_point_index2++, l_n, l_y3);
		    }
		}
	    }
	  l_bottom_points.setPoint(l_point_index++, l_n, l_y);
	  l_bottom_points.setPoint(l_point_index++, l_n, height());
	  l_last_N = l_n;
	}
      p_painter.setPen(Qt::NoPen);
      p_painter.setBrush(g_data->shading1Color());
      p_painter.drawRect(l_first_N, 0, l_last_N, height());
      p_painter.setPen(g_data->shading2Color());
      if(l_point_index > 1)
	{
	  p_painter.drawLines(l_bottom_points.constData(), l_point_index / 2);
	}

      if(g_data->showMeanVarianceBars())
	{
	  //shortTermMean bars
	  p_painter.setPen(Qt::green);
	  if(l_even_mid_point_index2 > 1)
	    {
	      p_painter.drawLines(l_even_mid_points2.constData(), l_even_mid_point_index2 / 2);
	    }
	  p_painter.setPen(Qt::yellow);
	  if(l_odd_mid_point_index2 > 1)
	    {
	      p_painter.drawLines(l_odd_mid_points2.constData(), l_odd_mid_point_index2 / 2);
	    }

	  //longTermMean bars
	  p_painter.setPen(Qt::yellow);
	  if(l_even_mid_point_index > 1)
	    {
	      p_painter.drawLines(l_even_mid_points.constData(), l_even_mid_point_index / 2);
	    }
	  p_painter.setPen(Qt::green);
	  if(l_odd_mid_point_index > 1)
	    {
	      p_painter.drawLines(l_odd_mid_points.constData(), l_odd_mid_point_index / 2);
	    }
	}
    }
  else
    {
      // More pixels than samples
      float l_err = 0.0;
      float l_pitch = 0.0;
      // Integer version of frame time
      int l_int_chunk = static_cast<int>(floor(l_frame_time));
      if(l_int_chunk < 0)
	{
	  l_int_chunk = 0;
	}
      // So we skip some pixels
      double l_step_size = 1.0 / l_base_X;
      int l_x = 0, l_y, l_y2, l_y3;
  
      double l_start = (double(l_int_chunk) - l_frame_time) * l_step_size;
      double l_stop = width() + (2 * l_step_size);
      l_bottom_points.setPoint(l_point_index++, toInt(l_start), height());
      l_last_N = l_first_N = toInt(l_start);
      for (double l_n = l_start; l_n < l_stop && l_int_chunk < static_cast<int>(p_channel->totalChunks()); l_n += l_step_size, l_int_chunk++)
	{
	  myassert(l_int_chunk >= 0);
	  AnalysisData * l_data = p_channel->dataAtChunk(l_int_chunk);
	  l_err = l_data->getCorrelation();
      
	  if(g_data->pitchContourMode() == 0)
	    {
	      p_painter.setPen(QPen(colorBetween(QColor(255, 255, 255), p_channel->get_color(), l_err * dB2ViewVal(l_data->getLogRms())), m_line_width));
	    }
	  else
	    {
	      p_painter.setPen(QPen(p_channel->get_color(), m_line_width));
	    }

	  l_x = toInt(l_n);
	  l_last_N = l_x;
	  l_pitch = (p_channel->isVisibleChunk(l_data)) ? l_data->getPitch() : 0.0f;
	  if(l_data->getNoteIndex() >= 0)
	    {
	      l_is_note_rect_even[l_rect_index] = (l_data->getNoteIndex() % 2) == 0;

	      if(g_data->showMeanVarianceBars())
		{
		  //longTermMean bars
		  l_y2 = height() - 1 - toInt((l_data->getLongTermMean() + l_data->getLongTermDeviation()) / p_zoom_Y) + l_view_bottom_offset;
		  l_y3 = height() - 1 - toInt((l_data->getLongTermMean() - l_data->getLongTermDeviation()) / p_zoom_Y) + l_view_bottom_offset;
		  l_note_rect[l_rect_index].setLeft(l_x);
		  l_note_rect[l_rect_index].setRight(toInt(l_n + l_step_size));
		  l_note_rect[l_rect_index].setTop(l_y2);
		  l_note_rect[l_rect_index++].setBottom(l_y3);
  
		  //shortTermMean bars
		  l_y2 = height() - 1 - toInt((l_data->getShortTermMean() + l_data->getShortTermDeviation()) / p_zoom_Y) + l_view_bottom_offset;
		  l_y3 = height() - 1 - toInt((l_data->getShortTermMean() - l_data->getShortTermDeviation()) / p_zoom_Y) + l_view_bottom_offset;
		  l_note_rect2[l_rect_index2].setLeft(l_x);
		  l_note_rect2[l_rect_index2].setRight(toInt(l_n + l_step_size));
		  l_note_rect2[l_rect_index2].setTop(l_y2);
		  l_note_rect2[l_rect_index2++].setBottom(l_y3);
		}
	    }
	  myassert(l_pitch >= 0.0 && l_pitch <= g_data->topPitch());
	  l_y = height() - 1 - toInt(l_pitch / p_zoom_Y) + l_view_bottom_offset;
	  l_bottom_points.setPoint(l_point_index++, l_x, l_y);
	}
      l_bottom_points.setPoint(l_point_index, l_bottom_points.point(l_point_index - 1).x(), height());
      l_point_index++;

      myassert(l_point_index <= width() * 2);
      p_painter.setPen(Qt::NoPen);
      p_painter.setBrush(g_data->shading1Color());
      p_painter.drawRect(l_first_N, 0, l_last_N, height());
      p_painter.setBrush(g_data->shading2Color());
      p_painter.drawPolygon(l_bottom_points.constData(), l_point_index, Qt::OddEvenFill);

      if(g_data->showMeanVarianceBars())
	{
	  //shortTermMean bars
	  for(int l_j = 0; l_j < l_rect_index2; l_j++)
	    {
	      if(l_is_note_rect_even[l_j])
		{
		  p_painter.setBrush(Qt::green);
		}
	      else
		{
		  p_painter.setBrush(Qt::yellow);
		}
	      p_painter.drawRect(l_note_rect2[l_j]);
	    }
	  //longTermMean bars
	  QColor l_see_through_yellow = Qt::yellow;
	  l_see_through_yellow.setAlpha(255);
	  QColor l_see_through_green = Qt::green;
	  l_see_through_green.setAlpha(255);
	  for(int l_j = 0; l_j < l_rect_index; l_j++)
	    {
	      if(l_is_note_rect_even[l_j])
		{
		  p_painter.setBrush(l_see_through_yellow);
		}
	      else
		{
		  p_painter.setBrush(l_see_through_green);
		}
	      p_painter.drawRect(l_note_rect[l_j]);
	    }
	}
    }
}

//------------------------------------------------------------------------------
void DrawWidget::setChannelVerticalView(Channel * p_channel,
					double p_left_time,
					double p_current_time,
					double p_zoom_X,
					double , // p_view_bottom,
					double // p_zoom_Y
					)
{
  ZoomLookup * l_zoom_lookup = & p_channel->get_normal_zoom_lookup();
    
  ChannelLocker l_channel_locker(p_channel);

  std::vector<float> l_ys;
  l_ys.reserve(width());
  std::vector<float> l_weightings;
  l_weightings.reserve(width());
  float l_max_Y = 0.0f;
  float l_min_Y = g_data->topPitch();
  float l_total_Y = 0.0f;
  float l_num_Y = 0.0f;
  
  // l_base_X is the no. of chunks a pixel must represent.
  double l_base_X = p_zoom_X / p_channel->timePerChunk();

  l_zoom_lookup->setZoomLevel(l_base_X);
  
  double l_current_chunk = p_channel->chunkFractionAtTime(p_current_time);
  double l_left_frame_time = l_current_chunk - ((p_current_time - p_left_time) / p_channel->timePerChunk());
  
  double l_frame_time = l_left_frame_time;
  int l_n = 0;
  int l_current_base_element = int(floor(l_current_chunk / l_base_X));
  int l_first_base_element = int(floor(l_frame_time / l_base_X));
  int l_base_element = l_first_base_element;
  if(l_base_element < 0)
    {
      l_n -= l_base_element;
      l_base_element = 0;
    }
  int l_last_base_element = int(floor(double(p_channel->totalChunks()) / l_base_X));
  double l_left_base_width = MAX(1.0, double(l_current_base_element - l_first_base_element));
  double l_right_base_width = MAX(1.0, double(l_last_base_element - l_current_base_element));
  
  /*
    We calculate the auto follow and scale by averaging all the note elements in view.
    We weight the frequency averaging by a triangle weighting function centered on the current time.
    Also it is weighted by the corr value of each frame.
             /|\
            / | \
           /  |  \
          /   |   \
            ^   ^
 l_left_base_width l_right_base_width
*/

  
  if (l_base_X > 1)
    {
      // More samples than pixels
      int l_the_width = width();
      if(l_last_base_element > l_zoom_lookup->size())
	{
	  l_zoom_lookup->setSize(l_last_base_element);
	}
      for(; l_n < l_the_width && l_base_element < l_last_base_element; l_n++, l_base_element++)
	{
	  myassert(l_base_element >= 0);
	  ZoomElement & l_zoom_element = l_zoom_lookup->at(l_base_element);
	  if(!l_zoom_element.isValid())
	    {
	      if(!calcZoomElement(p_channel, l_zoom_element, l_base_element, l_base_X))
		{
		  continue;
		}
	    }
	  if(l_zoom_element.low() > 0.0f && l_zoom_element.high() > 0.0f)
	    {
	      float l_weight = l_zoom_element.corr();
	      if(l_base_element < l_current_base_element)
		{
		  l_weight *= double(l_current_base_element - l_base_element) / l_left_base_width;
		}
	      else if(l_base_element > l_current_base_element)
		{
		  l_weight *= double(l_base_element - l_current_base_element) / l_right_base_width;
		}
	      if(l_zoom_element.low() < l_min_Y)
		{
		  l_min_Y = l_zoom_element.low();
		}
	      if(l_zoom_element.high() > l_max_Y)
		{
		  l_max_Y = l_zoom_element.high();
		}
	      l_total_Y += (l_zoom_element.low() + l_zoom_element.high()) / 2.0f * l_weight;
	      l_num_Y += l_weight;
	      l_ys.push_back((l_zoom_element.low() + l_zoom_element.high()) / 2.0f);
	      l_weightings.push_back(l_weight);
	    }
	}
    }
  else
    {
      // More pixels than samples
    float l_pitch = 0.0;
    // Integer version of frame time
    int l_int_chunk = static_cast<int>(floor(l_frame_time));
    if(l_int_chunk < 0)
      {
	l_int_chunk = 0;
      }
    // So we skip some pixels
    double l_step_size = 1.0 / l_base_X;
    float l_corr;
    
    double l_start = (double(l_int_chunk) - l_frame_time) * l_step_size;
    double l_stop = width() + (2 * l_step_size);
    for (double l_current_value = l_start; l_current_value < l_stop && l_int_chunk < static_cast<int>(p_channel->totalChunks()); l_current_value += l_step_size, l_int_chunk++)
      {
	myassert(l_int_chunk >= 0);
	AnalysisData * l_data = p_channel->dataAtChunk(l_int_chunk);
      
	l_pitch = (p_channel->isVisibleChunk(l_data)) ? l_data->getPitch() : 0.0f;
	myassert(l_pitch >= 0.0 && l_pitch <= g_data->topPitch());
	l_corr = l_data->getCorrelation() * dB2ViewVal(l_data->getLogRms());
	if(l_pitch > 0.0f)
	  {
	    float l_weight = l_corr;
	    if(l_min_Y < l_pitch)
	      {
		l_min_Y = l_pitch;
	      }
	    if(l_max_Y > l_pitch)
	      {
		l_max_Y = l_pitch;
	      }
	    l_total_Y += l_pitch * l_weight;
	    l_num_Y += l_weight;
	    l_ys.push_back(l_pitch);
	    l_weightings.push_back(l_weight);
	  }
      }
    }
  
  if(!l_ys.empty() > 0)
    {
      float l_mean_Y = l_total_Y / l_num_Y;
      g_data->getView().setViewBottomRaw(l_mean_Y - g_data->getView().viewHeight() / 2.0);
    }
}

//------------------------------------------------------------------------------
bool DrawWidget::calcZoomElement(Channel * p_channel,
				 ZoomElement & p_zoom_element,
				 int p_base_element,
				 double p_base_X
				 )
{
  int l_start_chunk = toInt(double(p_base_element) * p_base_X);
  int l_finish_chunk = toInt(double(p_base_element + 1) * p_base_X) + 1;
  if(l_finish_chunk >= static_cast<int>(p_channel->totalChunks()))
    {
      //dont go off the end
      l_finish_chunk--;
    }
  if(l_finish_chunk >= static_cast<int>(p_channel->totalChunks()))
    {
      //that data doesn't exist yet
      return false;
    }
  
  std::pair<large_vector<AnalysisData>::iterator, large_vector<AnalysisData>::iterator> l_iter =
    minMaxElement(p_channel->dataIteratorAtChunk(l_start_chunk), p_channel->dataIteratorAtChunk(l_finish_chunk), lessPitch());
  if(l_iter.second == p_channel->dataIteratorAtChunk(l_finish_chunk))
    {
      return false;
    }
  
  large_vector<AnalysisData>::iterator l_err = std::max_element(p_channel->dataIteratorAtChunk(l_start_chunk), p_channel->dataIteratorAtChunk(l_finish_chunk), lessValue(0));
  if(l_err == p_channel->dataIteratorAtChunk(l_finish_chunk))
    {
      return false;
    }
  
  float l_low, l_high;
  int l_note_index;
  if(p_channel->isVisibleChunk(&*l_err))
    {
      l_low = l_iter.first->getPitch();
      l_high = l_iter.second->getPitch();
      l_note_index = l_iter.first->getNoteIndex();
    }
  else
    {
      l_low = 0;
      l_high = 0;
      l_note_index = NO_NOTE;
    }
  float l_corr = l_err->getCorrelation() * dB2Normalised(l_err->getLogRms(), p_channel->get_rms_ceiling(), p_channel->get_rms_floor());
  QColor l_the_color = (g_data->pitchContourMode() == 0) ? colorBetween(g_data->backgroundColor(), p_channel->get_color(), l_corr) : p_channel->get_color();

  p_zoom_element.set(l_low, l_high, l_corr, l_the_color, l_note_index, (l_start_chunk + l_finish_chunk) / 2);
  return true;
}

//------------------------------------------------------------------------------
void DrawWidget::drawArray(float * p_input,
			   int p_n,
			   int p_sample_step,
			   double p_the_zoom_Y,
			   double p_offset
			   )
{
  double l_dh2 = double(height() - 1) / 2.0;
  double l_scale_Y = l_dh2 * p_the_zoom_Y;
  int l_w = width() / p_sample_step;
  QPolygon l_point_array(l_w);
  int l_int_step = int(p_n / l_w);
  int l_remainder_step = p_n - (l_int_step * l_w);
  int l_pos = 0;
  int l_remainder = 0;

  for(int l_j = 0; l_j < l_w; l_j++, l_pos += l_int_step, l_remainder += l_remainder_step)
    {
      if(l_remainder >= l_w)
	{
	  l_pos++;
	  l_remainder -= l_w;
	}
      myassert(l_pos < p_n);
      l_point_array.setPoint(l_j, l_j * p_sample_step, toInt(l_dh2 - (p_input[l_pos] + p_offset) * l_scale_Y));
    }
  m_painter.drawPolyline(l_point_array);
}

// EOF
