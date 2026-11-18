// src/core/d2client/d2client_2a80.cpp -- 1.13c D2Client.dll 0x2A80 (177 B). Base 0x6FAB0000.
// Read from the instruction bytes (the cached decompile drops the realloc-failed copy path).
// TSExplicitList<USGAMEDATA> resize. EAX = new count, ESI = list; [0] count, [1] old count, [2] array,
// 12-byte elements. Callees are host seams after the retail args: 2EC0(elem) dtor, 2B40 (ecx=old elem,
// arg=new slot) copy, Storm 405 realloc / 401 alloc / 403 free.
#include <cstdint>
#include <emscripten.h>

struct D2Client2a80Env {
    void (*sub_2EC0)(uint32_t element);
    uint32_t (*realloc405)(uint32_t ptr, uint32_t size, const char* file, int32_t line, uint32_t flags);
    uint32_t (*alloc401)(uint32_t size, const char* file, int32_t line, uint32_t zero);
    void (*sub_2B40)(uint32_t oldElem, uint32_t newSlot);
    void (*free403)(uint32_t ptr, const char* file, int32_t line, uint32_t zero);
};

// 1.13c D2Client+0x02A80
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_2a80(void* self, uint32_t count, const D2Client2a80Env* env) {
    static const char* const kName = "__AV__TSExplicitList_USGAMEDATA__";
    uint32_t* obj = (uint32_t*)self;
    uint32_t old = obj[2];
    if (count < obj[1]) {
        uint32_t e = old + count * 0xC;
        for (uint32_t i = count; ; e += 0xC) {
            env->sub_2EC0(e);
            if (!(++i < obj[1])) break;
        }
    }
    uint32_t bytes = count * 0xC;
    obj[0] = count;
    obj[2] = env->realloc405(old, bytes, kName, -2, 0x10);
    if (obj[2] != 0) return;
    obj[2] = env->alloc401(bytes, kName, -2, 0);
    if (old == 0) return;
    uint32_t n = obj[1];
    if (count < n) n = count;
    uint32_t src = old;
    for (uint32_t off = 0; n; --n, off += 0xC, src += 0xC) {
        env->sub_2B40(src, obj[2] + off);
        env->sub_2EC0(src);
    }
    env->free403(old, kName, -2, 0);
}
