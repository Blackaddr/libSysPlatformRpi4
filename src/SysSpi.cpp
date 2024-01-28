#include "sysPlatform/SysSpi.h"

namespace SysPlatform {

struct SysSpi::_impl {
    int dummy;
};

SysSpi::SysSpi(bool useDma)
: m_useDma(useDma), m_pimpl(std::make_unique<_impl>())
{

}

SysSpi::~SysSpi()
{

}

void SysSpi::begin()
{

}

void SysSpi::beginTransaction(SysSpiSettings settings)
{

}

uint8_t SysSpi::transfer(uint8_t data)
{
    return 0;
}

uint16_t SysSpi::transfer16(uint16_t data)
{
    return 0;
}

void SysSpi::transfer(void *buf, size_t count)
{

}

void SysSpi::endTransaction(void)
{

}

}