/*********************************************************************
This is a library for our Monochrome OLEDs based on SSD1306 drivers

  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/category/63_98

These displays use SPI to communicate, 4 or 5 pins are required to  
interface

Adafruit invests time and resources providing this open source code, 
please support Adafruit and open-source hardware by purchasing 
products from Adafruit!

Written by Limor Fried/Ladyada  for Adafruit Industries.  
BSD license, check license.txt for more information
All text above, and the splash screen must be included in any redistribution
*********************************************************************/

/*********************************************************************
Refactored for SPD0301 by Blackaddr Audio
*********************************************************************/
#include "ArduinoApi.h"

#include <SPI.h>
#include <Adafruit_GFX.h>

#define BLACK 0
#define WHITE 1
#define INVERSE 2

/*=========================================================================
    SPD0301 Displays
    -----------------------------------------------------------------------
    The driver is used in multiple displays (128x64, 128x32, etc.).
    Select the appropriate display below to create an appropriately
    sized framebuffer, etc.

    SPD0301_128_64  128x64 pixel display

    SPD0301_128_32  128x32 pixel display

    SPD0301_96_16

    -----------------------------------------------------------------------*/
   #define SPD0301_128_64
/*=========================================================================*/

#define SPD0301_LCDWIDTH                  128
#define SPD0301_LCDHEIGHT                 64

// Command set
#define SPD0301_SETCONTRAST 0x81
#define SPD0301_DISPLAYALLON_RESUME 0xA4
#define SPD0301_DISPLAYALLON 0xA5
#define SPD0301_NORMALDISPLAY 0xA6
#define SPD0301_INVERTDISPLAY 0xA7
#define SPD0301_DISPLAYOFF 0xAE
#define SPD0301_DISPLAYON 0xAF

#define SPD0301_SETDISPLAYOFFSET 0xD3
#define SPD0301_SETCOMPINS 0xDA

#define SPD0301_SETVCOMDETECT 0xDB

#define SPD0301_SETDISPLAYCLOCKDIV 0xD5
#define SPD0301_SETPRECHARGE 0xD9

#define SPD0301_SETMULTIPLEX 0xA8

#define SPD0301_SETLOWCOLUMN 0x00
#define SPD0301_SETHIGHCOLUMN 0x10

#define SPD0301_SETSTARTLINE 0x40

#define SPD0301_MEMORYMODE 0x20
#define SPD0301_COLUMNADDR 0x21
#define SPD0301_PAGEADDR   0x22

#define SPD0301_COMSCANINC 0xC0
#define SPD0301_COMSCANDEC 0xC8

#define SPD0301_SEGREMAP 0xA0

#define SPD0301_CHARGEPUMP 0x8D

#define SPD0301_EXTERNALVCC 0x1
#define SPD0301_SWITCHCAPVCC 0x2

class Adafruit_SPD0301 : public Adafruit_GFX {
 public:
  Adafruit_SPD0301(int8_t DC, int8_t RST, int8_t CS);

  void begin(uint8_t switchvcc = SPD0301_SWITCHCAPVCC, bool reset=true);
  void SPD0301_command(uint8_t c);
  void SPD0301_data(uint8_t c);

  void clearDisplay(void);
  void invertDisplay(uint8_t i);
  void flipVertical(bool flip);
  void display();

  int printf(const char *fmt, ...);
  void print(const char *s) override;
  
  void dim(uint8_t boolean);

  void drawPixel(int16_t x, int16_t y, uint16_t color);

  virtual void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
  virtual void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);

 private:
  int8_t _vccstate, sclk, dc, rst, cs;
  void fastSPIwrite(uint8_t c);

  inline void drawFastVLineInternal(int16_t x, int16_t y, int16_t h, uint16_t color) __attribute__((always_inline));
  inline void drawFastHLineInternal(int16_t x, int16_t y, int16_t w, uint16_t color) __attribute__((always_inline));

};
