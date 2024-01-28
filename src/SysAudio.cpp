#include "sysPlatform/SysAudio.h"

namespace SysPlatform {

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

}
