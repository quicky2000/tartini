#include "myglfonts.h"
#include <QImage>
#include <QPainter>
#include <QColor>

//------------------------------------------------------------------------------
void MyGLFont::init(const QFont f)
{
  QImage image(32, 32, QImage::Format_RGB32);
  QPainter p(&image);
  p.setFont(f);
  p.setPen(QColor(255, 255, 255));
  QFontMetrics fm = p.fontMetrics();
  int h = fm.height();
  int w, w_raw;
  GLubyte *data;
  QString s;
  for(int j = 0; j < 255; j++)
    {
      unsigned char ch = j;
      s = ch;
      w = fm.width(ch);
      w_raw = w + 3;
      w_raw -= (w_raw % 4); //round length up to multiple of 4
      data = (GLubyte*)malloc(w_raw * h * sizeof(GLubyte));
      for(int y = 0; y < h; y++)
	{
	  for(int x = 0; x < w_raw; x++)
	    {
	      image.setPixel(x, y, qRgb(0, 0, 0));
	    }
	}
      p.drawText(0, 0, 32, 32, Qt::AlignLeft, s);
      for(int y = 0; y < h; y++)
	{
	  for(int x = 0; x < w_raw; x++)
	    {
	      data[x + (h - 1 - y) * w_raw] = qRed(image.pixel(x,y)); //build our font upside down
	    }
	}
      c[ch] = new MyGLChar(w, h, data, w_raw);
    }
  beenInit = true;
}

//------------------------------------------------------------------------------
MyGLFont::~MyGLFont(void)
{
  if(beenInit)
    {
      for(int j = 0; j < 255; j++)
	{
	  delete c[j];
	}
    }
}

//------------------------------------------------------------------------------
void MyGLFont::drawGLtext(float x, float y, QString s, int w, int h)
{
  beginGLtext(w, h);
  drawGLtextRaw(x, y, s);
  endGLtext();
}

//------------------------------------------------------------------------------
void MyGLFont::drawGLtextRaw(float x, float y, QString s)
{
  unsigned char ch;
  for(int j = 0; j < s.size(); ++j)
    {
      ch = s.at(j).toAscii();
      glRasterPos2f(x, y);
      glDrawPixels(c[ch]->w_raw(), c[ch]->h(), GL_ALPHA, GL_UNSIGNED_BYTE, c[ch]->data());
      x += c[ch]->w();
    }
}

//------------------------------------------------------------------------------
void MyGLFont::beginGLtext(int w, int h)
{
  glPushAttrib(GL_ALL_ATTRIB_BITS);

  glDisable(GL_TEXTURE_1D);
  glDisable(GL_TEXTURE_2D);
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);

  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  gluOrtho2D(0, w, 0, h);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
}

//------------------------------------------------------------------------------
void MyGLFont::endGLtext(void)
{
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glPopAttrib();
}

//EOF
