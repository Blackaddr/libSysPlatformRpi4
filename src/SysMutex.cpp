#include "circle/sched/mutex.h"
#include "sysPlatform/SysTypes.h"
#include "sysPlatform/SysMutex.h"

#if defined(SYSPLATFORM_STD_MUTEX)

namespace std {

mutex::mutex()
{
    mxPtr = new CMutex;
    m_locked = false;
}

mutex::~mutex()
{
    if (mxPtr) { delete (CMutex*)mxPtr; }
}

void mutex::lock() {
    ((CMutex*)mxPtr)->Acquire();
    m_locked = true;
}
//bool mutex<T>::try_lock() { return mx.try_lock(); }
void mutex::unlock() {
    m_locked = false;
    ((CMutex*)mxPtr)->Release();
}

// TODO: Circle library doesn't support try_lock. Maybe we can try std::atmomic?
bool mutex::try_lock() {
    if (m_locked) { return false; }
    lock();
    return true;
}

}

#endif
