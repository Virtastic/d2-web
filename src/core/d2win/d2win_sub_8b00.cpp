// src/core/d2win/d2win_sub_8b00.cpp -- 1.13c D2Win+0x8B00 (ordinal 10041), transcribed from the cached
// decompile. Alpha lane: implementation + marker only. Callees are seams: 0x8450 (list teardown),
// 0x17FC0 (release), Fog #10265/#10024 (error text/report).
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <emscripten.h>
struct D2WinFreeEnv {
    uint32_t (*errorText)(uint32_t);            // Fog #10265
    void (*reportError)(const char*, uint32_t); // Fog #10024
    void (*clearList)(void*);                   // 0x8450, result ignored
    uint32_t (*release)(void*);                 // 0x17FC0
};
// 1.13c D2Win+0x8B00 -- free a type-4 control: its +0x90 block, its lists, then itself.
extern "C" EMSCRIPTEN_KEEPALIVE void d2win_sub_8B00(void* p, const D2WinFreeEnv* e) {
    uint32_t line = 0xCB;
    if (p) {
        uint32_t type, blk;
        std::memcpy(&type, p, 4);
        if (type == 4) {
            std::memcpy(&blk, static_cast<uint8_t*>(p) + 0x90, 4);
            if (blk) e->release(reinterpret_cast<void*>(static_cast<uintptr_t>(blk)));
            e->clearList(p);
            e->release(p);
            return;
        }
        line = 0xCC;
    }
    e->reportError("", e->errorText(line));
    std::_Exit(-1);
}
