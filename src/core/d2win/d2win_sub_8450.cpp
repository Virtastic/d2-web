// src/core/d2win/d2win_sub_8450.cpp -- 1.13c D2Win+0x8450 (ordinal 10106), transcribed from the cached
// decompile. Seams: Fog #10265/#10024 (error text/report), Fog #10043 (free(ptr,line); ptr is the
// fastcall ECX the decompiler dropped).
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <emscripten.h>
struct D2WinListEnv {
    uint32_t (*errorText)(uint32_t);
    void (*reportError)(const char*, uint32_t);
    void (*freeMem)(uint32_t ptr, uint32_t line);
    uint8_t* (*at)(uint32_t addr); // guest address -> host pointer
};
// 1.13c D2Win+0x8450 -- free a type-4 control's node list (5 ptrs + next@+0x1C per node), zero head fields.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2win_sub_8450(uint8_t* p, const D2WinListEnv* e) {
    if (!p) return 0;
    uint32_t v;
    std::memcpy(&v, p, 4);
    if (v != 4) {
        e->reportError("", e->errorText(0xDE));
        std::_Exit(-1);
    }
    std::memcpy(&v, p + 0x48, 4);
    while (v) {
        uint8_t* n = e->at(v);
        uint32_t next, q;
        std::memcpy(&next, n + 0x1C, 4);
        for (int i = 0; i < 5; i++) {
            std::memcpy(&q, n + i * 4, 4);
            if (q) e->freeMem(q, 0xE7);
        }
        e->freeMem(v, 0xE9);
        v = next;
    }
    std::memset(p + 0x48, 0, 0x10); // +0x48,+0x4C,+0x50,+0x54 (idx 0x12,0x13,0x14,0x15)
    std::memset(p + 0x58, 0, 4);    // idx 0x16
    return 1;
}
