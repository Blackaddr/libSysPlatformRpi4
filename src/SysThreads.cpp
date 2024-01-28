#include <vector>
#include <cmath>
#include "sysPlatform/SysTypes.h"
#include "sysPlatform/SysThreads.h"

namespace SysPlatform {

SysThreads sysThreads;

struct SysThreads::_impl {

};

SysThreads::SysThreads()
: m_pimpl(nullptr)
{

}

SysThreads::~SysThreads()
{

}

int SysThreads::addThread(SysThreadFunction p, void * arg, int stack_size, void *stack)
{
    return SYS_FAILURE;
}

int SysThreads::getState(int id)
{
    return SysThreads::NOT_STARTED;
}

int SysThreads::setState(int id, int state) {
    return SYS_FAILURE;
}

int SysThreads::suspend(int id)
{
    return SYS_FAILURE;
}

int SysThreads::restart(int id)
{
    return SYS_FAILURE;
}

int SysThreads::setTimeSlice(int id, unsigned ticks)
{
    return SYS_FAILURE;
}

int SysThreads::setTimeSliceUs(int id, unsigned microseconds)
{
    return SYS_FAILURE;
}

int SysThreads::id() {
    return SYS_FAILURE;
}

void SysThreads::yield()
{

}

void SysThreads::delay(int milliseconds)
{

}

}