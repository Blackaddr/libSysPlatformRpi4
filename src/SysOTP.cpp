#include "globalInstances.h"
#include "sysPlatform/SysTypes.h"
#include "sysPlatform/SysLogger.h"
#include "sysPlatform/SysOTP.h"

namespace SysPlatform {

#define BOOT_PARTITION	"emmc1-1"
#define FUSES_FILENAME "fuses.dat"

struct fuses_t {
    uint32_t HW_OCOTP_GP1;
    uint32_t HW_OCOTP_GP2;
    uint32_t HW_OCOTP_CFG0;
    uint32_t HW_OCOTP_CFG1;
    uint32_t HW_OCOTP_MAC0;
    uint32_t HW_OCOTP_MAC1;
    uint32_t HW_OCOTP_SW_GP1;
    uint32_t HW_OCOTP_0x6B0;
    uint32_t HW_OCOTP_0x6C0;
    uint32_t HW_OCOTP_0x690;
    uint32_t HW_OCOTP_0x6A0;
    uint32_t HW_OCOTP_LOCK;
};

static int writeFuses();
static int readFuses();
// fuses_t g_fuses = {0x74F9365FU, 0x3049BEE1U, 0x65F82969U, 0x300351D2U, 0xE50CF490U, 0x000004E9U,
//                    0x00000202U, 0xD322019DU, 0x97EB491BU, 0x129E599FU, 0xBF7DDC17U, 0x4073BD07U
// };
static fuses_t g_fuses;

int SysOTP::begin()
{
	return readFuses();
}

uint64_t SysOTP::getUidLower()
{
    return ((uint64_t)g_fuses.HW_OCOTP_CFG1 << 32) + (uint64_t)g_fuses.HW_OCOTP_CFG0;
}

uint64_t SysOTP::getUidUpper()
{
    return ((uint64_t)g_fuses.HW_OCOTP_MAC1 << 32) + (uint64_t)g_fuses.HW_OCOTP_MAC0;
}

uint32_t SysOTP::getDevicePBKHLower()
{
    return g_fuses.HW_OCOTP_GP1;
}

uint32_t SysOTP::getDevicePBKHUpper()
{
    return g_fuses.HW_OCOTP_GP2;
}

uint32_t SysOTP::getDevelPBKHLower()
{
    return g_fuses.HW_OCOTP_0x690;
}
uint32_t SysOTP::getDevelPBKHUpper()
{
    return g_fuses.HW_OCOTP_0x6A0;
}

uint32_t SysOTP::getEUIDHLower()
{
    return g_fuses.HW_OCOTP_0x6B0;
}
uint32_t SysOTP::getEUIDHUpper()
{
    return g_fuses.HW_OCOTP_0x6C0;
}

uint16_t SysOTP::getProductId()
{
    return (g_fuses.HW_OCOTP_SW_GP1) & 0xFFFFU;
}

uint32_t SysOTP::getLocks()
{
    return g_fuses.HW_OCOTP_LOCK;
}

int writeFuses()
{
	constexpr char FUNC_NAME[] = "writeFuses()";

	// Create the fuse file
	// Create file and write to it
	unsigned hFile = g_fileSystemPtr->FileCreate (FUSES_FILENAME);
	if (hFile == 0)
	{
		g_loggerPtr->Write (FUNC_NAME, TLogSeverity::LogDebug, "Cannot create file: %s", FUSES_FILENAME);
		return SYS_FAILURE;
	}

	if (g_fileSystemPtr->FileWrite (hFile, &g_fuses, sizeof(fuses_t)) != sizeof(fuses_t))
	{
		g_loggerPtr->Write (FUNC_NAME, TLogSeverity::LogDebug, "Write error");
		return SYS_FAILURE;
	}

	if (!g_fileSystemPtr->FileClose (hFile))
	{
		g_loggerPtr->Write (FUNC_NAME, TLogSeverity::LogDebug, "Cannot close file");
		return SYS_FAILURE;
	}
	return SYS_SUCCESS;
}

int readFuses()
{
    constexpr char FUNC_NAME[] = "readFuses()";

	// open the file and read into g_fuses
	unsigned hFile = g_fileSystemPtr->FileOpen (FUSES_FILENAME);
	if (hFile == 0)
	{
		g_loggerPtr->Write (FUNC_NAME, TLogSeverity::LogDebug, "Cannot open file: %s", FUSES_FILENAME);
		return SYS_FAILURE;
	}

	if (g_fileSystemPtr->FileRead (hFile, &g_fuses, sizeof(fuses_t)) != sizeof(fuses_t)) {
		g_loggerPtr->Write (FUNC_NAME, TLogSeverity::LogDebug, "Failure reading file: %s", FUSES_FILENAME);
		return SYS_FAILURE;
	}

	if (!g_fileSystemPtr->FileClose (hFile))
	{
		g_loggerPtr->Write (FUNC_NAME, TLogSeverity::LogDebug, "Cannot close file");
		return SYS_FAILURE;
	}

#if 0
    sysLogger.printf("*** FUSES ***\n");
	sysLogger.printf("HW_OCOTP_GP1:    %08X\n", g_fuses.HW_OCOTP_GP1);
	sysLogger.printf("HW_OCOTP_GP2:    %08X\n", g_fuses.HW_OCOTP_GP2);
	sysLogger.printf("HW_OCOTP_CFG0:   %08X\n", g_fuses.HW_OCOTP_CFG0);
	sysLogger.printf("HW_OCOTP_CFG1:   %08X\n", g_fuses.HW_OCOTP_CFG1);
	sysLogger.printf("HW_OCOTP_MAC0:   %08X\n", g_fuses.HW_OCOTP_MAC0);
	sysLogger.printf("HW_OCOTP_MAC1:   %08X\n", g_fuses.HW_OCOTP_MAC1);
	sysLogger.printf("HW_OCOTP_SW_GP1: %08X\n", g_fuses.HW_OCOTP_SW_GP1);
	sysLogger.printf("HW_OCOTP_0x6B0:  %08X\n", g_fuses.HW_OCOTP_0x6B0);
	sysLogger.printf("HW_OCOTP_0x6C0:  %08X\n", g_fuses.HW_OCOTP_0x6C0);
	sysLogger.printf("HW_OCOTP_0x690:  %08X\n", g_fuses.HW_OCOTP_0x690);
	sysLogger.printf("HW_OCOTP_LOCK:   %08X\n", g_fuses.HW_OCOTP_LOCK);
#endif

    return SYS_SUCCESS;
}

}
