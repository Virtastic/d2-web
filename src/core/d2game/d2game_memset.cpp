#include <cstddef>

extern "C" void* d2ddraw_memset(void* dst, int val, size_t size);

// 1.13c D2Game+0x07400 (_memset): identical CRT byte-fill routine to D2DDraw+0x04380.
extern "C" void* d2game_memset(void* dst, int val, size_t size) {
    return d2ddraw_memset(dst, val, size);
}
