#include "sysPlatform/SysOTP.h"

namespace SysPlatform {

// TODO replace with data from fuses.tst
static uint32_t HW_OCOTP_GP1    = 0x74F9365FU;
static uint32_t HW_OCOTP_GP2    = 0x3049BEE1U;
static uint32_t HW_OCOTP_CFG0   = 0x65F82969U;
static uint32_t HW_OCOTP_CFG1   = 0x300351D2U;
static uint32_t HW_OCOTP_MAC0   = 0xE50CF490U;
static uint32_t HW_OCOTP_MAC1   = 0x000004E9U;
static uint32_t HW_OCOTP_SW_GP1 = 0x00000202U;
static uint32_t HW_OCOTP_0x6B0  = 0xD322019DU;
static uint32_t HW_OCOTP_0x6C0  = 0x97EB491BU;
static uint32_t HW_OCOTP_0x690  = 0x129E599FU;
static uint32_t HW_OCOTP_0x6A0  = 0xBF7DDC17U;
static uint32_t HW_OCOTP_LOCK   = 0x4073BD07U;

uint64_t SysOTP::getUidLower()
{
    return ((uint64_t)HW_OCOTP_CFG1 << 32) + (uint64_t)HW_OCOTP_CFG0;
}

uint64_t SysOTP::getUidUpper()
{
    return ((uint64_t)HW_OCOTP_MAC1 << 32) + (uint64_t)HW_OCOTP_MAC0;
}

uint32_t SysOTP::getDevicePBKHLower()
{
    return HW_OCOTP_GP1;
}

uint32_t SysOTP::getDevicePBKHUpper()
{
    return HW_OCOTP_GP2;
}

uint32_t SysOTP::getDevelPBKHLower()
{
    return HW_OCOTP_0x690;
}
uint32_t SysOTP::getDevelPBKHUpper()
{
    return HW_OCOTP_0x6A0;
}

uint32_t SysOTP::getEUIDHLower()
{
    return HW_OCOTP_0x6B0;
}
uint32_t SysOTP::getEUIDHUpper()
{
    return HW_OCOTP_0x6C0;
}

uint16_t SysOTP::getProductId()
{
    return (HW_OCOTP_SW_GP1) & 0xFFFFU;
}

uint32_t SysOTP::getLocks()
{
    return HW_OCOTP_LOCK;
}

}
