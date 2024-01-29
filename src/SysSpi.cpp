#include "sysPlatform/SysSpi.h"

namespace SysPlatform {

const size_t SYS_SPI_MEM_SIZE = 0;

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

void SysSpi::write(size_t address, uint8_t data)
{

}

void SysSpi::write(size_t address, uint8_t *src, size_t numBytes)
{

}

void SysSpi::zero(size_t address, size_t numBytes)
{

}


void SysSpi::write16(size_t address, uint16_t data)
{

}

void SysSpi::write16(size_t address, uint16_t *src, size_t numWords)
{

}

void SysSpi::zero16(size_t address, size_t numWords)
{

}

uint8_t SysSpi::read(size_t address)
{
    return 0;
}

void SysSpi::read(size_t address, uint8_t *dest, size_t numBytes)
{

}

uint16_t SysSpi::read16(size_t address)
{
    return 0;
}

void SysSpi::read16(size_t address, uint16_t *dest, size_t numWords)
{

}

bool SysSpi::isWriteBusy() const
{
    return false;
}

bool SysSpi::isReadBusy() const
{
    return false;
}

void SysSpi::readBufferContents(uint8_t *dest,  size_t numBytes, size_t byteOffset)
{

}


void readBufferContents(uint16_t *dest, size_t numWords, size_t wordOffset)
{

}

bool SysSpi::setDmaCopyBufferSize(size_t numBytes)
{
    return false;
}

size_t SysSpi::getDmaCopyBufferSize(void)
{
    return 0;
}

bool SysSpi::isStarted() const
{
    return false;
}

bool SysSpi::isStopped() const
{
    return false;
}

void SysSpi::stop(bool waitForStop)
{

}

void SysSpi::start(bool waitForStart)
{

}

}
