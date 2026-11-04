// 1.13c D2sound+0x6070 -- read exactly `size` bytes of an open file into `buf` (register inputs ESI = file handle, EDI = expected size,
// one stack word = destination buffer; ret 4). Every failure is the fatal path (a message id through Ordinal_10265 / Ordinal_10024, then
// _exit(-1)), so the only observable of a surviving call is the Fog read (Ordinal_10104 thunk 0x5FD2).
#include <cstdint>
#include <unistd.h>

struct D2sound6070Env {
    // 0x5FD2: ECX = hFile, EDX = buf, stack (size, &bytesRead, 0, 0, 0), ret 0x14; 0 = failed
    int32_t (*read)(uint32_t hFile, uint32_t buf, uint32_t size, uint32_t* bytesRead);
};

// 1.13c D2sound+0x6070
void D2sound_b68_6070(uint32_t hFile, uint32_t size, uint32_t buf, const D2sound6070Env* e) {
    uint32_t got = 0;
    if (!hFile) _exit(-1);                      // id 0x87
    if (!e->read(hFile, buf, size, &got)) _exit(-1);   // Ordinal_276 then id 0x8F
    if (size != got) _exit(-1);                 // id 0x92
}
