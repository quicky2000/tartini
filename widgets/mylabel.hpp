/*    This file is part of Tartini
      Copyright (C) 2016  Julien Thevenon ( julien_thevenon at yahoo.fr )
      Copyright (C) 2004 by Philip McLeod ( pmcleod@cs.otago.ac.nz )

      This program is free software: you can redistribute it and/or modify
      it under the terms of the GNU General Public License as published by
      the Free Software Foundation, either version 3 of the License, or
      (at your option) any later version.

      This program is distributed in the hope that it will be useful,
      but WITHOUT ANY WARRANTY; without even the implied warranty of
      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
      GNU General Public License for more details.

      You should have received a copy of the GNU General Public License
      along with this program.  If not, see <http://www.gnu.org/licenses/>
*/

//------------------------------------------------------------------------------
const std::string & MyLabel::text() const
{
    return m_text;
}

//------------------------------------------------------------------------------
void MyLabel::setText(const std::string & p_text)
{
    m_text = p_text;
    compute_text_width();
    update();
}
  
//------------------------------------------------------------------------------
QSize MyLabel::sizeHint() const
{
    return QSize(m_text_width + 18, m_font_height + 4);
}

//------------------------------------------------------------------------------
void MyLabel::compute_text_width()
{
    if(get_painter().isActive())
    {
        const QFontMetrics & l_font_metric = get_painter().fontMetrics();
        m_font_height = l_font_metric.height();
#if (QT_VERSION >= QT_VERSION_CHECK(5, 11, 0))
        m_text_width = l_font_metric.horizontalAdvance(QString::fromStdString(m_text));
#else // QT_VERSION >= QT_VERSION_CHECK(5, 11, 0)
        m_text_width = l_font_metric.width(QString::fromStdString(m_text));
#endif // QT_VERSION >= QT_VERSION_CHECK(5, 11, 0)
        m_size_computed = true;
    }
    else
    {
        // Compute size with default font size values
        m_font_height = 18;
        m_text_width = m_text.length() * 10;
        m_size_computed = false;
    }
}
// EOF
