// src/core/d2game/d2game_batch_4385.cpp -- 1.13c D2Game.dll row 0x04385 write_multi_char, from the
// cached decompile (build-wasm/ghidra-d2game/decomp/0x4385.c). Alpha lane (OBJECTIVE v8.3):
// implementation + marker only, no spec, no vector.
//
// MSVC 2003 Release CRT _write_multi_char (output.c): emit `num` copies of ch, stopping after the write
// that leaves the running count at -1. The retail (cdecl) arguments are (ch, num); the stream and the
// running count reach the helper in ECX/ESI in the CRT's internal register-passing, so they follow as
// host context. The one callee _write_char is already recovered in src/core/d2game/crt_write.cpp (0x4352).
#include <cstdint>
#include <emscripten.h>

// MSVC 2003 32-bit FILE (VS2003 layout), as defined in crt_write.cpp.
struct CrtFile32 {
    uint32_t ptr;   // 0x00
    int32_t  cnt;   // 0x04
    uint32_t base;  // 0x08
    uint32_t flag;  // 0x0C
};

extern "C" void d2game_write_char(uint8_t ch, CrtFile32* f, int32_t* pCount);

// (marker lives on the probe, src/core/fn/d2game_4385.cpp)
extern "C" void d2game_write_multi_char(uint8_t ch, int32_t num, CrtFile32* f, int32_t* pCount) {
    do {
        if (num < 1) return;
        num--;
        d2game_write_char(ch, f, pCount);
    } while (*pCount != -1);   // the -1 check sits at the loop bottom in retail
}
