// src/core/d2client/d2client_2a80.cpp -- 1.13c D2Client.dll 0x2A80 (177 B), alpha lane, from the
// cached decompile build-wasm/ghidra-d2client/decomp/0x2A80.c. Base 0x6FAB0000.
//
// TSExplicitList<USGAMEDATA> resize/count setter. `self` is the object (unaff_ESI in the decompile;
// its argument register is not visible there), `count` arrives in EAX. Layout: [0] = stored count,
// [1] = previous element count, [2] = element array pointer, 12-byte elements.
//   - shrink: destroy elements [count, oldCount) through 0x2EC0, ascending.
//   - [0] = count.
//   - SMemReAlloc(oldArray, count*12, "__AV__TSExplicitList_USGAMEDATA__", -2, 0x10); if it returns
//     null, SMemAlloc(count*12, ...same file and line -2...). The result is stored at [2] either way.
// Unrecovered callees (0x2EC0) and Storm ordinals 405/401 are host seams after the retail arguments,
// in retail order (the d2client_batch_1de0.cpp pattern).
#include <cstdint>
#include <cstddef>
#include <emscripten.h>

struct D2Client2a80Env {
    void (*sub_2EC0)(uint32_t element);   // 0x2EC0  destructor of one 12-byte element
    uint32_t (*ordinal_405)(uint32_t ptr, uint32_t size, const char* file, int32_t line,
                            uint32_t flags);            // Storm ordinal 405 = SMemReAlloc
    uint32_t (*ordinal_401)(uint32_t size, const char* file, int32_t line); // Storm ordinal 401 = SMemAlloc
};

// 1.13c D2Client+0x02A80
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_2a80(void* self, uint32_t count,
                                         const D2Client2a80Env* env) {
    uint32_t* obj = (uint32_t*)self;
    uint32_t old_count = obj[1];
    uint32_t array = obj[2];
    if (count < old_count) {
        uint32_t elem = array + count * 0xC;
        uint32_t i = count;
        do {
            env->sub_2EC0(elem);
            ++i;
            elem += 0xC;
        } while (i < old_count);
    }
    obj[0] = count;
    uint32_t resized = env->ordinal_405(array, count * 0xC,
                                        "__AV__TSExplicitList_USGAMEDATA__", -2, 0x10);
    obj[2] = resized;
    if (resized == 0)
        obj[2] = env->ordinal_401(count * 0xC, "__AV__TSExplicitList_USGAMEDATA__", -2);
}
