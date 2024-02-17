#include <cstdint>
#include <cstdarg>
#include "display/Adafruit_SPD0301.h"
#include "display/Adafruit_GFX.h"
#include "FreeSansBold9pt7b.h"
#include "FreeSansBold12pt7b.h"
#include "OpenSansBold8p0.h"


#include "sysPlatform/SysDisplay.h"

#define DisplayType Adafruit_SPD0301

namespace SysPlatform {

const unsigned SysDisplay::SYS_BLACK = 0;
const unsigned SysDisplay::SYS_WHITE = 1;
const unsigned SysDisplay::SYS_DISPLAY_HEIGHT = 64;
const unsigned SysDisplay::SYS_DISPLAY_WIDTH  = 128;

// SPI0 pinouts for Mutliverse
constexpr uint8_t SPI0_CS_PIN   = 8;  // RPI GPIO8
constexpr uint8_t SPI0_DC_PIN   = 23;  // RPI GPIO23
constexpr uint8_t SPI0_RST_PIN  = 24;  // RPI GPIO24

DisplayType* rawDisplayPtr = nullptr;

// struct SysDisplay::impl {
//     int dummy;
// };

SysDisplay sysDisplay;

SysDisplay::SysDisplay()
//: m_pimpl(nullptr)
{
    rawDisplayPtr = new DisplayType(SPI0_DC_PIN, SPI0_RST_PIN, SPI0_CS_PIN); // The Avalon OLED display is on SPI0;
}

SysDisplay::~SysDisplay()
{
    if (rawDisplayPtr) delete rawDisplayPtr;
}

void SysDisplay::begin()
{
    rawDisplayPtr->begin();
    //rawDisplayPtr->flipVertical(true);
}

void SysDisplay::setFont(Font font)
{
    switch (font) {
        case Font::DEFAULT_SMALL : rawDisplayPtr->setFont(&Open_Sans_Bold_Small); break;
        case Font::DEFAULT_LARGE :
        default:
            rawDisplayPtr->setFont(&FreeSansBold9pt7b); break;
    }
}

void SysDisplay::getTextBounds(const char *string, int16_t x, int16_t y, int16_t *x1,
                     int16_t *y1, uint16_t *w, uint16_t *h)
{
    rawDisplayPtr->getTextBounds(string, x, y, x1, y1, w, h);
}

void SysDisplay::setTextColor(uint16_t color)
{
    rawDisplayPtr->setTextColor(color);
}

void SysDisplay::setCursor(int16_t x, int16_t y)
{
    rawDisplayPtr->setCursor(x,y);
}

void SysDisplay::clearDisplay()
{
    rawDisplayPtr->clearDisplay();
}

void SysDisplay::invertDisplay(bool invert)
{
    rawDisplayPtr->invertDisplay(uint8_t(invert));
}

void SysDisplay::flipVertical(bool flip)
{
    rawDisplayPtr->flipVertical(flip);
}

int SysDisplay::printf(const char* str, const char *fmt, ...)
{
    int result;
    va_list args;

    va_start(args, fmt);
    result = rawDisplayPtr->printf(str, fmt, args);
    va_end(args);

    return result;
}

void SysDisplay::display()
{
    rawDisplayPtr->display();
}

void SysDisplay::fillScreen(uint16_t color)
{
    rawDisplayPtr->fillScreen(color);
}

void SysDisplay::drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1,
                    uint16_t color)
{
    rawDisplayPtr->drawLine(x0, y0, x1, y1, color);
}

void SysDisplay::drawRect(int16_t x, int16_t y, int16_t w, int16_t h,
                    uint16_t color)
{
    rawDisplayPtr->drawRect(x,y,w,h,color);
}

void SysDisplay::drawBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h,
                uint16_t color)
{
    rawDisplayPtr->drawBitmap(x,y,bitmap,w,h,color);
}

}