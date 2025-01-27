#include <cstdlib>
#include "globalInstances.h"
#include "circle/sound/i2ssoundbasedevice.h"

#include "sysPlatform/SysLogger.h"
#include "SysCodecWM8731.h"
#include "sysPlatform/SysAudio.h"

#define FORMAT		    SoundFormatSigned16
#define WRITE_CHANNELS	2		// 1: Mono, 2: Stereo
#define QUEUE_SIZE_MSECS 100		// size of the sound queue in milliseconds duration

namespace SysPlatform {

CI2SSoundBaseDevice* m_soundDevPtr = nullptr;

static void rxIsr(void *param)
{
	AudioStream::update_all();
}

static void createSoundDevice()
{
    static bool deviceCreated = false;

    if (deviceCreated) { return; }
    if (!m_soundDevPtr) {
        m_soundDevPtr = new CI2SSoundBaseDevice (g_interruptSysPtr, AUDIO_SAMPLE_RATE_HZ, 2*AUDIO_SAMPLES_PER_BLOCK, TRUE /* RPI is slave*/,
						g_i2cMasterPtr, 0, CI2SSoundBaseDevice::DeviceModeTXRX);
        if (!m_soundDevPtr) {
            SYS_DEBUG_PRINT(sysLogger.printf("SysAudio:createSoundDevice(): Cannot create CI2SSoundBaseDevice\n"));
            return;
        }
    }

    m_soundDevPtr->RegisterSecondaryRxHandler(rxIsr, nullptr);
    //m_soundDevPtr->RegisterSecondaryTxHandler(txIsr, nullptr);


    // configure sound device
    if (!m_soundDevPtr->AllocateQueue (QUEUE_SIZE_MSECS))
    {
        //LogScreen("Cannot allocate sound queue\n");
        g_loggerPtr->Write("SysAudio:createSoundDevice", TLogSeverity::LogError, "Cannot allocate sound queue\n");
        SYS_DEBUG_PRINT(sysLogger.printf("SysAudio:createSoundDevice(): Cannot allocate sound queue\n"));
        return;
    }
    if (!m_soundDevPtr->AllocateReadQueue (QUEUE_SIZE_MSECS))
    {
        //LogScreen("Cannot allocate input sound queue\n");
        g_loggerPtr->Write("SysAudio:createSoundDevice", TLogSeverity::LogError, "Cannot allocate read queue\n");
        SYS_DEBUG_PRINT(sysLogger.printf("SysAudio:createSoundDevice(): Cannot allocate read queue\n"));
        return;
    }
    m_soundDevPtr->SetWriteFormat (FORMAT, WRITE_CHANNELS);
    m_soundDevPtr->SetReadFormat (FORMAT, WRITE_CHANNELS);

    // start sound device
    if (!m_soundDevPtr->Start ())
    {
        //LogScreen("Cannot start sound device\n");
        g_loggerPtr->Write("SysAudio:createSoundDevice", TLogSeverity::LogError, "Cannot start sound device\n");
        SYS_DEBUG_PRINT(sysLogger.printf("SysAudio:createSoundDevice(): Cannot start sound device\n"));
        return;
    }
    //LogScreen("AudioInputI2S::begin() completed sound device setup\n");
    //g_loggerPtr->Write("SysAudio:createSoundDevice", TLogSeverity::LogError, "completed sound device setup\n");
    deviceCreated = true;
}

/////////////
// SysCodec
/////////////
SysCodec sysCodec;

SysCodec& SysCodec::getCodec() { return sysCodec; }

struct SysCodec::_impl {
    SysCodecWM8731* m_codecPtr = nullptr;
};


SysCodec::SysCodec()
: m_pimpl(std::make_unique<_impl>())
{

}

SysCodec::~SysCodec()
{
    if (m_pimpl->m_codecPtr) { delete m_pimpl->m_codecPtr; }
}

void SysCodec::begin(void)
{
    if (!m_pimpl->m_codecPtr) {
        if (g_i2cMasterPtr) {
            m_pimpl->m_codecPtr = new SysCodecWM8731(g_i2cMasterPtr, WM8731_I2C_ADDRESS);
            if (!m_pimpl->m_codecPtr) { SYS_DEBUG_PRINT(sysLogger.printf("SysCodec::begin(): failed to create SysCodecWM8731\n")); }
        } else {
            SYS_DEBUG_PRINT(sysLogger.printf("SysCodec::begin(): ERROR: g_i2cMasterPtr is nullptr\n"));
        }
    }
}

void SysCodec::disable(void)
{
    if (m_pimpl->m_codecPtr) { m_pimpl->m_codecPtr->disable(); }
}


void SysCodec::enable(void)
{
    if (m_pimpl->m_codecPtr) { m_pimpl->m_codecPtr->enable(); }
    else { SYS_DEBUG_PRINT(sysLogger.printf("SysCodec::enable(): m_codecPtr is invalid\n")); }
}

bool SysCodec::isEnabled()
{
    if (m_pimpl->m_codecPtr) { return m_pimpl->m_codecPtr->isEnabled(); }
    else { return false; }
}


void SysCodec::setLeftInputGain(int val)
{
    if (m_pimpl->m_codecPtr) { m_pimpl->m_codecPtr->setLeftInputGain(val); }
}

void SysCodec::setGainLock(bool lockEnabled)
{
    if (m_pimpl->m_codecPtr) { m_pimpl->m_codecPtr->setGainLock(lockEnabled); }
}


void SysCodec::setRightInputGain(int val)
{
    if (m_pimpl->m_codecPtr) { m_pimpl->m_codecPtr->setRightInputGain(val); }
}


void SysCodec::setLeftInMute(bool val)
{
    if (m_pimpl->m_codecPtr) { m_pimpl->m_codecPtr->setLeftInMute(val); }
}


void SysCodec::setRightInMute(bool val)
{
    if (m_pimpl->m_codecPtr) { m_pimpl->m_codecPtr->setRightInMute(val); }
}


void SysCodec::setLinkLeftRightIn(bool val)
{
    if (m_pimpl->m_codecPtr) { m_pimpl->m_codecPtr->setLinkLeftRightIn(val); }
}


bool SysCodec::setLeftRightSwap(bool val)
{
    if (m_pimpl->m_codecPtr) { m_pimpl->m_codecPtr->setLeftRightSwap(val); return true; }
    else { return false; }
}


bool SysCodec::setHeadphoneVolume(float volume)
{
    if (m_pimpl->m_codecPtr) { m_pimpl->m_codecPtr->setHeadphoneVolume(volume); return true; }
    else { return false; }
}


bool SysCodec::setDacMute(bool val)
{
    if (m_pimpl->m_codecPtr) { m_pimpl->m_codecPtr->setDacMute(val); return true; }
    else { return false; }
}


bool SysCodec::setAdcBypass(bool val)
{
    if (m_pimpl->m_codecPtr) { m_pimpl->m_codecPtr->setAdcBypass(val); return true; }
    else { return false; }
}


void SysCodec::resetCodec(void)
{
    if (m_pimpl->m_codecPtr) { return m_pimpl->m_codecPtr->resetCodec(); }
}

bool SysCodec::recalibrateDcOffset(void)
{
    if (m_pimpl->m_codecPtr) { m_pimpl->m_codecPtr->recalibrateDcOffset(); return true; }
    else { return false; }
}

bool SysCodec::writeI2C(unsigned int addr, unsigned int val)
{
    if (m_pimpl->m_codecPtr) { return m_pimpl->m_codecPtr->writeI2C(addr, val); }
    else { return false; }
}


/////////////////////
// SysAudioInputI2S
/////////////////////
struct SysAudioInputI2S::_impl {
    int16_t* buffer = nullptr;
};

SysAudioInputI2S::SysAudioInputI2S(void)
: AudioStream(0, (audio_block_t**)NULL), m_pimpl(std::make_unique<_impl>())
{

}

SysAudioInputI2S::~SysAudioInputI2S()
{

}

void SysAudioInputI2S::enable()
{
	m_enable = true;
}

void SysAudioInputI2S::disable()
{
	m_enable = false;
}

void SysAudioInputI2S::update(void)
{
    if (!m_enable) { return; }

    if (!m_soundDevPtr) { return; }
    size_t buffSize = AUDIO_SAMPLES_PER_BLOCK*2*sizeof(int16_t);

    size_t nBytes = m_soundDevPtr->Read(m_pimpl->buffer, buffSize);

    if (nBytes != buffSize) {
        //LogScreen("AudioInputI2S::update(): ERROR only read %d bytes, expected %d\n", nBytes, buffSize);
        g_loggerPtr->Write("SysAudioInputI2S::update", TLogSeverity::LogError, "ERROR only read %d bytes, expected %d\n", nBytes, buffSize);
        return;
    }

    audio_block_t *new_left=NULL, *new_right=NULL;
	new_left = allocate();
	if (new_left != NULL) {
		new_right = allocate();
		if (new_right == NULL) {
			release(new_left);
			new_left = NULL;
            return;
		}
	} else {
        //LogScreen("AudioInputI2S::update(): allocate() errors\n");  // Currenly dies here
        g_loggerPtr->Write("SysAudioInputI2S::update", TLogSeverity::LogError, "allocate() errors\n");
    }

    const int16_t *src  = m_pimpl->buffer;
    int16_t* dest_left  = new_left->data;
    int16_t* dest_right = new_right->data;
    for (unsigned i=0; i < AUDIO_SAMPLES_PER_BLOCK; i++) {
        *dest_left++ = *src++;
        *dest_right++ = *src++;
    }

    transmit(new_left, 0);
    transmit(new_right, 1);

    if (new_left) release(new_left);
    if (new_right) release(new_right);
}

void SysAudioInputI2S::begin(void)
{
    if (m_isInitialized || !g_interruptSysPtr) { return; }
    if (!m_pimpl->buffer) { m_pimpl->buffer = (int16_t*)malloc(AUDIO_SAMPLES_PER_BLOCK*2*sizeof(int16_t)); }

    createSoundDevice();

    update_setup();
    enable();
    m_isInitialized = true;
}

/////////////////////
// SysAudioOutputI2S
/////////////////////
struct SysAudioOutputI2S::_impl {
    int16_t* buffer = nullptr;
};

SysAudioOutputI2S::SysAudioOutputI2S(void)
: AudioStream(2, inputQueueArray), m_pimpl(std::make_unique<_impl>())
{

}

SysAudioOutputI2S::~SysAudioOutputI2S()
{

}

void SysAudioOutputI2S::enable()
{
	m_enable = true;
}

void SysAudioOutputI2S::disable()
{
	m_enable = false;
}

void SysAudioOutputI2S::update(void)
{
    if (!m_enable) { return; }
    if (!m_soundDevPtr) { return; }

    int16_t* ptr = m_pimpl->buffer;
	audio_block_t *leftBlock  = receiveReadOnly(0);
	audio_block_t *rightBlock = receiveReadOnly(1);

	for (unsigned i=0; i < AUDIO_SAMPLES_PER_BLOCK; i++) {
		int16_t val = 0;
		if (leftBlock) { val = leftBlock->data[i]; }
		*ptr++ = val;

		val = 0;
		if (rightBlock) { val = rightBlock->data[i]; }
		*ptr++ = val;
	}
	m_soundDevPtr->Write(m_pimpl->buffer, AUDIO_SAMPLES_PER_BLOCK*2*sizeof(int16_t));
	if (leftBlock)  release(leftBlock);
	if (rightBlock) release(rightBlock);
}

void SysAudioOutputI2S::begin(void)
{
    if (m_isInitialized || !g_interruptSysPtr) { return; }
	if (!m_pimpl->buffer) { m_pimpl->buffer = (int16_t*)malloc(AUDIO_SAMPLES_PER_BLOCK*2*sizeof(int16_t)); }

    createSoundDevice();

	update_setup();
    enable();
    m_isInitialized = true;
}

/////////////////////
// SysAudioInputUsb
/////////////////////
struct SysAudioInputUsb::_impl {
    int dummy;
};

SysAudioInputUsb::SysAudioInputUsb(void)
: AudioStream(0, (audio_block_t**)NULL), m_pimpl(std::make_unique<_impl>())
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
