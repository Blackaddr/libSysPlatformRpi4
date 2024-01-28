#include <cstdlib>
#include <sysPlatform/SysNvStorage.h>

namespace SysPlatform {

const size_t NVSTORAGE_SIZE_BYTES = 4096;
NvStorage sysNvStorage;

NvStorage::NvStorage()
{
    data = (uint8_t*)malloc(NVSTORAGE_SIZE_BYTES);
}

NvStorage::~NvStorage()
{
    if (data) free(data);
}

uint8_t NvStorage::read( int idx ) {
    return data[idx];
}

void NvStorage::write( int idx, uint8_t val ) {
    data[idx] = val;
}

void NvStorage::update( int idx, uint8_t in) {
    write(idx, in);
}

}