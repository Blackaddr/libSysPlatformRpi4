#include "SysTypes.h"
#include "SysLogger.h"
#include "SysDebugPrint.h"
#include "SysCrashReport.h"

namespace SysPlatform {

SysCrashReport sysCrashReport;
bool SysCrashReport::m_enabled = true;

size_t SysCrashReport::printReport() const
{
    if (!m_enabled) { return 0; }
    if (!isSysDebugPrintEnabled()) { return 0; }
}

void SysCrashReport::disable()
{
    reset();
    m_enabled = false;
}

void SysCrashReport::reset()
{
    clear();
}

void SysCrashReport::clear()
{

}

SysCrashReport::operator bool()
{
    if (!m_enabled) { return false; }
	return false;
}

void SysCrashReport::setBreadcrumb(unsigned num, uint32_t mask, uint32_t value)
{
    if (!m_enabled) { return; }
}

void SysCrashReport::breadcrumbRaw(unsigned int num, unsigned int value)
{
    if (!m_enabled) { return; }
}

uint32_t SysCrashReport::getBreadcrumb(unsigned num)
{

}

uint32_t SysCrashReport::checksum(volatile const void *data, int len) {
    if (!data) { return 0; }
    if (len < 1) { return 0; }
    return 0;
}

}
