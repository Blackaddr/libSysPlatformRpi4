#include "sysPlatform/SysAudio.h"

namespace SysPlatform {

/////////////
// SysCodec
/////////////
struct SysCodec::_impl {
    int dummy;
};


SysCodec::SysCodec()
: m_pimpl(std::make_unique<_impl>())
{

}

SysCodec::~SysCodec()
{

}


void SysCodec::disable(void)
{

}


void SysCodec::enable(void)
{

}

bool SysCodec::isEnabled()
{
    return false;
}


void SysCodec::setLeftInputGain(int val)
{

}


void SysCodec::setRightInputGain(int val)
{

}


void SysCodec::setLeftInMute(bool val)
{

}


void SysCodec::setRightInMute(bool val)
{

}


void SysCodec::setLinkLeftRightIn(bool val)
{

}


bool SysCodec::setLeftRightSwap(bool val)
{
    return false;
}


bool SysCodec::setHeadphoneVolume(float volume)
{
    return false;
}


bool SysCodec::setDacMute(bool val)
{
    return false;
}


bool SysCodec::setAdcBypass(bool val)
{
    return false;
}


void SysCodec::resetCodec(void)
{

}

bool SysCodec::recalibrateDcOffset(void)
{
    return false;
}

bool SysCodec::writeI2C(unsigned int addr, unsigned int val)
{
    return false;
}


/////////////////////
// SysAudioInputI2S
/////////////////////
struct SysAudioInputI2S::_impl {
    int dummy;
};

SysAudioInputI2S::SysAudioInputI2S(void)
: AudioStream(0, NULL), m_pimpl(std::make_unique<_impl>())
{

}

SysAudioInputI2S::~SysAudioInputI2S()
{

}

void SysAudioInputI2S::update(void)
{

}

void SysAudioInputI2S::begin(void)
{

}

/////////////////////
// SysAudioOutputI2S
/////////////////////
struct SysAudioOutputI2S::_impl {
    int dummy;
};

SysAudioOutputI2S::SysAudioOutputI2S(void)
: AudioStream(2, inputQueueArray), m_pimpl(std::make_unique<_impl>())
{

}

SysAudioOutputI2S::~SysAudioOutputI2S()
{

}

void SysAudioOutputI2S::update(void)
{

}

void SysAudioOutputI2S::begin(void)
{

}

/////////////////////
// SysAudioInputUsb
/////////////////////
struct SysAudioInputUsb::_impl {
    int dummy;
};

SysAudioInputUsb::SysAudioInputUsb(void)
: AudioStream(0, NULL), m_pimpl(std::make_unique<_impl>())
{

}

SysAudioInputUsb::~SysAudioInputUsb()
{

}

void SysAudioInputUsb::update(void)
{

}

void SysAudioInputUsb::begin(void)
{

}

/////////////////////
// SysAudioOutputUsb
/////////////////////
struct SysAudioOutputUsb::_impl {
    int dummy;
};

SysAudioOutputUsb::SysAudioOutputUsb(void)
: AudioStream(2, inputQueueArray), m_pimpl(std::make_unique<_impl>())
{

}

SysAudioOutputUsb::~SysAudioOutputUsb()
{

}

void SysAudioOutputUsb::update(void)
{

}

void SysAudioOutputUsb::begin(void)
{

}

}
