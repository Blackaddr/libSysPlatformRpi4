#include "sysPlatform/SysTypes.h"
#include "sysPlatform/SysCpuTelemetry.h"

namespace SysPlatform {

unsigned SysCpuTelemetry::getCpuFreqHz()
{
    return 0;
}
float SysCpuTelemetry::getTemperatureCelcius()
{
    return 0.0f;
}

void   SysCpuTelemetry::markUnusedStack()
{

}

size_t SysCpuTelemetry::getStackSizeBytes()
{
    return 0;
}

size_t SysCpuTelemetry::getStackUsedBytes()
{
    return 0;
}

size_t SysCpuTelemetry::getStackFreeBytes()
{
    return 0;
}

float  SysCpuTelemetry::getStackUsageRatio()
{
    return 0;
}

size_t SysCpuTelemetry::getHeapSizeBytes()
{
    return 0;
}

size_t SysCpuTelemetry::getHeapUsedBytes()
{
    return 0;
}

size_t SysCpuTelemetry::getHeapFreeBytes()
{
    return 0;
}

float  SysCpuTelemetry::getHeapUsageRatio()
{
    return 0;
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
