#include <cstdarg>
#include <vector>
#include "circle/gpiopin.h"
#include "ArduinoApi.h"

constexpr int csPinNum  = 8;
constexpr int dcPinNum  = 23;
constexpr int rstPinNum = 24;
#ifdef SW_CS_PIN
static CGPIOPin csPin;
#endif
static CGPIOPin dcPin;
static CGPIOPin rstPin;

void pinMode(int pin, int mode) {
    switch(pin) {
        #ifdef SW_CS_PIN
    case csPinNum :
        csPin.AssignPin(csPinNum);
        csPin.SetMode(GPIOModeOutput);
        break;
        #endif
    case dcPinNum :
        dcPin.AssignPin(dcPinNum);
        dcPin.SetMode(GPIOModeOutput);
        break;
    case rstPinNum :
        rstPin.AssignPin(rstPinNum);
        rstPin.SetMode(GPIOModeOutput);
        break;
    }
}

void digitalWrite(int pin, int value) {
    switch(pin) {
        #ifdef SW_CS_PIN
    case csPinNum :
        csPin.Write(value ? HIGH : LOW);
        break;
        #endif
    case dcPinNum :
        dcPin.Write(value ? HIGH : LOW);
        break;
    case rstPinNum :
        rstPin.Write(value ? HIGH : LOW);
        break;
    }
}