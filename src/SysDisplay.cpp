#include "sysPlatform/SysDisplay.h"

namespace SysPlatform {

SysDisplay sysDisplay;

const unsigned SysDisplay::SYS_BLACK = 0;
const unsigned SysDisplay::SYS_WHITE = 1;
const unsigned SysDisplay::SYS_DISPLAY_HEIGHT = 64;
const unsigned SysDisplay::SYS_DISPLAY_WIDTH  = 128;

struct SysDisplay::impl {
    int dummy;
};

SysDisplay::SysDisplay()
: m_pimpl(std::make_shared<impl>())
{

}

SysDisplay::~SysDisplay()
{

}

void SysDisplay::begin()
{
    
}

unsigned SysDisplay::getHeight()
{
    return SYS_DISPLAY_HEIGHT;
}

unsigned SysDisplay::getWidth()
{
    return SYS_DISPLAY_WIDTH;
}

void SysDisplay::setFont(Font font)
{

}

void SysDisplay::setTextColor(uint16_t color)
{

}

void SysDisplay::setCursor(int16_t x, int16_t y)
{

}

void SysDisplay::getTextBounds(const char *string, int16_t x, int16_t y, int16_t *x1,
    int16_t *y1, uint16_t *w, uint16_t *h)
{
    
}

void SysDisplay::clearDisplay()
{

}

void SysDisplay::invertDisplay(bool invert)
{

}

void SysDisplay::display()
{

}

void SysDisplay::fillScreen(uint16_t color)
{

}

void SysDisplay::drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1,
                    uint16_t color)
{

}

void SysDisplay::drawRect(int16_t x, int16_t y, int16_t w, int16_t h,
                    uint16_t color)
{

}

void SysDisplay::drawBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h,
                  uint16_t color)
{

}

int SysDisplay::printf(const char* str, const char *fmt, ...)
{
    int result = 0;
    va_list args;

    va_start(args, fmt);
    //result = printf(str, fmt, args);
    va_end(args);

    return result;
}

}
