#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <sysPlatform/SysDebugPrint.h>
#include <sysPlatform/SysLogger.h>
#include "sysPlatform/SysSpi.h"

namespace SysPlatform {

const     size_t SYS_SPI_MEM_SIZE = 8*1024*1024; // 8 MiB
constexpr size_t MAX_BUFFER_SIZE = 256*2*4;      // Two audio blocks at 256 samples, float values

struct SysSpi::_impl {
    uint8_t* mem               = nullptr;
    uint8_t* buffer            = nullptr;
    size_t   dmaBufferCopySize = 0;
    bool     isStarted         = false;
};

SysSpi::SysSpi(bool useDma)
: m_useDma(useDma), m_pimpl(std::make_unique<_impl>())
{

}

SysSpi::~SysSpi()
{
    if (m_pimpl->mem) free(m_pimpl->mem);
}

void SysSpi::begin()
{
    m_pimpl->mem    = (uint8_t*)malloc(SYS_SPI_MEM_SIZE);
    m_pimpl->buffer = (uint8_t*)malloc(MAX_BUFFER_SIZE);
    if (!m_pimpl->mem || !m_pimpl->buffer) {
        SYS_DEBUG_PRINT(sysLogger.printf("SysSpi::begin(): error, unble to allocate SPI mem model"));
    }
    m_pimpl->isStarted = true;
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
    if (!m_pimpl->mem || !m_pimpl->isStarted) { return; }
    if (address < SYS_SPI_MEM_SIZE) {
        m_pimpl->mem[address] = data;
    }
}

void SysSpi::write(size_t address, uint8_t *src, size_t numBytes)
{
    if (!m_pimpl->mem || !m_pimpl->isStarted) { return; }
    if (address + numBytes < SYS_SPI_MEM_SIZE) {
        memcpy((void*)&m_pimpl->mem[address], src, numBytes);
    }
}

void SysSpi::zero(size_t address, size_t numBytes)
{
    if (!m_pimpl->mem || !m_pimpl->isStarted) { return; }
    write(address, 0);
}


void SysSpi::write16(size_t address, uint16_t data)
{
    if (!m_pimpl->mem || !m_pimpl->isStarted) { return; }
    write(address, reinterpret_cast<uint8_t*>(&data), sizeof(uint16_t));
}

void SysSpi::write16(size_t address, uint16_t *src, size_t numWords)
{
	write(address, reinterpret_cast<uint8_t*>(src), sizeof(uint16_t)*numWords);
}

void SysSpi::zero16(size_t address, size_t numWords)
{
	zero(address, sizeof(uint16_t)*numWords);
}

uint8_t SysSpi::read(size_t address)
{
    if (!m_pimpl->mem || !m_pimpl->isStarted) { return 0; }
    if (address < SYS_SPI_MEM_SIZE) {
        return m_pimpl->mem[address];
    }
}

void SysSpi::read(size_t address, uint8_t *dest, size_t numBytes)
{
    if (!m_pimpl->mem || !m_pimpl->isStarted) { return; }

    if (address + numBytes < SYS_SPI_MEM_SIZE) {
        if (m_pimpl->dmaBufferCopySize > 0) {
            memcpy((void*)m_pimpl->buffer, (void*)&m_pimpl->mem[address], numBytes);
        }
        memcpy((void*)dest, (void*)&m_pimpl->mem[address], numBytes);
    }
}

uint16_t SysSpi::read16(size_t address)
{
    uint16_t value;
	read(address, reinterpret_cast<uint8_t*>(&value), sizeof(uint16_t));
    return value;
}

void SysSpi::read16(size_t address, uint16_t *dest, size_t numWords)
{
    if (!m_pimpl->mem || !m_pimpl->isStarted) { return; }
    read(address, reinterpret_cast<uint8_t*>(dest), sizeof(uint16_t)*numWords);
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
    if (!m_pimpl->mem || !m_pimpl->isStarted) { return; }
    if (!m_pimpl->buffer) { return; }
    memcpy((void*)dest, (void*)&m_pimpl->buffer[byteOffset], numBytes);
}

bool SysSpi::setDmaCopyBufferSize(size_t numBytes)
{
    m_pimpl->dmaBufferCopySize = numBytes;
    return true;
}

size_t SysSpi::getDmaCopyBufferSize(void)
{
    return m_pimpl->dmaBufferCopySize;
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
    m_pimpl->isStarted = false;
}

void SysSpi::start(bool waitForStart)
{
    m_pimpl->isStarted = true;
}

}
