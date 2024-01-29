#include "globalInstances.h"
#include "tftpfileserver/tftpfileserver.h"

#include "sysPlatform/SysTypes.h"
#include "sysPlatform/SysProgrammer.h"

namespace SysPlatform {

SysProgrammer sysProgrammer;

static volatile CTFTPFileServer* tftpServerPtr = nullptr;

SysProgrammer::SysProgrammer()
{

}

SysProgrammer::~SysProgrammer()
{
    if (tftpServerPtr) delete tftpServerPtr;
}

int SysProgrammer::begin()
{
    if (!g_netPtr || !g_fileSystemPtr) { return SYS_FAILURE; }
    tftpServerPtr = new CTFTPFileServer(g_netPtr, g_fileSystemPtr);
    return SYS_SUCCESS;
}

bool SysProgrammer::isXferInProgress()
{
    if (tftpServerPtr) { return ((CTFTPFileServer*)tftpServerPtr)->isXferInProgress(); }
	else {
		g_loggerPtr->Write("SysProgrammer::isXferInProgress", TLogSeverity::LogError, "TFTP pointer is invalid\n");
		return false;
	}
}

bool SysProgrammer::isNewProgrammingReceived()
{
    if (tftpServerPtr) { return ((CTFTPFileServer*)tftpServerPtr)->isKernelImageReceived(); }
	else { return false; }
}

}
