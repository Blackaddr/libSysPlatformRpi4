#pragma once

#include <cstdint>

enum class HdlcMessageType : uint8_t {
    INVALID = 0,
    PHYSICAL_CONTROL,
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
    LED_GET_VALUE
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
    HdlcPhysicalControlType controlType, HdlcPhysicalControlActions controlAction, unsigned controlId, float controlValue)
{
  HdlcPhysicalControlPktRaw pkt;
  pkt.msgType       = static_cast<uint8_t>(HdlcMessageType::PHYSICAL_CONTROL);
  pkt.controlType   = static_cast<uint8_t>(controlType);
  pkt.controlAction = static_cast<uint8_t>(controlAction);
  pkt.controlId     = static_cast<uint8_t>(controlId);
  pkt.controlValue  = controlValue;

  return pkt;
}

HdlcPhysicalControlPkt decodePhysicalControlPkt(uint8_t* buf, size_t bufferSize)
{
  // Create a pkt initalized to invalid
  HdlcPhysicalControlPkt pkt = {HdlcMessageType::INVALID, HdlcPhysicalControlType::INVALID,
                       HdlcPhysicalControlActions::INVALID, 0};
  if (bufferSize < sizeof(HdlcPhysicalControlPktRaw)) { return pkt; }

  pkt.msgType       = static_cast<HdlcMessageType>(buf[0]);
  pkt.controlType   = static_cast<HdlcPhysicalControlType>(buf[1]);
  pkt.controlAction = static_cast<HdlcPhysicalControlActions>(buf[2]);
  pkt.controlId     = static_cast<uint8_t>(buf[3]);
  memcpy(&pkt.controlValue, &buf[4], sizeof(float));

  return pkt;
}

