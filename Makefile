# Library version.  Must use Semantic Versioning 'x.y.z'.  E.g. '1.5.2'
LIB_MAJOR_VER ?= 0
LIB_MINOR_VER ?= 0
LIB_PATCH_VER ?= 0
LIB_VER = $(LIB_MAJOR_VER).$(LIB_MINOR_VER).$(LIB_PATCH_VER)

# Name of the library (do not prefix with 'lib'), e.g. wscMyUtil
TARGET_NAME = sysPlatform

DOC_PATH_DOXYGEN =

# List of source files (do not include extentions.  E.g. 'MyClass', not 'MyClass.cpp')
#CPP_SRC_LIST = $(subst /$(TARGET_NAME)/,/, $(subst .cpp,,$(wildcard ./$(TARGET_NAME)/*.cpp)))
CPP_SRC_LIST += $(subst /$(TARGET_NAME)/,/, $(subst .cpp,,$(wildcard ./$(TARGET_NAME)/utility/*.cpp)))
CPP_SRC_LIST += \
    SysAudio \
    SysAudioUsb \
    SysCpuControl \
    SysCpuTelemetry \
    SysCrashReport \
    SysCrypto \
    SysDebugPrint \
    SysDisplay \
    SysInitialize \
    SysIO \
    SysLogger \
    SysMidi \
    SysMutex \
    SysNvStorage \
    SysOTP \
    SysProgrammer \
    SysSerial \
    SysSpi \
    SysThreads \
    SysTimer \
    SysTypes \
    SysWatchdog \
    AudioStream \
    SysCodecWM8731 \
    Adafruit_GFX \
    Adafruit_SPD0301 \
    ArduinoApi \
    SPI \
    HdlcProtocol


S_SRC_LIST =

# Do not include $(TARGET_NAME).h in API_HEADER LIST. This file is required and handled separately.
API_HEADER_LIST += \
    SysDebugPrint \
    SysTypes \
    SysPlatform \
    SysAudio \
    SysCpuControl \
    SysCpuTelemetry \
    SysCrashReport \
    SysCrypto \
    SysDebugPrint \
    SysDisplay \
    SysInitialize \
    SysIO \
    SysLogger \
    SysMidi \
    SysMutex \
    SysNvStorage \
    SysOTP \
    SysProgrammer \
    SysSerial \
    SysSpi \
    SysThreads \
    SysTimer \
    SysTwoWire \
    SysWatchdog \
    AudioSampleRate \
    AudioStream


# DMAChannel

# List of Git-repo dependencies
DEP_BUILD_LIST = \
    Blackaddr/rpicircle.git \
    Blackaddr/arm_math_aarch64.git

MAKEFILE_NAME ?= Makefile

SYS_INC_DIRS = -I./src/display

# Check if build engine location has already been defined
export LIB_BUILD_ENGINE_LOC ?= $(CURDIR)/scripts/avalon_lib_build_engine.$(MCU_TYPE)/
include $(LIB_BUILD_ENGINE_LOC)/Makefile.inc
