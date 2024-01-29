#include "sysPlatform/SysMidi.h"

namespace SysPlatform {

SysMidiBase::SysMidiBase() = default;
SysMidiBase::~SysMidiBase() = default;

////////////////
// SERIAL MIDI
////////////////
#if defined(PROCESS_SERIAL_MIDI)
SysSerialMidi sysSerialMidi;

SysSerialMidi::SysSerialMidi()
{

}

SysSerialMidi::~SysSerialMidi()
{

}

void SysSerialMidi::init()
{

}

bool SysSerialMidi::read()
{
    return false;
}

MidiMessageType SysSerialMidi::getType()
{
    return MidiMessageType::InvalidType;
}

uint8_t* SysSerialMidi::getSysExArray()
{
    return nullptr;
}

size_t   SysSerialMidi::getSysExArrayLength()
{
    return 0;
}

uint8_t  SysSerialMidi::getData1()
{
    return 0;
}

uint8_t  SysSerialMidi::getData2()
{
    return 0;
}

void SysSerialMidi::sendSysEx(size_t sysExDataLength, uint8_t* sysExData)
{

}

void SysSerialMidi::sendProgramChange(unsigned program, unsigned channel)
{

}
#endif

/////////////
// USB MIDI
/////////////
#if defined(PROCESS_USB_MIDI)
SysUsbMidi sysUsbMidi;

SysUsbMidi::SysUsbMidi()
{

}

SysUsbMidi::~SysUsbMidi()
{

}

bool SysUsbMidi::read()
{
    return false;
}

MidiMessageType SysUsbMidi::getType()
{
    return MidiMessageType::NO_MESSAGE;
}

uint8_t* SysUsbMidi::getSysExArray()
{
    return nullptr;
}

size_t   SysUsbMidi::getSysExArrayLength()
{
    return 0;
}

uint8_t  SysUsbMidi::getData1()
{
    return 0;
}

uint8_t  SysUsbMidi::getData2()
{
    return 0;
}

void SysUsbMidi::sendSysEx(size_t sysExDataLength, uint8_t* sysExData)
{

}

void SysUsbMidi::sendProgramChange(unsigned program, unsigned channel)
{

}
#endif

}