// src/core/d2win/d2win_sub_5750.cpp -- 1.13c D2Win+0x5750, transcribed from the cached decompile.
// Maps a Windows code page (passed in EAX) to a LANGID.
#include <cstdint>
#include <emscripten.h>
// 1.13c D2Win+0x5750
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2win_sub_5750(int32_t codepage) {
    switch (codepage) {
    case 0x3a4: return 0x411;
    case 0x3a8: return 0x804;
    case 0x3b5: return 0x412;
    case 0x3b6: return 0x404;
    }
    return 0;
}
