/***************************************************************************
                          vibratotimeaxis.cpp  -  description
                             -------------------
    begin                : May 18 2005
    copyright            : (C) 2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
    copyright            : (C) 2019 by Julien Thevenon
    email                : julien_thevenon at yahoo.fr

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/
#include "vibratotimeaxis.h"
#include "gdata.h"
#include "channel.h"
#include "analysisdata.h"

//------------------------------------------------------------------------------
VibratoTimeAxis::VibratoTimeAxis(QWidget *p_parent, int p_nls):
  DrawWidget(p_parent)
{
  setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed, false));

  // The horizontal space in pixels a note label requires + 2 for the border of the VibratoWidget
  m_note_label_offset = p_nls + 2;
  m_start_chunk_to_use = -1;
  m_current_chunk_to_use = -1;
  m_end_chunk_to_use = -1;
  m_note_length_to_use = 0.0;
  m_prev_current_chunk = -1;
  m_prev_window_offset = -999999;
  m_zoom_factor_X = 2.0;
}

//------------------------------------------------------------------------------
VibratoTimeAxis::~VibratoTimeAxis(void)
{
}

//------------------------------------------------------------------------------
void VibratoTimeAxis::paintEvent(QPaintEvent *)
{
  beginDrawing(false);

  fillBackground(colorGroup().background());

  doUpdate();

  if (m_current_chunk_to_use >= 0)
    {
      QFontMetrics l_font_metrics = get_painter().fontMetrics();
      QString l_string;
      get_painter().setBrush(Qt::black);
      get_painter().setFont(QFont("AnyStyle", 12));

      int l_poly_length_in_pixels = toInt((m_end_chunk_to_use - m_start_chunk_to_use) * m_zoom_factor_X);
      float l_pixels_per_second = l_poly_length_in_pixels / m_note_length_to_use;
      int l_notches_divider = 2;
      double l_seconds_per_notch = 5;
      int l_calculation_step = 1;

      // Calculate which notches and labels to draw
      for (int l_pixels_per_second_threshold = 25; ; l_pixels_per_second_threshold *= 2)
	{
	  if (l_pixels_per_second < l_pixels_per_second_threshold)
	    {
	      break;
	    }
	  else
	    {
	      switch (l_calculation_step)
		{
		case 1:
		  l_notches_divider = 5;
		  l_seconds_per_notch /= 5;
		  l_calculation_step = 2;
		  break;
		case 2:
		  l_notches_divider = 2;
		  l_seconds_per_notch = l_seconds_per_notch;
		  l_calculation_step = 3;
		  break;
		case 3:
		  l_notches_divider = 2;
		  l_seconds_per_notch /= 2;
		  l_calculation_step = 1;
		  break;
		}
	    }
	}

      // Draw the notches + labels
      for (int l_index = 0; l_index < (m_note_length_to_use / l_seconds_per_notch); l_index++)
	{
	  int l_x = toInt((((m_end_chunk_to_use - m_start_chunk_to_use) * m_zoom_factor_X) / m_note_length_to_use) * l_index * l_seconds_per_notch - m_window_offset_to_use);
	  if ((l_x >= 0) && (l_x < width()))
	    {
	      if (l_index % l_notches_divider == 0)
		{
		  // Even: bigger notch + label
		  get_painter().drawLine(l_x, height() - 6, l_x, height() - 1);
		  // The 1.000001 factors in the following statements prevent freaky rounding/floor errors...
		  int l_minutes = intFloor(l_index * l_seconds_per_notch * 1.000001) / 60;
		  int l_seconds = intFloor(l_index * l_seconds_per_notch * 1.000001) % 60;
		  int l_thousandth_seconds = intFloor(1000 * l_index *l_seconds_per_notch * 1.000001) % 1000;
		  if (l_thousandth_seconds == 0)
		    {
		      // Label: m:ss
		      l_string.sprintf("%1d:%02d", l_minutes, l_seconds);
		    }
		  else if (l_thousandth_seconds % 100 == 0)
		    {
		      // Label: m:ss.h
		      l_string.sprintf("%1d:%02d.%01d", l_minutes, l_seconds, l_thousandth_seconds / 100);
		    }
		  else if (l_thousandth_seconds % 10 == 0)
		    {
		      // Label: m:ss.hh
		      l_string.sprintf("%1d:%02d.%02d", l_minutes, l_seconds, l_thousandth_seconds / 10);
		    }
		  else
		    {
		      // Label: m:ss.hhh
		      l_string.sprintf("%1d:%02d.%03d", l_minutes, l_seconds, l_thousandth_seconds);
		    }
		  get_painter().drawText(l_x - l_font_metrics.width(l_string) / 2, 12, l_string);
		}
	      else
		{
		  // Odd: smaller notch
		  get_painter().drawLine(l_x, height() - 3, l_x, height() - 1);
		}
	    }
	}

      // Draw the horizontal line at the bottom of the axis
      get_painter().drawLine(0, height() - 1, width(), height() - 1);
    }
  endDrawing();
}

//------------------------------------------------------------------------------
void VibratoTimeAxis::resizeEvent(QResizeEvent *)
{
  // Do forced update on resize
  m_prev_current_chunk = -1;
  update();
}

//------------------------------------------------------------------------------
void VibratoTimeAxis::doUpdate(void)
{
  Channel * l_active_channel = gdata->getActiveChannel();

  int l_my_start_chunk = -1;
  int l_my_current_chunk = -1;
  int l_my_end_chunk = -1;
  double l_my_note_length = 0.0;
  int l_my_window_offset = -999999;

  if (l_active_channel)
    {
      AnalysisData * l_data = l_active_channel->dataAtCurrentChunk();
      if(l_data && l_active_channel->isVisibleNote(l_data->getNoteIndex()) && l_active_channel->isLabelNote(l_data->getNoteIndex()))
	{
	  const NoteData * l_note = new NoteData();
	  l_note = &l_active_channel->get_note_data()[l_data->getNoteIndex()];

	  l_my_start_chunk = l_note->startChunk();
	  l_my_current_chunk = l_active_channel->chunkAtCurrentTime();
	  l_my_end_chunk = l_note->endChunk();
	  l_my_note_length = l_note->noteLength();

	  // Calculate windowoffset
	  if ((l_my_end_chunk - l_my_start_chunk) * m_zoom_factor_X > width() - 2 * m_note_label_offset)
	    {
	      // The vibrato-polyline doesn't fit in the window
	      if ((l_my_current_chunk - l_my_start_chunk) * m_zoom_factor_X < (width() - 2 * m_note_label_offset) / 2)
		{
		  // We're at the left side of the vibrato-polyline
		  l_my_window_offset = 0 - m_note_label_offset;
		}
	      else if ((l_my_end_chunk - l_my_current_chunk) * m_zoom_factor_X < (width() - 2 * m_note_label_offset) / 2)
		{
		  // We're at the right side of the vibrato-polyline
		  l_my_window_offset = toInt((l_my_end_chunk - l_my_start_chunk) * m_zoom_factor_X - width() + m_note_label_offset + 1);
		}
	      else
		{
		  // We're somewhere in the middle of the vibrato-polyline
		  l_my_window_offset = toInt((l_my_current_chunk - l_my_start_chunk) * m_zoom_factor_X - width() / 2);
		}
	    }
	  else
	    {
	      // The vibrato-polyline does fit in the window
	      l_my_window_offset = 0 - m_note_label_offset;
	    }
	}
    }

  if (l_my_current_chunk == -1)
    {
      // No note
      if (m_prev_current_chunk == l_my_current_chunk)
	{
	  // Still no timeaxis needed, no update needed
	}
      else
	{
	  // Timeaxis should be erased, update widget
	  m_prev_current_chunk = -1;
	  m_prev_window_offset = -999999;
	  m_current_chunk_to_use = -1;
	}
    }
  else
    {
      // Note
      if (m_prev_window_offset == l_my_window_offset)
	{
	  // No movement, don't redraw timeaxis
	}
      else
	{
	  // Position in note to draw has changed, so draw the timeaxis
	  m_prev_current_chunk = l_my_current_chunk;
	  m_prev_window_offset = l_my_window_offset;
	  m_start_chunk_to_use = l_my_start_chunk;
	  m_current_chunk_to_use = l_my_current_chunk;
	  m_end_chunk_to_use = l_my_end_chunk;
	  m_note_length_to_use = l_my_note_length;
	  m_window_offset_to_use = l_my_window_offset;
	}
    }
}

//------------------------------------------------------------------------------
void VibratoTimeAxis::setZoomFactorX(double x)
{
  m_zoom_factor_X = 2 * pow10(log2(x / 25));

  // Do forced update on changed zoomwheel
  m_prev_window_offset = -1;

  update();
}

//------------------------------------------------------------------------------
QSize VibratoTimeAxis::sizeHint(void) const
{
  return QSize(100, 20);
}
// EOF
