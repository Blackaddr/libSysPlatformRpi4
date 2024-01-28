#include "sysPlatform/SysTypes.h"
#include "sysPlatform/SysMutex.h"

#if defined(SYSPLATFORM_STD_MUTEX)

namespace std {

mutex::mutex() {}
mutex::~mutex() {}
void mutex::lock() {}
void mutex::unlock() {}

}

#endif
