#include "sysPlatform/SysTypes.h"
#include "sysPlatform/SysLogger.h"
#include "sysPlatform/SysInitialize.h"
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
    if (!sysIsInitialized() || !g_loggerPtr) { return 0; }

	va_list var;
	va_start(var, fmt);
	CString msg;
	msg.FormatV(fmt, var);
	va_end(var);
	g_loggerPtr->Write("", TLogSeverity::LogNotice, msg);
	return msg.GetLength();

    // int result = 0;
    // va_list args;

    // va_start(args, fmt);
    // //result = printf(fmt, args);
    // va_end(args);

    // return result;
}

SysLogger::operator bool() const
{
    return true;
}

void SysLogger::flush()
{

}

}
