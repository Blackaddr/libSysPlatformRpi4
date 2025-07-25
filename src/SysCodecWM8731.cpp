#include <circle/string.h>
#include <circle/types.h>
#include <assert.h>

#include <circle/screen.h>
#include <circle/serial.h>
#include <circle/exceptionhandler.h>
#include <circle/interrupt.h>
#include <circle/timer.h>
#include <circle/logger.h>
#include <circle/types.h>

#include "sysPlatform/SysLogger.h"
#include "SysCodecWM8731.h"

namespace SysPlatform {

#define delay(x) do { CTimer::Get()->MsDelay(x);} while(0)

SysCodecWM8731::SysCodecWM8731 (CI2CMaster *pI2CMaster, u8 uchI2CAddress)
:	m_pI2CMaster (pI2CMaster),
	m_uchI2CAddress (uchI2CAddress)
{
    resetInternalReg();
}

SysCodecWM8731::~SysCodecWM8731()
{
	
}

// For WM8731 i2c register is 7 bits and value is 9 bits,
// so let's have a helper for packing this into two bytes
//#define SHIFT_BIT(r, v) {((v&0x0100)>>8) | (r<<1), (v&0xff)}

// use const instead of define for proper scoping
constexpr int WM8731_I2C_ADDR = 0x1A;

// The WM8731 register map
constexpr int WM8731_REG_LLINEIN   = 0;
constexpr int WM8731_REG_RLINEIN   = 1;
constexpr int WM8731_REG_LHEADOUT  = 2;
constexpr int WM8731_REG_RHEADOUT  = 3;
constexpr int WM8731_REG_ANALOG     =4;
constexpr int WM8731_REG_DIGITAL   = 5;
constexpr int WM8731_REG_POWERDOWN = 6;
constexpr int WM8731_REG_INTERFACE = 7;
constexpr int WM8731_REG_SAMPLING  = 8;
constexpr int WM8731_REG_ACTIVE    = 9;
constexpr int WM8731_REG_RESET    = 15;

// Register Masks and Shifts
// Register 0
constexpr int WM8731_LEFT_INPUT_GAIN_ADDR = 0;
constexpr int WM8731_LEFT_INPUT_GAIN_MASK = 0x1F;
constexpr int WM8731_LEFT_INPUT_GAIN_SHIFT = 0;
constexpr int WM8731_LEFT_INPUT_MUTE_ADDR = 0;
constexpr int WM8731_LEFT_INPUT_MUTE_MASK = 0x80;
constexpr int WM8731_LEFT_INPUT_MUTE_SHIFT = 7;
constexpr int WM8731_LINK_LEFT_RIGHT_IN_ADDR = 0;
constexpr int WM8731_LINK_LEFT_RIGHT_IN_MASK = 0x100;
constexpr int WM8731_LINK_LEFT_RIGHT_IN_SHIFT = 8;
// Register 1
constexpr int WM8731_RIGHT_INPUT_GAIN_ADDR = 1;
constexpr int WM8731_RIGHT_INPUT_GAIN_MASK = 0x1F;
constexpr int WM8731_RIGHT_INPUT_GAIN_SHIFT = 0;
constexpr int WM8731_RIGHT_INPUT_MUTE_ADDR = 1;
constexpr int WM8731_RIGHT_INPUT_MUTE_MASK = 0x80;
constexpr int WM8731_RIGHT_INPUT_MUTE_SHIFT = 7;
constexpr int WM8731_LINK_RIGHT_LEFT_IN_ADDR = 1;
constexpr int WM8731_LINK_RIGHT_LEFT_IN_MASK = 0x100;
constexpr int WM8731_LINK_RIGHT_LEFT_IN_SHIFT = 8;
// Register 2
constexpr int WM8731_LEFT_HEADPHONE_VOL_ADDR = 2;
constexpr int WM8731_LEFT_HEADPHONE_VOL_MASK = 0x7F;
constexpr int WM8731_LEFT_HEADPHONE_VOL_SHIFT = 0;
constexpr int WM8731_LEFT_HEADPHONE_ZCD_ADDR = 2;
constexpr int WM8731_LEFT_HEADPHONE_ZCD_MASK = 0x80;
constexpr int WM8731_LEFT_HEADPHONE_ZCD_SHIFT = 7;
constexpr int WM8731_LEFT_HEADPHONE_LINK_ADDR = 2;
constexpr int WM8731_LEFT_HEADPHONE_LINK_MASK = 0x100;
constexpr int WM8731_LEFT_HEADPHONE_LINK_SHIFT = 8;
// Register 3
constexpr int WM8731_RIGHT_HEADPHONE_VOL_ADDR = 3;
constexpr int WM8731_RIGHT_HEADPHONE_VOL_MASK = 0x7F;
constexpr int WM8731_RIGHT_HEADPHONE_VOL_SHIFT = 0;
constexpr int WM8731_RIGHT_HEADPHONE_ZCD_ADDR = 3;
constexpr int WM8731_RIGHT_HEADPHONE_ZCD_MASK = 0x80;
constexpr int WM8731_RIGHT_HEADPHONE_ZCD_SHIFT = 7;
constexpr int WM8731_RIGHT_HEADPHONE_LINK_ADDR = 3;
constexpr int WM8731_RIGHT_HEADPHONE_LINK_MASK = 0x100;
constexpr int WM8731_RIGHT_HEADPHONE_LINK_SHIFT = 8;
// Register 4
constexpr int WM8731_ADC_BYPASS_ADDR = 4;
constexpr int WM8731_ADC_BYPASS_MASK = 0x8;
constexpr int WM8731_ADC_BYPASS_SHIFT = 3;
constexpr int WM8731_DAC_SELECT_ADDR = 4;
constexpr int WM8731_DAC_SELECT_MASK = 0x10;
constexpr int WM8731_DAC_SELECT_SHIFT = 4;
// Register 5
constexpr int WM8731_DAC_MUTE_ADDR = 5;
constexpr int WM8731_DAC_MUTE_MASK = 0x8;
constexpr int WM8731_DAC_MUTE_SHIFT = 3;
constexpr int WM8731_HPF_DISABLE_ADDR = 5;
constexpr int WM8731_HPF_DISABLE_MASK = 0x1;
constexpr int WM8731_HPF_DISABLE_SHIFT = 0;
constexpr int WM8731_HPF_STORE_OFFSET_ADDR = 5;
constexpr int WM8731_HPF_STORE_OFFSET_MASK = 0x10;
constexpr int WM8731_HPF_STORE_OFFSET_SHIFT = 4;
// Register 7
constexpr int WM8731_LRSWAP_ADDR = 5;
constexpr int WM8731_LRSWAP_MASK = 0x20;
constexpr int WM8731_LRSWAPE_SHIFT = 5;

// Register 9
constexpr int WM8731_ACTIVATE_ADDR = 9;
constexpr int WM8731_ACTIVATE_MASK = 0x1;

boolean SysCodecWM8731::InitWM8731 (u8 uchI2CAddress)
{
    disable();
    delay(100);
    enable();

	return TRUE;
}

// Reset the internal shadow register array to match
// the reset state of the codec.
void SysCodecWM8731::resetInternalReg(void) {
	// Set to reset state
	regArray[0] = 0x97;
	regArray[1] = 0x97;
	regArray[2] = 0x79;
	regArray[3] = 0x79;
	regArray[4] = 0x0a;
	regArray[5] = 0x8;
	regArray[6] = 0x9f;
	regArray[7] = 0xa;
	regArray[8] = 0;
	regArray[9] = 0;
}

// Powerdown and disable the codec
void SysCodecWM8731::disable(void)
{
	SYS_DEBUG_PRINT(sysLogger.printf("SysCodecWM8731::disable(): ...disabling the codec\n"));
	// set OUTPD to '1' (powerdown), which is bit 4
	regArray[WM8731_REG_POWERDOWN] |= 0x10;
	write(WM8731_REG_POWERDOWN, regArray[WM8731_REG_POWERDOWN]);
	delay(100); // wait for power down

	// power down the rest of the supplies
	write(WM8731_REG_POWERDOWN, 0x9f); // complete codec powerdown
	delay(100);

	resetCodec();
	m_isEnabled = false;
}

void SysCodecWM8731::setGainLock(bool lockEnabled)
{
	m_gainLocked = lockEnabled;
}

// Set the PGA gain on the Left channel
void SysCodecWM8731::setLeftInputGain(int val)
{
	if (m_gainLocked) { return; }

	regArray[WM8731_LEFT_INPUT_GAIN_ADDR] &= ~WM8731_LEFT_INPUT_GAIN_MASK;
	regArray[WM8731_LEFT_INPUT_GAIN_ADDR] |=
			((val << WM8731_LEFT_INPUT_GAIN_SHIFT) & WM8731_LEFT_INPUT_GAIN_MASK);
	write(WM8731_LEFT_INPUT_GAIN_ADDR, regArray[WM8731_LEFT_INPUT_GAIN_ADDR]);
}

// Mute control on the ADC Left channel
void SysCodecWM8731::setLeftInMute(bool val)
{
	if (val) {
		regArray[WM8731_LEFT_INPUT_MUTE_ADDR] |= WM8731_LEFT_INPUT_MUTE_MASK;
	} else {
		regArray[WM8731_LEFT_INPUT_MUTE_ADDR] &= ~WM8731_LEFT_INPUT_MUTE_MASK;
	}
	write(WM8731_LEFT_INPUT_MUTE_ADDR, regArray[WM8731_LEFT_INPUT_MUTE_ADDR]);
}

// Link the gain/mute controls for Left and Right channels
void SysCodecWM8731::setLinkLeftRightIn(bool val)
{
	if (val) {
		regArray[WM8731_LINK_LEFT_RIGHT_IN_ADDR] |= WM8731_LINK_LEFT_RIGHT_IN_MASK;
		regArray[WM8731_LINK_RIGHT_LEFT_IN_ADDR] |= WM8731_LINK_RIGHT_LEFT_IN_MASK;
	} else {
		regArray[WM8731_LINK_LEFT_RIGHT_IN_ADDR] &= ~WM8731_LINK_LEFT_RIGHT_IN_MASK;
		regArray[WM8731_LINK_RIGHT_LEFT_IN_ADDR] &= ~WM8731_LINK_RIGHT_LEFT_IN_MASK;
	}
	write(WM8731_LINK_LEFT_RIGHT_IN_ADDR, regArray[WM8731_LINK_LEFT_RIGHT_IN_ADDR]);
	write(WM8731_LINK_RIGHT_LEFT_IN_ADDR, regArray[WM8731_LINK_RIGHT_LEFT_IN_ADDR]);
}

// Set the PGA input gain on the Right channel
void SysCodecWM8731::setRightInputGain(int val)
{
	if (m_gainLocked) { return; }

	regArray[WM8731_RIGHT_INPUT_GAIN_ADDR] &= ~WM8731_RIGHT_INPUT_GAIN_MASK;
	regArray[WM8731_RIGHT_INPUT_GAIN_ADDR] |=
			((val << WM8731_RIGHT_INPUT_GAIN_SHIFT) & WM8731_RIGHT_INPUT_GAIN_MASK);
	write(WM8731_RIGHT_INPUT_GAIN_ADDR, regArray[WM8731_RIGHT_INPUT_GAIN_ADDR]);
}

// Mute control on the input ADC right channel
void SysCodecWM8731::setRightInMute(bool val)
{
	if (val) {
		regArray[WM8731_RIGHT_INPUT_MUTE_ADDR] |= WM8731_RIGHT_INPUT_MUTE_MASK;
	} else {
		regArray[WM8731_RIGHT_INPUT_MUTE_ADDR] &= ~WM8731_RIGHT_INPUT_MUTE_MASK;
	}
	write(WM8731_RIGHT_INPUT_MUTE_ADDR, regArray[WM8731_RIGHT_INPUT_MUTE_ADDR]);
}

// Left/right swap control
void SysCodecWM8731::setLeftRightSwap(bool val)
{
	if (val) {
		regArray[WM8731_LRSWAP_ADDR] |= WM8731_LRSWAP_MASK;
	} else {
		regArray[WM8731_LRSWAP_ADDR] &= ~WM8731_LRSWAP_MASK;
	}
	write(WM8731_LRSWAP_ADDR, regArray[WM8731_LRSWAP_ADDR]);
}

void SysCodecWM8731::setHeadphoneVolume(float volume)
{
	// the codec volume goes from 0x30 to 0x7F. Anything below 0x30 is mute.
	// 0dB gain is 0x79. Total range is 0x50 (80) possible values.
	unsigned vol;
	constexpr unsigned RANGE = 80.0f;
	if (volume < 0.0f) {
		vol = 0;
	} else if (volume > 1.0f) {
		vol = 0x7f;
	} else {
		vol = 0x2f + static_cast<unsigned>(volume * RANGE);
	}
	regArray[WM8731_LEFT_HEADPHONE_VOL_ADDR] &= ~WM8731_LEFT_HEADPHONE_VOL_MASK; // clear the volume first
	regArray[WM8731_LEFT_HEADPHONE_VOL_ADDR] |=
			((vol << WM8731_LEFT_HEADPHONE_VOL_SHIFT) & WM8731_LEFT_HEADPHONE_VOL_MASK);
	write(WM8731_LEFT_HEADPHONE_VOL_ADDR, regArray[WM8731_LEFT_HEADPHONE_VOL_ADDR]);
}

// Dac output mute control
void SysCodecWM8731::setDacMute(bool val)
{
	if (val) {
		regArray[WM8731_DAC_MUTE_ADDR] |= WM8731_DAC_MUTE_MASK;
	} else {
		regArray[WM8731_DAC_MUTE_ADDR] &= ~WM8731_DAC_MUTE_MASK;
	}
	write(WM8731_DAC_MUTE_ADDR, regArray[WM8731_DAC_MUTE_ADDR]);
}

// Switches the DAC audio in/out of the output path
void SysCodecWM8731::setDacSelect(bool val)
{
	if (val) {
		regArray[WM8731_DAC_SELECT_ADDR] |= WM8731_DAC_SELECT_MASK;
	} else {
		regArray[WM8731_DAC_SELECT_ADDR] &= ~WM8731_DAC_SELECT_MASK;
	}
	write(WM8731_DAC_SELECT_ADDR, regArray[WM8731_DAC_SELECT_ADDR]);
}

// Bypass sends the ADC input audio (analog) directly to analog output stage
// bypassing all digital processing
void SysCodecWM8731::setAdcBypass(bool val)
{
	if (val) {
		regArray[WM8731_ADC_BYPASS_ADDR] |= WM8731_ADC_BYPASS_MASK;
	} else {
		regArray[WM8731_ADC_BYPASS_ADDR] &= ~WM8731_ADC_BYPASS_MASK;
	}
	write(WM8731_ADC_BYPASS_ADDR, regArray[WM8731_ADC_BYPASS_ADDR]);
}

// Enable/disable the dynamic HPF (recommended, it creates noise)
void SysCodecWM8731::setHPFDisable(bool val)
{
	if (val) {
		regArray[WM8731_HPF_DISABLE_ADDR] |= WM8731_HPF_DISABLE_MASK;
	} else {
		regArray[WM8731_HPF_DISABLE_ADDR] &= ~WM8731_HPF_DISABLE_MASK;
	}
	write(WM8731_HPF_DISABLE_ADDR, regArray[WM8731_HPF_DISABLE_ADDR]);
}

// Activate/deactive the I2S audio interface
void SysCodecWM8731::setActivate(bool val)
{
	if (val) {
		write(WM8731_ACTIVATE_ADDR, WM8731_ACTIVATE_MASK);
	} else {
		write(WM8731_ACTIVATE_ADDR, 0);
	}

}

// Trigger the on-chip codec reset
void SysCodecWM8731::resetCodec(void)
{
	write(WM8731_REG_RESET, 0x0);
	resetInternalReg();
}

void SysCodecWM8731::recalibrateDcOffset(void)
{
    // mute the inputs
    setLeftInMute(true);
    setRightInMute(true);

    // enable the HPF and DC offset store
    setHPFDisable(false);
    regArray[WM8731_HPF_STORE_OFFSET_ADDR] |= WM8731_HPF_STORE_OFFSET_MASK;
    write(WM8731_REG_DIGITAL, regArray[WM8731_REG_DIGITAL]);

    delay(1000); // wait for the DC offset to be calculated over 1 second
    setHPFDisable(true); // disable the dynamic HPF calculation
    delay(500);

    // unmute the inputs
    setLeftInMute(false);
    setRightInMute(false);
}

// Direct write control to the codec
bool SysCodecWM8731::writeI2C(unsigned int addr, unsigned int val)
{
	return write(addr, val);
}

// Low level write control for the codec via the Teensy I2C interface
bool SysCodecWM8731::write(unsigned int reg, unsigned int val)
{
	bool done = false;

	while (!done) {

        u8 buffer[2];
		buffer[0] = (reg << 1) | ((val >> 8) & 1);
		buffer[1] = val & 0xFFU;
		int result = m_pI2CMaster->Write (WM8731_I2C_ADDR, buffer, 2);
		if (result != 2) {
			// retry
			//CString msg;
			//msg.Format("SysCodecWM8731::write(): could not write 2 bytes, only wrote %d\n", result);
		} else {
			done = true;
		}
	}

	return true;
}

// Powerup and unmute the codec
void SysCodecWM8731::enable(void)
{
    SYS_DEBUG_PRINT(sysLogger.printf("SysCodecWM8731::enable(): enabling the codec\n"));
    disable(); // disable first in case it was already powered up
	delay(100);

    // Sequence from WAN0111.pdf
    // Begin configuring the codec
    resetCodec();
    delay(100); // wait for reset

    // Power up all domains except OUTPD and microphone
    regArray[WM8731_REG_POWERDOWN] = 0x12;
    write(WM8731_REG_POWERDOWN, regArray[WM8731_REG_POWERDOWN]);
    delay(100); // wait for codec powerup


    setAdcBypass(false); // causes a slight click
    setDacSelect(true);
    setHPFDisable(true);
    setLeftInputGain(0x17); // default input gain
    setRightInputGain(0x17);
    setLeftInMute(false); // no input mute
    setRightInMute(false);
    setDacMute(false); // unmute the DAC

    // link, but mute the headphone outputs
    regArray[WM8731_REG_LHEADOUT] = WM8731_LEFT_HEADPHONE_LINK_MASK;
    write(WM8731_REG_LHEADOUT, regArray[WM8731_REG_LHEADOUT]);      // volume off
    regArray[WM8731_REG_RHEADOUT] = WM8731_RIGHT_HEADPHONE_LINK_MASK;
    write(WM8731_REG_RHEADOUT, regArray[WM8731_REG_RHEADOUT]);

    /// Configure the audio interface
    write(WM8731_REG_INTERFACE, 0x42); // I2S, 16 bit, MCLK master
    regArray[WM8731_REG_INTERFACE] = 0x42;

    write(WM8731_REG_SAMPLING, 0x20);  // 256*Fs, 44.1 kHz, MCLK/1
    regArray[WM8731_REG_SAMPLING] = 0x20;
    delay(100); // wait for interface config

    // Activate the audio interface
    setActivate(true);
    delay(100);

    write(WM8731_REG_POWERDOWN, 0x02); // power up outputs
    regArray[WM8731_REG_POWERDOWN] = 0x02;
    delay(500); // wait for output to power up

    delay(100); // wait for mute ramp

	m_isEnabled = true;
}

} // namespace multiverse
