#include "globalInstances.h"
#include "Hdlcpp.hpp"
#include "HdlcProtocol.h"
#include "sysPlatform/SysTypes.h"
#include "sysPlatform/SysLogger.h"
#include "sysPlatform/SysMutex.h"
#include "sysPlatform/SysMidi.h"

namespace SysPlatform {

SysMidiBase::SysMidiBase() = default;
SysMidiBase::~SysMidiBase() = default;

////////////////
// SERIAL MIDI
////////////////
#if defined(PROCESS_SERIAL_MIDI)
SysSerialMidi sysSerialMidi;

struct SysSerialMidi::_impl {

};

SysSerialMidi::SysSerialMidi()
: m_pimpl(nullptr)
{

}

SysSerialMidi::~SysSerialMidi()
{

}

void SysSerialMidi::init()
{
    m_isInitialized = true;
}

bool SysSerialMidi::read()
{
    return false;
}

bool SysSerialMidi::putMsg(uint8_t msg[4]) { return false; }

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

uint8_t  SysSerialMidi::getChannel()
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

void SysSerialMidi::sendMidiMessage(uint8_t type, uint8_t channel, uint8_t data1, uint8_t data2)
{

}
#endif

/////////////
// USB MIDI
/////////////
#if defined(PROCESS_USB_MIDI)
SysUsbMidi sysUsbMidi;

struct SysUsbMidi::_impl {
    static constexpr size_t POOL_SIZE = 49;
    UsbMidiMessage msgPool[POOL_SIZE];
    UsbMidiMessage currentMsg;
    size_t poolWriteIndex = 0;
    size_t poolReadIndex = 0;

    bool sysexInProgress = false;
    bool sysExReady = false;
    static constexpr size_t SYSEX_BUFFER_SIZE = 2048U;
    size_t sysexWriteIndex = 0;
    size_t sysexReadIndex  = 0;
    size_t sysexMessageLength = 0;
    uint8_t sysexBuffer[SYSEX_BUFFER_SIZE];
    std::mutex mtx;

    bool isMsgEmpty() { return (poolWriteIndex == poolReadIndex); }
    bool isMsgFull() { return ((poolWriteIndex+1 % POOL_SIZE) == poolReadIndex); }

    bool isSysexEmpty() { return (sysexWriteIndex == sysexReadIndex); }
    bool isSysexFull() { return ((sysexWriteIndex+1 % SYSEX_BUFFER_SIZE) == sysexReadIndex); }
};

SysUsbMidi::SysUsbMidi()
: m_pimpl(std::make_unique<_impl>())
{
    init();
}

SysUsbMidi::~SysUsbMidi()
{

}

void SysUsbMidi::init()
{
    m_pimpl->poolWriteIndex  = 0;
    m_pimpl->poolReadIndex   = 0;
    m_pimpl->sysexWriteIndex = 0;
    m_pimpl->sysexReadIndex  = 0;

    m_isInitialized = true;
}

bool SysUsbMidi::read()
{
    if (!m_isInitialized) { init(); }

    std::lock_guard<std::mutex> lock(m_pimpl->mtx);
    bool sysexEmpty = m_pimpl->isSysexEmpty();
    bool msgEmpty   = m_pimpl->isMsgEmpty();


    if (!sysexEmpty && m_pimpl->sysExReady) {
        m_pimpl->sysExReady = false;
        return true;
    }

    if (!msgEmpty) {  // normal message in the pool
        {
            size_t poolReadIndex = (volatile size_t)m_pimpl->poolReadIndex;
            m_pimpl->currentMsg = m_pimpl->msgPool[poolReadIndex];
            m_pimpl->poolReadIndex = (poolReadIndex+1) % m_pimpl->POOL_SIZE;
        }
        return true;
    }
    return false;
}

bool SysUsbMidi::putMsg(uint8_t msg[4]) {
    if (!m_isInitialized) { init(); }

    // assume sysex start always on the first byte for simplicity
    constexpr uint8_t SYSEX_START = static_cast<unsigned>(MidiMessageType::SystemExclusive);
    constexpr uint8_t SYSEX_END   = static_cast<unsigned>(MidiMessageType::SystemExclusiveEnd);
    constexpr uint8_t MIDI_COMMAND_MASK = 0x80U;

    uint8_t type = msg[1];  // assume all sysex start as a new USB packet

    std::lock_guard<std::mutex> lock(m_pimpl->mtx);

    // SYSEX Processing
    if (type == SYSEX_START && m_pimpl->isSysexFull()) { return false; }  // Sysex buffer is full
    if (type == SYSEX_START) {  // sysex and not full
        m_pimpl->sysexInProgress = true;
    } else if ( m_pimpl->sysexInProgress && ((msg[1] & MIDI_COMMAND_MASK) || (msg[2] & MIDI_COMMAND_MASK) || (msg[3] & MIDI_COMMAND_MASK)) ) {
        // one of the bytes is a midi control byte (MSB is set) but it is not a Sysex Start
        // check if it's also not a Sysex end. If so, we must abort any sysex in progress as the
        // command is something else, like a program change, CC, etc.
        if ((msg[1] != SYSEX_END) && (msg[2] != SYSEX_END) && (msg[3] != SYSEX_END) ) {
            m_pimpl->sysexInProgress = false;  // disable the sysex mode
            m_pimpl->sysExReady = false;
            SYS_DEBUG_PRINT(sysLogger.printf("SysUsbMidi::putMsg(): sysex aborted\n"));
        }
    }

    if (m_pimpl->sysexInProgress) {
        // process sysex message
        if (!m_pimpl->isSysexFull()) {
            m_pimpl->sysexBuffer[m_pimpl->sysexWriteIndex++] = msg[1];
            if (m_pimpl->sysexWriteIndex >= m_pimpl->SYSEX_BUFFER_SIZE) { m_pimpl->sysexWriteIndex = 0; }
            m_pimpl->sysexMessageLength++;
            if (msg[1] == SYSEX_END) { m_pimpl->sysexInProgress = false; m_pimpl->sysExReady = true; return true; } // skip the rest of the message
        }

        if (!m_pimpl->isSysexFull()) {
            m_pimpl->sysexBuffer[m_pimpl->sysexWriteIndex++] = msg[2];
            if (m_pimpl->sysexWriteIndex >= m_pimpl->SYSEX_BUFFER_SIZE) { m_pimpl->sysexWriteIndex = 0; }
            m_pimpl->sysexMessageLength++;
            if (msg[2] == SYSEX_END) { m_pimpl->sysexInProgress = false; m_pimpl->sysExReady = true; return true; } // skip the rest of the message
        }

        if (!m_pimpl->isSysexFull()) {
            m_pimpl->sysexBuffer[m_pimpl->sysexWriteIndex++] = msg[3];
            if (m_pimpl->sysexWriteIndex >= m_pimpl->SYSEX_BUFFER_SIZE) { m_pimpl->sysexWriteIndex = 0; }
            m_pimpl->sysexMessageLength++;
            if (msg[1] == SYSEX_END) { m_pimpl->sysexInProgress = false; m_pimpl->sysExReady = true; return true; } // skip the rest of the message
        }

        return true;
    }

    // MIDI MSG processing
    if (!m_pimpl->isMsgFull()) {
        // regular 3 bytes midi message
        size_t poolWriteIndex = (volatile size_t)m_pimpl->poolWriteIndex;
        m_pimpl->msgPool[poolWriteIndex].code_index_number = 0;
        m_pimpl->msgPool[poolWriteIndex].cable_number = msg[0];
        m_pimpl->msgPool[poolWriteIndex].MIDI_0 = msg[1];
        m_pimpl->msgPool[poolWriteIndex].MIDI_1 = msg[2];
        m_pimpl->msgPool[poolWriteIndex].MIDI_2 = msg[3];
        m_pimpl->poolWriteIndex = (poolWriteIndex+1) % m_pimpl->POOL_SIZE;
        return true;
    }
    return false;
}

MidiMessageType SysUsbMidi::getType()
{
    std::lock_guard<std::mutex> lock(m_pimpl->mtx);
    if (!m_pimpl->isSysexEmpty()) { return MidiMessageType::SystemExclusive; }

    // otherwise return last msg type
    MidiMessageType msgType = static_cast<MidiMessageType>(m_pimpl->currentMsg.MIDI_0 & 0xF0U);  // mask off the lower bits
    switch(msgType) {
    case MidiMessageType::ControlChange :
    case MidiMessageType::ProgramChange :
    case MidiMessageType::SystemExclusive :
    case MidiMessageType::SystemExclusiveEnd :
        return msgType;
    default :
        return MidiMessageType::InvalidType;
    }

}

uint8_t* SysUsbMidi::getSysExArray()
{
    return &m_pimpl->sysexBuffer[m_pimpl->sysexReadIndex];
}

size_t   SysUsbMidi::getSysExArrayLength()
{
    size_t length = m_pimpl->sysexMessageLength;
    m_pimpl->sysexReadIndex += m_pimpl->sysexMessageLength;
    if (m_pimpl->sysexReadIndex >= m_pimpl->SYSEX_BUFFER_SIZE) { m_pimpl->sysexReadIndex = m_pimpl->sysexReadIndex % m_pimpl->SYSEX_BUFFER_SIZE; }
    m_pimpl->sysexMessageLength = 0;
    return length;
}

uint8_t  SysUsbMidi::getChannel()
{
    return (m_pimpl->currentMsg.MIDI_0 & 0x0FU);  // channel is lower 4 bits
}

uint8_t  SysUsbMidi::getData1()
{
    return m_pimpl->currentMsg.MIDI_1;
}

uint8_t  SysUsbMidi::getData2()
{
    return m_pimpl->currentMsg.MIDI_2;
}

void SysUsbMidi::sendSysEx(size_t sysExDataLength, uint8_t* sysExData)
{
#if 0 // doesn't work yet
    if (!sysExData || (sysExDataLength < 1)) { return; }

    static bool msgSent = false;
    if (msgSent) return;

    // The sysex data requires two more bytes for the start and end bytes.
    uint8_t buffer[sysExDataLength+2];
    buffer[0] = (uint8_t)MidiMessageType::SystemExclusive;
    memcpy((void*)&buffer[1], sysExData, sysExDataLength);
    buffer[sysExDataLength+1] = (uint8_t)MidiMessageType::SystemExclusiveEnd;
    sysExDataLength += 2;  // the buffer is now 2 bytes longer

    // sysLogger.printf("sendSysEx: ");
    // for (size_t i=0; i < sysExDataLength; i++) {
    //     sysLogger.printf("%02X ", buffer[i]);
    // }
    // sysLogger.printf("\n");

    size_t idx=0;
    while(idx < sysExDataLength) {
        uint8_t data[3] = {0,0,0};  // each message must have full three bytes
        if (idx < sysExDataLength) data[0] = buffer[idx++];
        if (idx < sysExDataLength) data[1] = buffer[idx++];
        if (idx < sysExDataLength) data[2] = buffer[idx++];
        HdlcMidiMsgPktRaw pkt = makeMidiMsgPktRaw(0, data[0], data[1], data[2]);
        //sysLogger.printf("--SysExSend: %02X %02X %02X\n", data[0], data[1], data[2]);

        {
            std::lock_guard<std::mutex> lock(g_hdlcMtx);
            g_hdlcppPtr->write((uint8_t*)&pkt, sizeof(pkt));
            msgSent = true;
            sysLogger.printf("HDLC SENT: ");
            uint8_t* pktPtr = (uint8_t*)&pkt;
            for (size_t i=0; i<sizeof(pkt); i++) {
                sysLogger.printf("%02X ", pktPtr[i]);
            }
            sysLogger.printf("\n");
            }
    }
#endif
}

void SysUsbMidi::sendProgramChange(unsigned program, unsigned channel)
{

}

void SysUsbMidi::sendMidiMessage(uint8_t type, uint8_t channel, uint8_t data1, uint8_t data2)
{

}
#endif

}