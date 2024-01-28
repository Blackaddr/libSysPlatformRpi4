#include "sysPlatform/SysDisplay.h"

namespace SysPlatform {

SysDisplay sysDisplay;

SysDisplay::SysDisplay()
{

}

SysDisplay::~SysDisplay()
{

}

void SysDisplay::begin()
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