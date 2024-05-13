#pragma once

#include <cstdint>
#include <cstddef> // for size_t
#include <cstring> // for memcpy
#include "Hdlcpp.hpp"

int transportRead(uint8_t *data, uint16_t length);
int transportWrite(const uint8_t *data, uint16_t length);

enum class HdlcMessageType : uint8_t {
    INVALID = 0,
    PHYSICAL_CONTROL,
    MIDI_MSG,
    NUM_MESSAGE_TYPES
};

enum class HdlcPhysicalControlType : uint8_t {
    INVALID = 0,
    SWITCH,
    ENCODER,
    LED,
    POT,
    EXPRESSION
};

enum class HdlcPhysicalControlActions : uint8_t {
    INVALID = 0,
    SW_PUSH_STATE,
    SW_RELEASE_STATE,
    SW_PUSH_TRIGGER,
    SW_RELEASE_TRIGGER,
    SW_RAW_VALUE,
    LED_SET_VALUE,
    LED_GET_VALUE,
    ENCODER_VALUE,
    ANALOG_VALUE
};


struct HdlcPhysicalControlPktRaw {
    uint8_t msgType;
    uint8_t controlType;
    uint8_t controlAction;
    uint8_t controlId;
    float   controlValue;
};

struct HdlcPhysicalControlPkt {
    HdlcMessageType         msgType;
    HdlcPhysicalControlType controlType;
    HdlcPhysicalControlActions     controlAction;
    uint8_t                 controlId;
    float                   controlValue;
};


// Switch Processing
HdlcPhysicalControlPktRaw makePhysicalControlPktRaw(
    HdlcPhysicalControlType controlType, HdlcPhysicalControlActions controlAction, unsigned controlId, float controlValue);

HdlcPhysicalControlPkt decodePhysicalControlPkt(uint8_t* buf, size_t bufferSize);

// MIDI Processing
// From usb.org MIDI 1.0 specification. This 4 byte structure is the unit
// of transfer for MIDI data over USB.
typedef struct __attribute__((__packed__)) {
  uint8_t code_index_number : 4;
  uint8_t cable_number : 4;
  uint8_t MIDI_0;
  uint8_t MIDI_1;
  uint8_t MIDI_2;
} UsbMidiMessage;

struct HdlcMidiMsgPkt {
    HdlcMessageType msgType;
    uint8_t         cable;
    uint8_t         data0;
    uint8_t         data1;
    uint8_t         data2;
};

struct HdlcMidiMsgPktRaw {
    uint8_t msgType;
    uint8_t cable;
    uint8_t data0;
    uint8_t data1;
    uint8_t data2;
};

HdlcMidiMsgPktRaw makeMidiMsgPktRaw(unsigned cable, unsigned data0, unsigned data1, unsigned data2);

HdlcMidiMsgPkt decodeMidiMsgPkt(uint8_t* buf, size_t bufferSize);
