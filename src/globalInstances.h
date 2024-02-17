#pragma once

#include "circle/interrupt.h"
#include "circle/screen.h"
#include "circle/logger.h"
#include "circle/actled.h"
#include "circle/devicenameservice.h"
#include "circle/koptions.h"
#include "circle/timer.h"
#include "circle/device.h"
#include "circle/exceptionhandler.h"
#include "circle/sched/scheduler.h"
#include "circle/cputhrottle.h"
#include "circle/i2cmaster.h"
#include "circle/serial.h"
#include <circle/spimaster.h>

#include "SDCard/emmc.h"  // for CEMMCDevice
#include "circle/fs/fat/fatfs.h"  // for CFATFileSystem
#include "circle/net/netsubsystem.h" // for CNetSubSystem
#include "circle/usb/usbhcidevice.h"

#include "SPI.h"

namespace SysPlatform {

extern CInterruptSystem*   g_interruptSysPtr;
extern CExceptionHandler*  g_exceptionHandlerPtr ;
extern CScreenDevice*      g_screenPtr;
extern CLogger*            g_loggerPtr;
extern CActLED*            g_actLedPtr;
extern CDeviceNameService* g_deviceNameServicePtr;
extern CKernelOptions*     g_optionsPtr;
extern CTimer*             g_timerPtr;
extern CScheduler*         g_schedulerPtr;
extern CCPUThrottle*       g_cpuThrottlePtr;
extern CI2CMaster*         g_i2cMasterPtr;

extern CEMMCDevice*    g_emmcPtr;
extern CFATFileSystem* g_fileSystemPtr;
extern CUSBHCIDevice*  g_usbHciPtr;
extern CNetSubSystem*  g_netPtr;

extern CSerialDevice* g_uart0Ptr;
extern CSerialDevice* g_uart1Ptr;
extern CSPIMaster*    g_spi0Ptr;

}
