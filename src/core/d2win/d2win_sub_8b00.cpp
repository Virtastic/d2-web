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
// FN probe glue (checked against retail): hooked 0x8450 and 0x17FC0 log (rva, ecx=token) into lg; both return 0.
static uint32_t* fn8bLg; static void* fn8bP;
static void fn8bLog(uint32_t rva, uint32_t tok) {
    uint32_t* w = fn8bLg + 1 + fn8bLg[0]++ * 10;
    w[0] = rva; w[1] = 0; w[2] = tok; for (int i = 3; i < 10; ++i) w[i] = 0;
}
static void fn8bClear(void*) { fn8bLog(0x8450, 0xB1000000u); }
static uint32_t fn8bRelease(void* q) { fn8bLog(0x17FC0, q == fn8bP ? 0xB1000000u : 0xB1010000u); return 0; }
extern "C" uint32_t d2win_sub_8B00_fn(void* p, uint32_t* lg) {
    fn8bLg = lg; lg[0] = 0; fn8bP = p;
    D2WinFreeEnv e = {}; e.clearList = fn8bClear; e.release = fn8bRelease;
    d2win_sub_8B00(p, &e);
    return 0;
}
