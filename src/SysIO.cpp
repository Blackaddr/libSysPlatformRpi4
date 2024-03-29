#include <memory>
#include "globalInstances.h"
#include "sysPlatform/SysLogger.h"
#include "sysPlatform/SysTypes.h"
#include "SysIOMapping.h"
#include "SysIO.h"

#include "Hdlcpp.hpp"
#include "HdlcProtocol.h"

namespace SysPlatform {


//////////
// SysIO
//////////
SysIO sysIO;
constexpr size_t HDLC_READ_BUFFER_SIZE = 2048;

struct SysIO::_impl {
    std::shared_ptr<Hdlcpp::Hdlcpp> hdlcppPtr = nullptr;
    uint8_t* readBuffer = nullptr;

    void m_processHdlcPkt(HdlcPhysicalControlPkt& pkt);
    void m_processPhysicalControl(HdlcPhysicalControlPkt& pkt);
    void m_processSwitch(HdlcPhysicalControlPkt& pkt);
};

void SysIO::_impl::m_processHdlcPkt(HdlcPhysicalControlPkt& pkt)
{
    switch (pkt.msgType) {
    case HdlcMessageType::PHYSICAL_CONTROL : return m_processPhysicalControl(pkt);
    default :
        sysLogger.printf("SysIO::_impl::m_processHdlcPkt(): invalid packet type:%u\n", pkt.msgType);
        break;
    }
}

void SysIO::_impl::m_processPhysicalControl(HdlcPhysicalControlPkt& pkt)
{
    switch (pkt.controlType) {
    case HdlcPhysicalControlType::SWITCH : return m_processSwitch(pkt);
    default :
        sysLogger.printf("SysIO::_impl::m_processHdlcPkt(): invalid control type:%u\n", pkt.controlType);
        break;
    }
}

void SysIO::_impl::m_processSwitch(HdlcPhysicalControlPkt& rxPkt)
{
    switch (rxPkt.controlAction) {
    case HdlcPhysicalControlActions::SW_PUSH_TRIGGER :
    {
        HdlcPhysicalControlPktRaw txPkt = makePhysicalControlPktRaw(
            HdlcPhysicalControlType::LED, HdlcPhysicalControlActions::LED_SET_VALUE, rxPkt.controlId, 1.0f);
            hdlcppPtr->write((uint8_t*)&txPkt, sizeof(txPkt));
        break;
    }
    case HdlcPhysicalControlActions::SW_RELEASE_TRIGGER :
    {
        HdlcPhysicalControlPktRaw txPkt = makePhysicalControlPktRaw(
            HdlcPhysicalControlType::LED, HdlcPhysicalControlActions::LED_SET_VALUE, rxPkt.controlId, 0.0f);
            hdlcppPtr->write((uint8_t*)&txPkt, sizeof(txPkt));
        break;
    }
    default :
        sysLogger.printf("m_processHdlcPkt(): invalid action type:%u\n", rxPkt.controlAction);
        break;
    }
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

    m_pimpl->hdlcppPtr = std::make_shared<Hdlcpp::Hdlcpp>(
    [&](uint8_t *data, uint16_t length) {
            if (!g_hdlcPtr) { return 0; }
            return g_hdlcPtr->Read(data, length);
      },
    [&](const uint8_t *data, uint16_t length) {
            if (!g_hdlcPtr) { return 0; }
            //sysLogger.printf("HDLC TX Lambda: transmitting %u bytes\n", length);
            return g_hdlcPtr->Write(data, length);
    }, HDLC_READ_BUFFER_SIZE, 0 /* timeout in ms */, 0 /* retries */);

    if (!m_pimpl->hdlcppPtr) { sysLogger.printf("SysIO::begin(): Error creating Hdlcpp\n\r"); return SYS_FAILURE; }

    m_isInitialized = true;
    return SYS_SUCCESS;
}

bool SysIO::scanInputs()
{
    if (!m_pimpl->hdlcppPtr) { return 0; }
    if (!m_pimpl->readBuffer) { return 0; }
    if (!g_hdlcPtr) { return 0; }
    if (!g_debugPtr) { return 0; }

    int bytesRead = m_pimpl->hdlcppPtr->read(m_pimpl->readBuffer, HDLC_READ_BUFFER_SIZE);
    if (bytesRead > 0) {
        //sysLogger.printf("SysIO::scanInputs(): HDLC decoded data is available, read %u bytes\n", bytesRead);

        //uint8_t* buf = m_pimpl->readBuffer;
        //sysLogger.printf("decodePhysicalControlPkt: buf: %02X %02X %02X %02X\n", buf[0], buf[1], buf[2], buf[3]);

        HdlcPhysicalControlPkt pkt = decodePhysicalControlPkt(m_pimpl->readBuffer, bytesRead);
        //sysLogger.printf("pkt: mtype:%u ctype:%u act:%u id:%u\n", pkt.msgType, pkt.controlType, pkt.controlAction, pkt.controlId);
        m_pimpl->m_processHdlcPkt(pkt);
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
};

SysBounce::SysBounce()
: m_pimpl(std::make_unique<_impl>())
{

}

SysBounce::SysBounce(int id, PinMode mode)
: m_pimpl(std::make_unique<_impl>())
{

}

SysBounce::SysBounce(const SysBounce& sysBounce)
: m_pimpl(std::make_unique<_impl>())
{

}

SysBounce::~SysBounce() = default;

void SysBounce::setupPin(int id, PinMode mode)
{

}

void SysBounce::debounceIntervalMs(int milliseconds)
{

}

bool SysBounce::read()
{
    return false;
}

bool SysBounce::update() {
    return false;
}

bool SysBounce::rose()
{
    return false;
}

bool SysBounce::fell()
{
    return false;
}

bool SysBounce::changed()
{
    return false;
}

//////////////
// SysButton
//////////////
SysButton::SysButton()
: SysBounce()
{

}

SysButton::SysButton(int id, PinMode mode)
: SysBounce(id, mode)
{

}

SysButton::~SysButton() = default;

void SysButton::setPressedState(bool state) {  }
bool SysButton::getPressedState() { return false; }
bool SysButton::isPressed() {  return false; }
bool SysButton::pressed() { return false; }
bool SysButton::released() {  return false; }

///////////////
// SysEncoder
///////////////
struct SysEncoder::_impl {
};

SysEncoder::SysEncoder(int id)
: m_pimpl(std::make_unique<_impl>())
{

}

SysEncoder::~SysEncoder() = default;

SysEncoder::SysEncoder(const SysEncoder& sysEncoder)
: m_pimpl(std::make_unique<_impl>())
{

}

int SysEncoder::read() {
    return 0;
}

//////////////
// SysOutput
//////////////
SysOutput::SysOutput() = default;

SysOutput::SysOutput(int id)
: m_mappedId(-1)
{
}

void SysOutput::setPin(int id)
{

}

void SysOutput::setValue(bool value)
{

}

////////////////////
// SysAnalogInput
////////////////////
SysAnalogInput::SysAnalogInput() = default;

SysAnalogInput::SysAnalogInput(int id)
: m_mappedId(-1)
{

}

void SysAnalogInput::setPin(int id)
{

}
int SysAnalogInput::getValueInt()
{
    return 0;
}

int  sysAnalogReadInt(int id)
{
    return 0;
}

}
