#include "sysPlatform/SysTypes.h"
#include "sysPlatform/SysTimer.h"

namespace SysPlatform {

// milliseconds since program start
uint32_t SysTimer::millis()
{
    return 0;
}

// microseonds since program start
uint32_t SysTimer::micros()
{
    return 0;
}

// CPU cycle counts since program start
uint32_t SysTimer::cycleCnt32()
{
    return 0;
}

uint64_t SysTimer::cycleCnt64()
{
    return 0;
}

void SysTimer::delayMilliseconds(unsigned x)
{

}

void SysTimer::delayMicroseconds(unsigned x)
{

}

}
