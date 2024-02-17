#pragma once

#include <stdint.h>
#include <string.h>
#include <circle/spimasterdma.h>
#include <circle/spimaster.h>
#include <circle/sched/mutex.h>

#define SPI_CLOCK_SPEED		20000000		// Hz
#define SPI_CPOL		0
#define SPI_CPHA		0
#define SPI_CHIP_SELECT		0		// 0 or

#ifndef LSBFIRST
#define LSBFIRST 0
#endif
#ifndef MSBFIRST
#define MSBFIRST 1
#endif

typedef enum _BitOrder {
  SPI_BITORDER_MSBFIRST = MSBFIRST,
  SPI_BITORDER_LSBFIRST = LSBFIRST,
} BusIOBitOrder;

#define SPI_MODE0 0x00
#define SPI_MODE1 0x04
#define SPI_MODE2 0x08
#define SPI_MODE3 0x0C


class SPISettings {
public:
    SPISettings(uint32_t clock, uint8_t bitOrder, uint8_t dataMode) {}
};

#define CSPI CSPIMaster

class SPIClass {
public:
    void begin();
    bool beginTransaction(SPISettings&);
    void endTransaction();
    uint8_t transfer(uint8_t);
    void transfer(uint8_t* buf, size_t size);
    void setRawSpi(CSPI* ptr) { m_spiDma = ptr; }

    bool isBusy();
    void m_clearXferInProgress();

    volatile bool m_transferInProgress = false;
    CMutex m_mutex;

private:
    CSPI* m_spiDma = nullptr;    
    void m_setXferInProgress();
    
    bool m_checkXferInProgress();

};

extern SPIClass SPI;
