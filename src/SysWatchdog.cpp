#include "sysPlatform/SysWatchdog.h"

#define INCLUDE_OUTPUT_WATCHDOG

// No watchdog in debug mode
#if defined(INCLUDE_OUTPUT_WATCHDOG) && (defined(DEBUG) || defined(JUCE_DEBUG))
#undef INCLUDE_OUTPUT_WATCHDOG
#endif

namespace SysPlatform {

SysWatchdog sysWatchdog;

#if defined(INCLUDE_OUTPUT_WATCHDOG)

SysWatchdog::SysWatchdog()
{
    reset();
}

SysWatchdog::~SysWatchdog()
{

}

void SysWatchdog::begin(float seconds)
{

}

bool SysWatchdog::isStarted()
{
    return m_isStarted;
}


void SysWatchdog::feed()
{

}

void SysWatchdog::longFeed()
{

}

void SysWatchdog::reset()
{

}

#else

SysWatchdog::SysWatchdog()
{

}

SysWatchdog::~SysWatchdog()
{

}

void SysWatchdog::begin(float seconds)
{

}


void SysWatchdog::feed()
{

}

void SysWatchdog::longFeed()
{

}

void SysWatchdog::reset()
{

}
#endif


}