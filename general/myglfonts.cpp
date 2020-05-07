#include "myglfonts.h"
#include <QImage>
#include <QPainter>
#include <QColor>
#include <QtGlobal>
#include <new>

//------------------------------------------------------------------------------
void MyGLFont::init(const QFont p_font)
{
    QImage l_image(32, 32, QImage::Format_RGB32);
    QPainter l_painter(&l_image);
    l_painter.setFont(p_font);
    l_painter.setPen(QColor(255, 255, 255));
    QFontMetrics l_font_metric = l_painter.fontMetrics();
    int l_height = l_font_metric.height();
    int l_width, l_width_raw;
    GLubyte * l_data;
    QString l_string;
    for(int l_j = 0; l_j < 255; l_j++)
    {
        unsigned char l_char = l_j;
        l_string = l_char;
#if (QT_VERSION >= QT_VERSION_CHECK(5, 11, 0))
        l_width = l_font_metric.horizontalAdvance(l_char);
#else // QT_VERSION >= QT_VERSION_CHECK(5, 11, 0)
        l_width = l_font_metric.width(l_char);
#endif // QT_VERSION >= QT_VERSION_CHECK(5, 11, 0)
        l_width_raw = l_width + 3;
        l_width_raw -= (l_width_raw % 4); //round length up to multiple of 4
        l_data = (GLubyte*)malloc(l_width_raw * l_height * sizeof(GLubyte));
        if(nullptr == l_data)
        {
            throw std::bad_alloc();
        }
        for(int l_y = 0; l_y < l_height; l_y++)
        {
            for(int l_x = 0; l_x < l_width_raw; l_x++)
            {
                l_image.setPixel(l_x, l_y, qRgb(0, 0, 0));
            }
        }
        l_painter.drawText(0, 0, 32, 32, Qt::AlignLeft, l_string);
        for(int l_y = 0; l_y < l_height; l_y++)
        {
            for(int l_x = 0; l_x < l_width_raw; l_x++)
            {
                l_data[l_x + (l_height - 1 - l_y) * l_width_raw] = qRed(l_image.pixel(l_x, l_y)); //build our font upside down
            }
        }
        m_chars[l_char] = new MyGLChar(l_width, l_height, l_data, l_width_raw);
    }
    m_been_init = true;
}

//------------------------------------------------------------------------------
MyGLFont::~MyGLFont()
{
    if(m_been_init)
    {
        for(int l_j = 0; l_j < 255; l_j++)
        {
            delete m_chars[l_j];
        }
    }
}

//------------------------------------------------------------------------------
void MyGLFont::drawGLtext( float p_x
                         , float p_y
                         , QString p_string
                         , int p_width
                         , int p_height
                         ) const
{
    beginGLtext(p_width, p_height);
    drawGLtextRaw(p_x, p_y, p_string);
    endGLtext();
}

//------------------------------------------------------------------------------
void MyGLFont::drawGLtextRaw( float p_x
                            , float p_y
                            , QString p_string
                            ) const
{
    unsigned char l_char;
    for(int l_j = 0; l_j < p_string.size(); ++l_j)
    {
        l_char = p_string.at(l_j).toLatin1();
        glRasterPos2f(p_x, p_y);
        glDrawPixels(m_chars[l_char]->w_raw(), m_chars[l_char]->h(), GL_ALPHA, GL_UNSIGNED_BYTE, m_chars[l_char]->data());
        p_x += m_chars[l_char]->w();
    }
}

//------------------------------------------------------------------------------
void MyGLFont::beginGLtext( int p_width
                          , int p_height
                          ) const
{
    glPushAttrib(GL_ALL_ATTRIB_BITS);


    glDisable(GL_TEXTURE_1D);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, p_width, 0, p_height);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
}

//------------------------------------------------------------------------------
void MyGLFont::endGLtext() const
{
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glPopAttrib();
}

//EOF
