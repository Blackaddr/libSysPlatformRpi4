#pragma once

#include <circle/i2cmaster.h>
#include <circle/types.h>

namespace SysPlatform {

constexpr int WM8731_I2C_ADDRESS = 0x1A;
constexpr int WM8731_NUM_REGS     = 10; ///< Number of registers in the internal shadow array

class SysCodecWM8731 {
public:
    SysCodecWM8731() = delete;
	SysCodecWM8731 (CI2CMaster *pI2CMaster, u8 uchI2CAddress = 0);
	virtual ~SysCodecWM8731();


	/// Mute and power down the codec.
	void disable(void);

	/// First disable, then cleanly power up and unmute the codec.
	virtual void enable(void);

    /// Sets if the gain controls are locked
	/// @param lockEnabled when true, locks the gain settings from being altered
	void setGainLock(bool lockEnabled);

	/// Set the input gain of the codec's PGA for the left (TRS Tip) channel.
	/// @param val an interger value from 31 = +12dB . . 1.5dB steps down to 0 = -34.5dB
	void setLeftInputGain(int val);

	/// Set the input gain of the codec's PGA for the right (TRS Ring) channel.
	/// @param val an interger value from 31 = +12dB . . 1.5dB steps down to 0 = -34.5dB
	void setRightInputGain(int val);

	/// Mute/unmute the Left (TRS Tip) channel at the ADC input.
	/// @param val when true, channel is muted, when false, channel is unmuted
	void setLeftInMute(bool val);

	/// Mute/unmute the Right (TRS Ring) channel at the ADC input.
	/// @param val when true, channel is muted, when false, channel is unmuted
	void setRightInMute(bool val);

	/// Links the Left/Right channel contols for mute and input gain.
	/// @details when true, changing either the left or right gain/mute controls will
	/// affect both channels.
	/// @param val when true, channels are linked, when false, they are controlled separately
	void setLinkLeftRightIn(bool val);
	/// Swaps the left and right channels in the codec.
	///param val when true, channels are swapped, else normal.
	void setLeftRightSwap(bool val);

	/// Set the volume for the codec's built-in headphone amp
	/// @param volume the input volume level from 0.0f to 1.0f;
	void setHeadphoneVolume(float volume);

	/// Mute/unmute the output DAC, affects both Left and Right output channels.
	/// @param when true, output DAC is muted, when false, unmuted.
	void setDacMute(bool val);

	/// Control when the DAC is feeding the output analog circuitry.
	/// @param val when true, the DAC output is connected to the analog output. When
	/// false, the DAC is disconnected.
	void setDacSelect(bool val);

	/// ADC Bypass control.
	/// @details This permits the analog audio from the Codec's PGA to bypass the ADC
	/// and go directly to the analog output of the codec, bypassing the digital domain
	/// entirely.
	/// @param val when true, analog ADC input is fed directly to codec analog otuput.
	void setAdcBypass(bool val);

	/// Digital High Pass Filter disable. RECOMMENDED ALWAYS TRUE!
	/// @details this controls a HPF in the codec that attempts to remove the lowest
	/// frequency (i.e. < 10 Hz) to improve headroom by dynamically measuring DC level.
	/// In most cases, it introduces noise components by modulating the filter. This
	/// is not suitable for applications where the audio is used for output, but might
	/// be useful for applications like tuning, pitch detection, whre the noise components
	/// can be tolerated.
	/// @param val when true (recommended) the dynamic HPF is disabled, otherwise enabled.
	void setHPFDisable(bool val);

    /// Activates the I2S interface on the codec.
	/// @param val when true, I2S interface is active, when false it is disabled.
	void setActivate(bool val);

	/// Soft-reset the codec.
	/// @details This will cause the codec to reset its internal registers to default values.
	void resetCodec(void);

	/// Recalibrate the DC offset removal
	/// @details This function will temporarily mute the inputs, enable the CODEC's
	/// HPF in order to calculate the DC offset (which is subtracted out). Then the filter
	/// is disabled.
	void recalibrateDcOffset(void);

	/// Write a custom command to the codec via I2C control interface.
	/// @details See WM8731 datasheet for register map details.
	/// @param addr The register address you wish to write to, range 0 to 15.
	/// @param val the 9-bit data value you wish to write at the address specifed.
	bool writeI2C(unsigned int addr, unsigned int val);

	bool isEnabled() { return m_isEnabled; }

private:
	boolean InitWM8731 (u8 uchI2CAddress);

private:
	CI2CMaster *m_pI2CMaster;
	u8 m_uchI2CAddress;
	bool m_isEnabled  = false;
	bool m_gainLocked = false;

	// A shadow array for the registers on the codec since the interface is write-only.
	int regArray[WM8731_NUM_REGS];

	// low-level write command
	bool write(unsigned int reg, unsigned int val);

	// resets the internal shadow register array
	void resetInternalReg(void);
};

} // namespace SysPlatform
