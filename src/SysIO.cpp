#include <memory>
#include "globalInstances.h"
#include "sysPlatform/SysLogger.h"
#include "sysPlatform/SysTypes.h"
#include "sysPlatform/SysMidi.h"
#include "SysIOMapping.h"
#include "SysIO.h"

#include "Hdlcpp.hpp"
#include "HdlcProtocol.h"

namespace SysPlatform {

const int   POT_CALIB_MIN      = 50;
const int   POT_CALIB_MAX      = 3085;
const bool  POT_SWAP_DIRECTION = true;
const float POT_THRESHOLD      = 0.01f;

const int   EXP_CALIB_MIN      = 1670;
const int   EXP_CALIB_MAX      = 3085;
const bool  EXP_SWAP_DIRECTION = false;
const float EXP_THRESHOLD      = 0.05f;

// TODO protect this with a lock guard or atomic
volatile static unsigned g_switches                           = 0;
volatile static int      g_encoderValues[AVALON_NUM_ENCODERS] = {0,0,0,0};
volatile static int      g_potValues[AVALON_NUM_POTS]         = {0,0,0,0};


//////////
// SysIO
//////////
SysIO sysIO;

constexpr size_t HDLC_READ_BUFFER_SIZE = 2048;

struct SysIO::_impl {
    uint8_t* readBuffer = nullptr;

    void m_processPhysicalControl(HdlcPhysicalControlPkt& pkt);
    void m_processSwitch(HdlcPhysicalControlPkt& pkt);
    void m_processEncoder(HdlcPhysicalControlPkt& pkt);
    void m_processAnalog(HdlcPhysicalControlPkt& pkt);
    void m_processMidiMsg(HdlcMidiMsgPkt& pkt);
};

// Processing incomding Physical Control HDLC packet
void SysIO::_impl::m_processPhysicalControl(HdlcPhysicalControlPkt& pkt)
{
    switch (pkt.controlType) {
    case HdlcPhysicalControlType::SWITCH     : return m_processSwitch(pkt);
    case HdlcPhysicalControlType::ENCODER    : return m_processEncoder(pkt);
    case HdlcPhysicalControlType::POT        :
    case HdlcPhysicalControlType::EXPRESSION : return m_processAnalog(pkt);
    default :
        sysLogger.printf("SysIO::_impl::m_processPhysicalControl(): invalid control type:%u\n", pkt.controlType);
        break;
    }
}

// Processing incoming switch events
void SysIO::_impl::m_processSwitch(HdlcPhysicalControlPkt& rxPkt)
{
    switch (rxPkt.controlAction) {
    case HdlcPhysicalControlActions::SW_PUSH_TRIGGER :
    {
        g_switches |= 0x1U << rxPkt.controlId;
        break;
    }
    case HdlcPhysicalControlActions::SW_RELEASE_TRIGGER :
    {
        g_switches &= ~(0x1U << rxPkt.controlId);
        break;
    }
    default :
        sysLogger.printf("m_processSwitch(): invalid action type:%u\n", rxPkt.controlAction);
        break;
    }
}

// Processing incoming encoder events
void SysIO::_impl::m_processEncoder(HdlcPhysicalControlPkt& rxPkt)
{
    switch (rxPkt.controlAction) {
    case HdlcPhysicalControlActions::ENCODER_VALUE :
    {
        if (rxPkt.controlId >= AVALON_NUM_ENCODERS) { return; }
        g_encoderValues[rxPkt.controlId] = rxPkt.controlValue;
        break;
    }
    default :
        sysLogger.printf("m_processEncoder(): invalid action type:%u\n", rxPkt.controlAction);
        break;
    }
}

// Processing incoming analog events
void SysIO::_impl::m_processAnalog(HdlcPhysicalControlPkt& rxPkt)
{
    switch (rxPkt.controlAction) {
    case HdlcPhysicalControlActions::ANALOG_VALUE :
    {
        if (rxPkt.controlId >= AVALON_NUM_POTS) { return; }
        g_potValues[rxPkt.controlId] = rxPkt.controlValue;
        break;
    }
    default :
        sysLogger.printf("m_processAnalog(): invalid action type:%u\n", rxPkt.controlAction);
        break;
    }
}

void SysIO::_impl::m_processMidiMsg(HdlcMidiMsgPkt& pkt)
{
#if defined(PROCESS_USB_MIDI)
    //sysLogger.printf("SysIO::_impl::m_processMidiMsg(): channel:%02X  data0:%02X  data1:%u  data2:%02X\n",
    //    pkt.channel, pkt.data0, pkt.data1, pkt.data2);
    uint8_t msg[4];
    msg[0] = 0; // cable
    msg[1] = pkt.data0;
    msg[2] = pkt.data1;
    msg[3] = pkt.data2;
    sysUsbMidi.putMsg(msg);
#endif
}

SysIO::SysIO()
: m_pimpl(std::make_unique<_impl>())
{

}

SysIO::~SysIO()
{
    if (m_pimpl->readBuffer) { free (m_pimpl->readBuffer); }
}

int SysIO::begin()
{
    if (m_isInitialized) { return SYS_SUCCESS; }

    m_pimpl->readBuffer = (uint8_t*)malloc(HDLC_READ_BUFFER_SIZE * sizeof(uint8_t));
    if (!m_pimpl->readBuffer) { sysLogger.printf("SysIO::begin(): Error creating Hdlcpp\n\r"); return SYS_FAILURE; }

    m_isInitialized = true;
    return SYS_SUCCESS;
}

bool SysIO::scanInputs()
{
    if (!g_hdlcppPtr) { return 0; }
    if (!m_pimpl->readBuffer) { return 0; }
    if (!g_hdlcPtr) { return 0; }
    if (!g_debugPtr) { return 0; }

    int bytesRead = 0;
    {
        std::lock_guard<std::mutex> lock(g_hdlcMtx);
        bytesRead = g_hdlcppPtr->read(m_pimpl->readBuffer, HDLC_READ_BUFFER_SIZE);
    }
    if (bytesRead > 0) {
        HdlcMessageType type = static_cast<HdlcMessageType>(m_pimpl->readBuffer[0]);  // type is the first byte
        switch(type) {
        case HdlcMessageType::PHYSICAL_CONTROL :
        {
            HdlcPhysicalControlPkt pkt = decodePhysicalControlPkt(m_pimpl->readBuffer, bytesRead);
            m_pimpl->m_processPhysicalControl(pkt);
            break;
        }
        case HdlcMessageType::MIDI_MSG :
        {
            HdlcMidiMsgPkt pkt = decodeMidiMsgPkt(m_pimpl->readBuffer, bytesRead);
            m_pimpl->m_processMidiMsg(pkt);
            break;
        }
        default:
            SYS_DEBUG_PRINT(sysLogger.printf("SysIO::scanInputs(): invalid HDLC message received:%u\n", static_cast<unsigned>(type)));
            break;
        }

        return true;
    }
    else if (bytesRead < 0) {
        sysLogger.printf("SysIO::scanInputs(): HDLC Read error, errno=%d\n", bytesRead);
    }

    return false;
}

///////////////
// SysBounce
///////////////
struct SysBounce::_impl {
    int  id;
    bool state;
    bool prevState;
    bool changed;
};

SysBounce::SysBounce()
: m_pimpl(std::make_unique<_impl>())
{
    m_pimpl->id        = 0;
    m_pimpl->state     = false;
    m_pimpl->prevState = false;
    m_pimpl->changed   = false;
}

SysBounce::SysBounce(int id, PinMode mode)
: SysBounce()
{
    m_pimpl->id = id;
}

SysBounce::SysBounce(const SysBounce& sysBounce)
: SysBounce()
{

}

SysBounce::~SysBounce() = default;

void SysBounce::setupPin(int id, PinMode mode)
{
    m_pimpl->id = id;
}

void SysBounce::debounceIntervalMs(int milliseconds)
{

}

bool SysBounce::read()
{
    return m_pimpl->state;
}

bool SysBounce::update() {
    // libAvalon assumes active low, but these are active high coming from the ESP32
    m_pimpl->state = !static_cast<bool>(g_switches & (0x1U << m_pimpl->id));

    if (m_pimpl->state != m_pimpl->prevState) {
        m_pimpl->prevState = m_pimpl->state;
        m_pimpl->changed = true;
        return true;
    } else {
        m_pimpl->changed = false;
    }
    return false;
}

bool SysBounce::rose()
{
    return m_pimpl->changed && m_pimpl->state;
}

bool SysBounce::fell()
{
    return m_pimpl->changed && !m_pimpl->state;
}

bool SysBounce::changed()
{
    return m_pimpl->changed;
}

//////////////
// SysButton
//////////////
SysButton::SysButton()
: SysBounce()
{
    m_stateForPressed = 0;  // active low
}

SysButton::SysButton(int id, PinMode mode)
: SysBounce(id, mode)
{

}

SysButton::~SysButton() = default;

void SysButton::setPressedState(bool state) { m_stateForPressed = state; }
bool SysButton::getPressedState() { return m_stateForPressed; }
bool SysButton::isPressed() {  return read() == getPressedState(); }
bool SysButton::pressed() {
    return changed() && isPressed();
}

bool SysButton::released() {
    return  changed() && !isPressed();
}

///////////////
// SysEncoder
///////////////
struct SysEncoder::_impl {
    int id = 0;
};

SysEncoder::SysEncoder(int id)
: m_pimpl(std::make_unique<_impl>())
{
    m_pimpl->id = id;
}

SysEncoder::~SysEncoder() = default;

SysEncoder::SysEncoder(const SysEncoder& sysEncoder)
: m_pimpl(std::make_unique<_impl>())
{

}

int SysEncoder::read() {
    return g_encoderValues[m_pimpl->id];
}

//////////////
// SysOutput
//////////////
SysOutput::SysOutput() = default;

SysOutput::SysOutput(int id)
: m_mappedId(id)
{
}

void SysOutput::setPin(int id)
{
    m_mappedId = id;
}

void SysOutput::setValue(bool value)
{
    if (!g_hdlcppPtr) { return; }
    HdlcPhysicalControlPktRaw txPkt = makePhysicalControlPktRaw(
        HdlcPhysicalControlType::LED, HdlcPhysicalControlActions::LED_SET_VALUE, m_mappedId, value);
        {
            std::lock_guard<std::mutex> lock(g_hdlcMtx);
            g_hdlcppPtr->write((uint8_t*)&txPkt, sizeof(txPkt));
        }
}

////////////////////
// SysAnalogInput
////////////////////
SysAnalogInput::SysAnalogInput() = default;

SysAnalogInput::SysAnalogInput(int id)
: m_mappedId(id)
{

}

void SysAnalogInput::setPin(int id)
{
    m_mappedId = id;
}
int SysAnalogInput::getValueInt()
{
    return g_potValues[m_mappedId];
}

int  sysAnalogReadInt(int id)
{
    return g_potValues[id];
}

}
