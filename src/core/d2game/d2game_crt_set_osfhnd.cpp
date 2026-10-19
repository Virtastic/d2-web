// Retail CRT handle installation; shares the explicit state model of __free_osfhnd.
#include <cstdint>
#include <emscripten.h>

struct D2GameCrtSetOsfhndEnv {
    int32_t* (*errnoPtr)();
    uint32_t* (*doserrnoPtr)();
    int32_t (*setStdHandle)(uint32_t, uint32_t);
    uint32_t osplatform; // retail 0x10FD78
    uint32_t nhandle;    // retail 0x11A214
    uint8_t** pioinfo;   // retail 0x11A220; blocks of 32 records, stride 0x24
};

// 1.13c D2Game+0x0879F
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2game_set_osfhnd(
    int32_t fileHandle, uint32_t osHandle, const D2GameCrtSetOsfhndEnv* s) {
    const uint32_t fd = static_cast<uint32_t>(fileHandle);
    if (fd < s->nhandle) {
        uint8_t* io = s->pioinfo[fd >> 5] + (fd & 31u) * 0x24;
        int32_t previous;
        __builtin_memcpy(&previous, io, sizeof(previous));
        if (previous == -1) {
            if (s->osplatform == 1 && fd <= 2)
                s->setStdHandle(0xfffffff6u - fd, osHandle);
            // Retail ignores SetStdHandle's return, including failure.
            __builtin_memcpy(io, &osHandle, sizeof(osHandle));
            return 0;
        }
    }
    *s->errnoPtr() = 9;
    *s->doserrnoPtr() = 0;
    return -1;
}
