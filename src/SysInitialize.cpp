#include "globalInstances.h"
#include "HdlcProtocol.h"
#include "sysPlatform/SysTypes.h"
#include "sysPlatform/SysCpuControl.h"
#include "sysPlatform/SysProgrammer.h"
#include "sysPlatform/SysLogger.h"
#include "sysPlatform/SysDebugPrint.h"
#include "sysPlatform/SysInitialize.h"
#include "sysPlatform/SysOTP.h"
#include "sysPlatform/SysAudio.h"
#include "sysPlatform/SysTimer.h"
#include "sysPlatform/SysIO.h"

#include <circle/gpiopin.h>

#define BOOT_PARTITION	"emmc1-1"

// Network configuration
#define USE_DHCP
#define USE_SCREEN

#ifndef USE_DHCP
static const u8 IPAddress[]      = {192, 168, 0, 250};
static const u8 NetMask[]        = {255, 255, 255, 0};
static const u8 DefaultGateway[] = {192, 168, 0, 1};
static const u8 DNSServer[]      = {192, 168, 0, 1};
#endif

// I2C Master config
#define I2C_MASTER_DEVICE	1		// 0 on Raspberry Pi 1 Rev. 1 boards, 1 otherwise
#define I2C_MASTER_CONFIG	0		// 0 or 1 on Raspberry Pi 4, 0 otherwise
#define I2C_FAST_MODE		FALSE		// standard mode (100 Kbps) or fast mode (400 Kbps)
#define I2C_SLAVE_ADDRESS	0x1A	// 7 bit slave address

// UARTS
//constexpr unsigned UART_BAUD_RATE = 2000000;  // unsure if this is totally reliable yet
constexpr unsigned UART_BAUD_RATE = 921600;

namespace SysPlatform {

//////////////////////////
// LIBCIRCLE SINGLETONS
CActLED             g_actLED;
CKernelOptions		g_options;
CDeviceNameService	g_deviceNameService;
CExceptionHandler   g_exceptionHandler;
CInterruptSystem    g_interrupt;
CScheduler          g_scheduler;
CTimer              g_timer(&g_interrupt);
CCPUThrottle        g_cpuThrottle;

CEMMCDevice	    	g_emmc(&g_interrupt, &g_timer, &g_actLED);
CFATFileSystem		g_fileSystem;
CUSBHCIDevice		g_usbHci(&g_interrupt, &g_timer);
#ifdef USE_DHCP
CNetSubSystem		g_net;
#else
CNetSubSystem		g_net(IPAddress, NetMask, DefaultGateway, DNSServer);
#endif

#ifdef USE_SCREEN
CScreenDevice  g_screen(g_options.GetWidth (), g_options.GetHeight ());
#endif
CLogger        g_logger(g_options.GetLogLevel (), &g_timer);
//////////////////////////////

CInterruptSystem*   g_interruptSysPtr      = nullptr;
CExceptionHandler*  g_exceptionHandlerPtr  = nullptr;
CScreenDevice*      g_screenPtr            = nullptr;  // TODO might not have a GET function
CLogger*            g_loggerPtr            = nullptr;
CActLED*            g_actLedPtr            = nullptr;
CDeviceNameService* g_deviceNameServicePtr = nullptr;
CKernelOptions*     g_optionsPtr           = nullptr;
CTimer*             g_timerPtr             = nullptr;
CScheduler*         g_schedulerPtr         = nullptr;
CCPUThrottle*       g_cpuThrottlePtr       = nullptr;
CI2CMaster*         g_i2cMasterPtr         = nullptr;

CEMMCDevice*    g_emmcPtr       = nullptr;
CFATFileSystem* g_fileSystemPtr = nullptr;
CUSBHCIDevice*  g_usbHciPtr     = nullptr;
CNetSubSystem*  g_netPtr        = nullptr;

CSerialDevice* g_debugPtr = nullptr;
CSerialDevice* g_hdlcPtr  = nullptr;
CSPIMaster*    g_spi0Ptr  = nullptr;

constexpr size_t HDLC_READ_BUFFER_SIZE = 2048;
std::mutex     g_hdlcMtx;
std::shared_ptr<Hdlcpp::Hdlcpp> g_hdlcppPtr = nullptr;
static int hdlcTransportRead(uint8_t *data, uint16_t length)
{
  if (!g_hdlcPtr) { return 0; }
  return g_hdlcPtr->Read(data, length);
}

static int hdlcTransportWrite(const uint8_t *data, uint16_t length)
{
  if (!g_hdlcPtr) { return 0; }
  g_hdlcPtr->Write(data, length);
  g_hdlcPtr->Flush();
  return length;
}

static void __attribute__((unused)) enable_cycle_counter_el0(void)
{
	uint64_t val;
	/* Disable cycle counter overflow interrupt */
	asm volatile("msr pmintenset_el1, %0" : : "r" ((uint64_t)(0 << 31)));
	/* Enable cycle counter */
	asm volatile("msr pmcntenset_el0, %0" :: "r" (1 << 31));
	/* Enable user-mode access to cycle counters. */
	asm volatile("msr pmuserenr_el0, %0" :: "r" ((1 << 0) | (1 << 2)));
	/* Clear cycle counter and start */
	asm volatile("mrs %0, pmcr_el0" : "=r" (val));
	val |= ((1 << 0) | (1 << 2));
	asm volatile("isb");
	asm volatile("msr pmcr_el0, %0" :: "r" (val));
	val = (1 << 27);
	asm volatile("msr pmccfiltr_el0, %0" :: "r" (val));
}

static bool isInitialized = false;

int  sysInitialize() {
	g_interruptSysPtr      = CInterruptSystem::Get();
	g_exceptionHandlerPtr  = CExceptionHandler::Get();
	g_screenPtr            = &g_screen;
	g_loggerPtr            = CLogger::Get();
	g_actLedPtr            = CActLED::Get();
	g_deviceNameServicePtr = CDeviceNameService::Get();
	g_optionsPtr           = CKernelOptions::Get();
	g_timerPtr             = CTimer::Get();
	g_schedulerPtr         = CScheduler::Get();
	g_cpuThrottlePtr       = CCPUThrottle::Get();

	g_emmcPtr       = &g_emmc;
	g_fileSystemPtr = &g_fileSystem;
	g_usbHciPtr     = &g_usbHci;
	g_netPtr        = CNetSubSystem::Get();

    if (!g_interruptSysPtr || !g_exceptionHandlerPtr || !g_loggerPtr ||
	    !g_actLedPtr || !g_deviceNameServicePtr || !g_optionsPtr || !g_timerPtr ||
		!g_schedulerPtr || !g_cpuThrottlePtr || !g_emmcPtr || !g_fileSystemPtr ||
		!g_usbHciPtr || !g_netPtr) { return SYS_FAILURE; }

	g_actLedPtr->Blink (5);	// show we are alive

	boolean bOK = TRUE;
	enable_cycle_counter_el0();

#ifdef USE_SCREEN
	if (!g_screenPtr) { bOK = false; }

	if (bOK)
	{
		bOK = g_screenPtr->Initialize ();
		if (!bOK) { g_loggerPtr->Write("sysInitialize()", TLogSeverity::LogError, "Screen init FAILED!\n"); }
	}

	if (bOK)
	{
		CDevice *pTarget = g_deviceNameServicePtr->GetDevice(g_optionsPtr->GetLogDevice (), FALSE);
		if ((pTarget == 0) && g_screenPtr) {
			pTarget = g_screenPtr;

			bOK = g_loggerPtr->Initialize (pTarget);
			if (!bOK) { g_loggerPtr->Write("sysInitialize()", TLogSeverity::LogError, "device name service FAILED!\n"); }
		}
	}
#endif

	if (bOK)
	{
		bOK = g_interruptSysPtr->Initialize ();
		if (!bOK) { g_loggerPtr->Write("sysInitialize()", TLogSeverity::LogError, "interrupt system FAILED!\n"); }
	}

	if (bOK) {
		std::lock_guard<std::mutex> lock(g_hdlcMtx);
		g_hdlcPtr  = new CSerialDevice(g_interruptSysPtr,  false, 0);  // GPIO 14/15 for TX/RX
		g_debugPtr = new CSerialDevice(g_interruptSysPtr, false, 3);  // GPIO 4/5 for TX/RX

		if (!g_debugPtr || !g_hdlcPtr) {
			bOK = false;
			g_loggerPtr->Write("sysInitialize()", TLogSeverity::LogError, "UART allocate FAILED!\n");
		}

		bOK = g_debugPtr->Initialize(UART_BAUD_RATE, 8, 1, CSerialDevice::ParityNone);
		if (!bOK) { g_loggerPtr->Write("sysInitialize()", TLogSeverity::LogError, "DEBUG serial init FAILED!\n"); }
		bOK = g_hdlcPtr->Initialize(UART_BAUD_RATE, 8, 1, CSerialDevice::ParityNone);
		g_hdlcPtr->SetOptions(0);  // Disable auto CR when LR sent
		if (!bOK) { g_loggerPtr->Write("sysInitialize()", TLogSeverity::LogError, "HDLC serial init FAILED!\n"); }

		// Initialize the HDLCpp
		g_hdlcppPtr = std::make_shared<Hdlcpp::Hdlcpp>(
		    hdlcTransportRead, hdlcTransportWrite, HDLC_READ_BUFFER_SIZE, 0 /* timeout in ms */, 0 /* retries */);
		}

	if (bOK)
	{
		bOK = g_timerPtr->Initialize ();
		if (!bOK) { g_loggerPtr->Write("sysInitialize()", TLogSeverity::LogError, "timer FAILED!\n"); }
	}

	if (bOK)
	{
		bOK = g_emmcPtr->Initialize ();
		if (!bOK) { g_loggerPtr->Write("sysInitialize()", TLogSeverity::LogError, "eMMC FAILED!\n"); }
	}

	if (bOK)
	{
		bOK = g_netPtr->Initialize ();
		if (!bOK) { g_loggerPtr->Write("sysInitialize()", TLogSeverity::LogError, "network FAILED!\n"); }
	}

    if (bOK)
	{
		// Mount file system
		CDevice *pPartition = g_deviceNameServicePtr->GetDevice (BOOT_PARTITION, TRUE);
		if (pPartition == 0)
		{
			g_loggerPtr->Write("sysInitialize()", TLogSeverity::LogPanic, "Partition not found: %s\n", BOOT_PARTITION);
			bOK = false;

		}

		if (!g_fileSystemPtr->Mount (pPartition))
		{
			g_loggerPtr->Write("sysInitialize()", TLogSeverity::LogPanic, "Cannot mount partition: %s\n", BOOT_PARTITION);
			bOK = false;
		}
	}

	if (bOK) {
		if (sysProgrammer.begin() != SYS_SUCCESS) {
			bOK = false;
			g_loggerPtr->Write("sysInitialize()", TLogSeverity::LogError, "programmer FAILED!\n");
		}
	}

	if (bOK) {
		g_i2cMasterPtr = new CI2CMaster(I2C_MASTER_DEVICE, I2C_FAST_MODE, I2C_MASTER_CONFIG);
		if (g_i2cMasterPtr) { bOK = g_i2cMasterPtr->Initialize(); }
		if (!bOK) { g_loggerPtr->Write("sysInitialize()", TLogSeverity::LogPanic, "Cannot create CI2CMaster\n");}
	}

	if (bOK) {
		g_spi0Ptr = new CSPIMaster(5000000, 0, 0, 0);
		if (g_spi0Ptr) { bOK = g_spi0Ptr->Initialize(); }
		if (!bOK) { g_loggerPtr->Write("sysInitialize()", TLogSeverity::LogPanic, "Cannot create CSPIMasterDMA\n");}
		else { SPI.setRawSpi(g_spi0Ptr); }
	}

	if (bOK) {
		if (SysOTP::begin() != SYS_SUCCESS) {
			bOK = false;
			g_loggerPtr->Write("sysInitialize()", TLogSeverity::LogPanic, "Cannot initialize SysOTP\n");
		}
	}

	if (bOK) {
		if (sysIO.begin() != SYS_SUCCESS) {
			bOK = false;
			g_loggerPtr->Write("sysInitialize()", TLogSeverity::LogPanic, "Cannot initialize SysIO)\n");
		}
	}

	if (bOK) {
		g_loggerPtr->Write("sysInitialize()", TLogSeverity::LogDebug, "initialization complete!\n");
        isInitialized = true;
        return SYS_SUCCESS;
    } else {
        isInitialized = false;
		g_loggerPtr->Write("sysInitialize()", TLogSeverity::LogError, "initialization FAILED!\n");
        return SYS_FAILURE;
    }

	SYS_DEBUG_PRINT(sysLogger.printf("*** SysPlatform::initialize() is complete, success: %d\n", bOK));
}

void sysInitShowSummary()
{
	const u8* ipPtr =  g_netPtr->GetConfig()->GetIPAddress()->Get();
	if (ipPtr) {
		SYS_DEBUG_PRINT(sysLogger.printf("SYS INIT SUMMARY:\n\r"));
		SYS_DEBUG_PRINT(sysLogger.begin(115200));
		char ipAddressStr[32+1];
		snprintf(ipAddressStr, 32, "IP Address: %u.%u.%u.%u", ipPtr[0], ipPtr[1], ipPtr[2], ipPtr[3]);
		SYS_DEBUG_PRINT(sysLogger.printf("%s\n\r", ipAddressStr));
	}
}

bool sysIsInitialized() { return isInitialized; }

void sysDeinitialize() {
#ifdef USE_SCREEN
    if (g_screenPtr) { delete g_screenPtr; }
#endif
    isInitialized = false;
    return;
}

}  // end namespace SysPlatform

static SysPlatform::SysCpuControl::ShutdownMode run() {
    using namespace SysPlatform;
	SYS_DEBUG_PRINT(sysLogger.printf("run() entered\n"));

    while(true)
	{
        //if (sysProgrammer.isXferInProgress() && sysCodec.isEnabled()) { sysCodec.disable(); }
		if (sysProgrammer.isXferInProgress()) {
			//if (sysCodec.isEnabled()) { SYS_DEBUG_PRINT(sysLogger.printf("XFER in progress...codec is ENABLED\n")); }
			//else { SYS_DEBUG_PRINT(sysLogger.printf("XFER in progress...codec is DISABLED...\n")); }
			//sysCodec.disable();
		}

        if (sysProgrammer.isNewProgrammingReceived()) {
			SYS_DEBUG_PRINT(sysLogger.printf("***New firmware received, REBOOTING!!!\n"));
			SysCpuControl::yield();  // must yield to give the TFTP thread a chance to close the connection
			SysTimer::delayMilliseconds(100);
			return SysCpuControl::ShutdownMode::REBOOT;
		}

        loop();
    }
}

int main (void)
{
    using namespace SysPlatform;

    setup();
	SysCpuControl::ShutdownMode mode = run();

	switch (mode)
	{
	case SysCpuControl::ShutdownMode::REBOOT : return SysCpuControl::reboot ();
	case SysCpuControl::ShutdownMode::HALT:
	default: return SysCpuControl::halt ();
	}
}
