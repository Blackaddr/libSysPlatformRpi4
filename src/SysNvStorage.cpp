#include <cstdlib>
#include "globalInstances.h"
#include <sysPlatform/SysLogger.h>
//#include <sysPlatform/SysMutex.h>
#include <sysPlatform/SysNvStorage.h>

namespace SysPlatform {

constexpr char EEPROM_FILENAME[] = "eeprom.bin";
const size_t NVSTORAGE_SIZE_BYTES = 4096;
NvStorage sysNvStorage;

//static std::mutex g_mtx;

struct NvStorage::_impl {
    _impl() {}
    virtual ~_impl() {}
    uint8_t data[NVSTORAGE_SIZE_BYTES];
    bool isInitialized = false;
    unsigned fileHandle;

    bool init();
    bool writeFlushFile();
};

bool NvStorage::_impl::writeFlushFile()
{
    //std::lock_guard<std::mutex> lock(g_mtx);
    if (fileHandle != 0) {
        g_fileSystemPtr->FileClose(fileHandle);
        fileHandle = 0;
    }

    fileHandle = g_fileSystemPtr->FileCreate(EEPROM_FILENAME);
    {
        if (fileHandle == 0) {
            SYS_DEBUG_PRINT(sysLogger.printf("NvStorage::_impl::writeFlushFile(): unable to create EEPROM file: %d\n", fileHandle));
            return false;
        }
    }

    unsigned bytesWritten = g_fileSystemPtr->FileWrite(fileHandle, data, NVSTORAGE_SIZE_BYTES);
    if (bytesWritten != NVSTORAGE_SIZE_BYTES) {
        SYS_DEBUG_PRINT(sysLogger.printf("NvStorage::_impl::writeFlushFile(): unable to write init data to EEPROM file, returned %d\n", bytesWritten));
        g_fileSystemPtr->FileClose(fileHandle);
        return false;
    }

    if (g_fileSystemPtr->FileClose(fileHandle) == 0) {
        SYS_DEBUG_PRINT(sysLogger.printf("NvStorage::_impl::writeFlushFile(): unable to close EEPROM file\n"));
        return false;
    }
    fileHandle = 0;

    return true;
}

bool NvStorage::_impl::init()
{
    if (isInitialized) return true;
    if (!g_fileSystemPtr) return false;

    static bool retried = false;

    fileHandle = g_fileSystemPtr->FileOpen(EEPROM_FILENAME);

    if (fileHandle == 0) {  // unable to open file, so create it

        if (!writeFlushFile()) {
            SYS_DEBUG_PRINT(sysLogger.printf("NvStorage::_impl::init(): FAILED to write data to new EEPROM file\n"));
            return false;
        }
    } else { // file exists
        //std::lock_guard<std::mutex> lock(g_mtx);
        unsigned bytesRead = g_fileSystemPtr->FileRead(fileHandle, data, NVSTORAGE_SIZE_BYTES);
        if (bytesRead != NVSTORAGE_SIZE_BYTES) {
            SYS_DEBUG_PRINT(sysLogger.printf("NvStorage::_impl::init(): unable to read data from EEPROM file\n"));

            // delete the file it's probably invalid and try once more before failing
            g_fileSystemPtr->FileClose(fileHandle);
            fileHandle = 0;
            g_fileSystemPtr->FileDelete(EEPROM_FILENAME);

            // try one more time
            if (!retried) {
                retried = true;
                return init();
            }
        }
    }

    if (fileHandle) {
        g_fileSystemPtr->FileClose(fileHandle);
        fileHandle = 0;
    }
    isInitialized = true;
    return isInitialized;
}

NvStorage::NvStorage()
: m_pimpl(std::make_shared<_impl>())
{
    //m_pimpl->data = (uint8_t*)malloc(NVSTORAGE_SIZE_BYTES);
    // if (m_pimpl->data) {
    memset(m_pimpl->data, 0, NVSTORAGE_SIZE_BYTES);
    // }
}

NvStorage::~NvStorage()
{
    //if (m_pimpl->data) free(m_pimpl->data);
}

uint8_t NvStorage::read( int idx ) {
    if (!m_pimpl->isInitialized) { if (!m_pimpl->init()) { return 0; } }
    return m_pimpl->data[idx];
}

void NvStorage::write( int idx, uint8_t val ) {
    if (!m_pimpl->isInitialized) { if (!m_pimpl->init()) { return; } }
    m_pimpl->data[idx] = val;

}

void NvStorage::update( int idx, uint8_t in) {
    write(idx, in);
}

void NvStorage::flush()
{
    if (!m_pimpl->isInitialized) { if (!m_pimpl->init()) { return; } }
    m_pimpl->writeFlushFile();
}

}