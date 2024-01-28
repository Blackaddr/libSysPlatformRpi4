#include "SysIOMapping.h"
#include "SysIO.h"

namespace SysPlatform {

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
