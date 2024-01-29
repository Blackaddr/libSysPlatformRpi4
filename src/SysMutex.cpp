#include "circle/sched/mutex.h"
#include "sysPlatform/SysTypes.h"
#include "sysPlatform/SysMutex.h"

#if defined(SYSPLATFORM_STD_MUTEX)

namespace std {

mutex::mutex()
{
    mxPtr = new CMutex;
}

mutex::~mutex()
{
    if (mxPtr) { delete (CMutex*)mxPtr; }
}

void mutex::lock() { ((CMutex*)mxPtr)->Acquire(); }
//bool mutex<T>::try_lock() { return mx.try_lock(); }
void mutex::unlock() { ((CMutex*)mxPtr)->Release(); }

}

#endif
