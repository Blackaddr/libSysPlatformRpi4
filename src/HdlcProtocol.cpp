#include "globalInstances.h"
#include "HdlcProtocol.h"

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

HdlcMidiMsgPktRaw makeMidiMsgPktRaw(unsigned cable, unsigned data0, unsigned data1, unsigned data2)
{
  HdlcMidiMsgPktRaw pkt;
  pkt.msgType  = static_cast<uint8_t>(HdlcMessageType::MIDI_MSG);
  pkt.cable    = cable;
  pkt.data0    = data0;
  pkt.data1    = data1;
  pkt.data2    = data2;

  return pkt;
}

HdlcMidiMsgPkt decodeMidiMsgPkt(uint8_t* buf, size_t bufferSize)
{
  // Create a pkt initalized to invalid
  HdlcMidiMsgPkt pkt = {HdlcMessageType::INVALID, 0, 0, 0, 0};
  if (bufferSize < sizeof(HdlcMidiMsgPktRaw)) { return pkt; }

  pkt.msgType = static_cast<HdlcMessageType>(buf[0]);
  pkt.cable   = static_cast<uint8_t>(buf[1]);
  pkt.data0   = static_cast<uint8_t>(buf[2]);
  pkt.data1   = static_cast<uint8_t>(buf[3]);
  pkt.data2   = static_cast<uint8_t>(buf[4]);

  return pkt;
}
