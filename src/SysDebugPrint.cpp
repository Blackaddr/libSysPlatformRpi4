#include "SysDebugPrint.h"

namespace SysPlatform {

void sysEnableDebugPrint() { g_sysDebugPrintEnable = true; }
void sysDisableDebugPrint() { g_sysDebugPrintEnable = false; };

}
