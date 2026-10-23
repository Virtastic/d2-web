// 1.13c D2Win+0x07E60 : allocate a 0x21A-byte holder, resolve a path into holder+4 (sub_7DC0),
// retrying every 100 ms while the optional callback returns nonzero.
#include <cstdint>
#include <emscripten.h>

struct D2WinSub7e60Env {
    int32_t* (*fogAlloc)(int32_t size, int32_t line);      // Ordinal_10042(0x21a,0)
    void (*fogFree)(int32_t line, int32_t flag);           // Ordinal_10043(0x22c,0)
    char* (*sub_7dc0)(void* buf, const void* name);        // 0x7DC0 (thiscall: buf in ecx)
    void (*sleepMs)(uint32_t ms);
};

// 1.13c D2Win+0x07E60
// name = the stack word the decompile shows as unaff_retaddr; retry = param_3.
extern "C" EMSCRIPTEN_KEEPALIVE
int32_t* d2win_sub_7e60(const void* name, int32_t (*retry)(), const D2WinSub7e60Env* e) {
    int32_t* h = e->fogAlloc(0x21a, 0);
    *h = 0;
    for (;;) {
        *h = (int32_t)(intptr_t)e->sub_7dc0(h + 1, name);
        if (*h) return h;
        if (!retry || retry() == 0) break;
        e->sleepMs(100);
        if (*h) return h;
    }
    e->fogFree(0x22c, 0);
    return nullptr;
}
