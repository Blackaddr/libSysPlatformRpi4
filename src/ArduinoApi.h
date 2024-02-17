#pragma once

#include <cstdint>
#include <cstddef>
#include <cstring>
#include <string>
#include <circle/string.h>
#include "sysPlatform/SysTimer.h"

#ifndef INPUT
#define INPUT 0
#endif

#ifndef OUTPUT
#define OUTPUT 1
#endif

#ifndef LOW
#define LOW 0
#endif

#ifndef HIGH
#define HIGH 1
#endif

#ifndef PROGMEM
#define PROGMEM
#endif

typedef std::string String;

typedef uint8_t byte;
typedef bool boolean;

void pinMode(int pin, int direction);
void digitalWrite(int pin ,int value);

class Print {
public:

	virtual void print(const char* str) = 0;

};
