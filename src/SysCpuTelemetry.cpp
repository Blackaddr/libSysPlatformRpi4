#include <cstdint>
#include "circle/cputhrottle.h"
#include "circle/memory.h"
#include "sysPlatform/SysTypes.h"
#include "sysPlatform/SysCpuTelemetry.h"

namespace SysPlatform {

unsigned SysCpuTelemetry::getCpuFreqHz()
{
	CCPUThrottle* ptr = CCPUThrottle::Get();
	if (ptr) {
		return (unsigned)(ptr->GetClockRate());
    } else {
        return 0;
    }
}
float SysCpuTelemetry::getTemperatureCelcius()
{
	CCPUThrottle* ptr = CCPUThrottle::Get();
	if (ptr) { return ptr->GetTemperature(); }
	return 0.0f;
}

void   SysCpuTelemetry::markUnusedStack()
{
	// the stack starts at MEM_KERNEL_STACK and grows down
	//uint32_t STACK_START = MEM_KERNEL_STACK;
	uintptr_t STACK_END   = MEM_KERNEL_STACK - KERNEL_STACK_SIZE;
	// Guess of where it is safe to fill memory... Maybe address of last variable (STACK_END) we have defined minus some slop...
	for (uint32_t *pfill = (uint32_t*)(STACK_END) + (uint32_t)32; pfill < (uint32_t*)((uintptr_t)&STACK_END - 16); pfill++) {
		*pfill = 0x01020304;  // some random value
	}
}

size_t SysCpuTelemetry::getStackSizeBytes()
{
	return (size_t)KERNEL_STACK_SIZE;
}

size_t SysCpuTelemetry::getStackUsedBytes()
{
	uintptr_t STACK_START = MEM_KERNEL_STACK;
	uintptr_t STACK_END   = MEM_KERNEL_STACK - KERNEL_STACK_SIZE;
	uint32_t *pmem = (uint32_t*)(STACK_END)+ 32;
	while (*pmem == 0x01020304) pmem++;
	return ((uintptr_t)STACK_START - (uintptr_t)pmem);
}

size_t SysCpuTelemetry::getStackFreeBytes()
{
	return getStackSizeBytes() - getStackUsedBytes();
}

float  SysCpuTelemetry::getStackUsageRatio()
{
	return (float)getStackUsedBytes() / (float)getStackSizeBytes();
}

size_t SysCpuTelemetry::getHeapSizeBytes()
{
	CMemorySystem* ptr = CMemorySystem::Get();
	if (!ptr) { return 0; }

	return ptr->GetMemSize();
}

size_t SysCpuTelemetry::getHeapUsedBytes()
{
	return getHeapSizeBytes() - getHeapFreeBytes();
}

size_t SysCpuTelemetry::getHeapFreeBytes()
{
	CMemorySystem* ptr = CMemorySystem::Get();
	if (!ptr) { return 0; }

	return ptr->GetHeapFreeSpace(HEAP_ANY);
}

float  SysCpuTelemetry::getHeapUsageRatio()
{
	return (float)getHeapUsedBytes() / (float)getHeapSizeBytes();
}

size_t SysCpuTelemetry::getRam0Size()
{
    return 0;
}

float  SysCpuTelemetry::getRam0UsageRatio()
{
    return 0.0f;
}

size_t SysCpuTelemetry::getRam1Size()
{
    return 0;
}

float  SysCpuTelemetry::getRam1UsageRatio()
{
    return 0.0f;
}

void SysCpuTelemetry::debugShowMemoryConfig()
{
    
}

}
