#include "sysPlatform/SysTypes.h"

namespace std {

float nanf(const char* arg) { return nanf(arg); }
float roundf(float val) { return static_cast<float>(round(val)); }
float clamp(float val, float minVal, float maxVal) {
    if (val < minVal) { return minVal; }
    if (val > maxVal) { return maxVal; }
    return val;
}

}
