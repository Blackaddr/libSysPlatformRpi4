#include "sysPlatform/SysTypes.h"
#include "sysPlatform/SysCpuControl.h"
#include "sysPlatform/SysProgrammer.h"
#include "sysPlatform/SysLogger.h"
#include "sysPlatform/SysDebugPrint.h"
#include "sysPlatform/SysInitialize.h"

namespace SysPlatform {

static bool isInitialized = false;

int  sysInitialize() {
    isInitialized = true;
    return SYS_SUCCESS;
}

bool sysIsInitialized() { return isInitialized; }

void sysDeinitialize() {
    isInitialized = false;
    return;
}

}  // end namespace SysPlatform

static SysPlatform::SysCpuControl::ShutdownMode run() {
    using namespace SysPlatform;

    //auto& codec = hardwareControls.getCodec();

    while(true)
	{
        //if (sysProgrammer.isXferInProgress() && codec.isEnabled()) { codec.disable(); }

        if (sysProgrammer.isNewProgrammingReceived()) {
			SYS_DEBUG_PRINT(sysLogger.printf("***New firmware received, REBOOTING!!!"));
			SysCpuControl::yield();  // must yield to give the TFTP thread a chance to close the connection
			return SysCpuControl::ShutdownMode::REBOOT;
		}

        loop();
    }
}

int main (void)
{
    using namespace SysPlatform;

	if (!SysPlatform::sysInitialize()) {
        return SysCpuControl::halt();
    }
    setup();
	SysCpuControl::ShutdownMode mode = run();

	switch (mode)
	{
	case SysCpuControl::ShutdownMode::REBOOT : return SysCpuControl::reboot ();
	case SysCpuControl::ShutdownMode::HALT:
	default: return SysCpuControl::halt ();
	}
}
