#include <cstdint>
#include <circle/startup.h>
#include "globalInstances.h"
#include "sysPlatform/SysTypes.h"
#include "sysPlatform/SysCpuControl.h"

namespace SysPlatform {


int  SysCpuControl::halt(void)
{
    ::halt();
	return EXIT_HALT;
}

int  SysCpuControl::reboot(void)
{
	::reboot();
	return EXIT_REBOOT;
}

void  SysCpuControl::yield(void) {
    g_schedulerPtr->Yield();
}

void SysCpuControl::disableIrqs()
{
	DisableIRQs();
}

void SysCpuControl::enableIrqs()
{
	EnableIRQs();
}

void SysCpuControl::AudioNoInterrupts()
{
    DisableIRQs();  // TODO
}

void SysCpuControl::AudioInterrupts()
{
    EnableIRQs(); // TODO
}

void SysCpuControl::AudioTriggerInterrupt()
{

}

void SysCpuControl::AudioSetInterruptPriority(int priority)
{

}

void SysCpuControl::AudioAttachInterruptVector(void (*function)(void))
{

}

void SysCpuControl::SysDataSyncBarrier()
{
	DataSyncBarrier();
}

void SysCpuControl::shutdownUsb()
{

}

}
