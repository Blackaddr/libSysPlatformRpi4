#include "circle/logger.h"
#include "circle/sched/scheduler.h"
#include "SPI.h"

SPIClass SPI;

constexpr size_t BUFFER_SIZE = 512;

static DMA_BUFFER (u8, TxData, BUFFER_SIZE);
static DMA_BUFFER (u8, RxBuffer, BUFFER_SIZE);

bool SPIClass::isBusy()
{
    return m_checkXferInProgress();
}

void SPICompletionRoutine (boolean bStatus, void *pParam)
{
	// CKernel *pThis = (CKernel *) pParam;
	// assert (pThis != 0);
    SPIClass *pThis = (SPIClass *) pParam;

	if (!bStatus)
	{
		CLogger::Get ()->Write ("SPICompletionRoutine", LogError, "SPI transfer error");
	} else {
        CLogger::Get ()->Write ("SPICompletionRoutine", LogNotice, "SPI transfer complete");
    }

    pThis->m_clearXferInProgress();
}

void SPIClass::begin() {
    //m_spiDma->SetCompletionRoutine (SPICompletionRoutine, this);
    m_clearXferInProgress();
}
bool SPIClass::beginTransaction(SPISettings&) {
    if (isBusy()) { CLogger::Get ()->Write ("beginTransaction()", LogError, "SPI transfer is busy"); return false; }
    return true;
}

void SPIClass::endTransaction() {

}

bool SPIClass::m_checkXferInProgress()
{
    m_mutex.Acquire();
    bool busy = m_transferInProgress;
    m_mutex.Release();
    return busy;
}

void SPIClass::m_setXferInProgress()
{
    m_mutex.Acquire();
    m_transferInProgress = true;
    m_mutex.Release();
}

void SPIClass::m_clearXferInProgress()
{
    m_mutex.Acquire();
    m_transferInProgress = false;
    m_mutex.Release();
}

uint8_t SPIClass::transfer(uint8_t data) {
    if (!m_spiDma) { return 0; }
    TxData[0] = data;
    m_setXferInProgress();
    //m_spiDma->StartWriteRead (SPI_CHIP_SELECT, TxData, RxBuffer, 1);
    m_spiDma->WriteRead(0, TxData, RxBuffer, 1);
    m_clearXferInProgress();
    return RxBuffer[0];
}

void SPIClass::transfer(uint8_t* buf, size_t size) {
    if (!m_spiDma) { return; }
    memcpy(TxData, buf, size);
    m_setXferInProgress();
    //m_spiDma->StartWriteRead (SPI_CHIP_SELECT, TxData, RxBuffer, size);
    m_spiDma->WriteRead(0, TxData, RxBuffer, size);
    m_clearXferInProgress();
    return;
}
