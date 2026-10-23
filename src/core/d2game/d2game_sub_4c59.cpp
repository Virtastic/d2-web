#include <cstdint>
#include <emscripten.h>

// CRT __lock_file. file is the retail 32-bit FILE address. Callbacks model
// __lock(n) and EnterCriticalSection(file+4).
// 1.13c D2Game+0x04C59
extern "C" EMSCRIPTEN_KEEPALIVE void d2game_lock_file_4c59(
    uint32_t file, void (*lock)(int32_t), void (*enter_cs)(uint32_t)) {
    if (file > 0x6fd1f334u + 3u && file < 0x6fd1f599u) {
        // _File[-0x37e8f9a] is a 32-byte FILE step: (file - 0x6fd1f334) >> 5 + 0x10
        lock((int32_t)((file - 0x6fd1f334u) >> 5) + 0x10);
        return;
    }
    enter_cs(file + 0x20);
}
