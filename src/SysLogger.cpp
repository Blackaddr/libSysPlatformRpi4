#include "circle/serial.h"
#include "sysPlatform/SysTypes.h"
#include "sysPlatform/SysLogger.h"
#include "sysPlatform/SysInitialize.h"
#include "sysPlatform/SysCpuControl.h"
#include "globalInstances.h"

namespace SysPlatform {

SysLogger sysLogger;

SysLogger::SysLogger()
{

}

SysLogger::~SysLogger()
{

}

void SysLogger::begin()
{

}

void SysLogger::begin(unsigned baudRate)
{

}

void SysLogger::getSeverityStr(LogSeverity severity, char* str)
{

}

void SysLogger::log(const char *pMessage, ...)
{

}

void SysLogger::log(const char *pSource, LogSeverity severity, const char *pMessage, ...)
{

}

unsigned SysLogger::available() {
    return 0;
}

int SysLogger::read()
{
    return SYS_SUCCESS;
}

int SysLogger::printf(const char *fmt, ...)
{
    if (!g_loggerPtr) { return 0; }

	va_list var;
	va_start(var, fmt);
	CString msg;
	msg.FormatV(fmt, var);
	va_end(var);
	size_t bytesRemaining = msg.GetLength();
	const char* ptr = msg;
	return g_debugPtr->Write(ptr, bytesRemaining);
}

int SysLogger::vprintf(const char * format, va_list arg)
{
	CString msg;
	msg.FormatV(format, arg);
    size_t bytesRemaining = msg.GetLength();
	const char* ptr = msg;
	return g_debugPtr->Write(ptr, bytesRemaining);
}

SysLogger::operator bool() const
{
    return true;
}

void SysLogger::flush()
{
	if (g_debugPtr) { g_debugPtr->Flush(); }
}

}
