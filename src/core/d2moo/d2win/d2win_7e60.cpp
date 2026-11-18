// 1.13c D2Win+0x7E60 -- sub_7E60.
// Stdcall, ret 0x14 (five stack words). Word 1 = name, word 3 = EAX flag for #7DC0 (the retry callback is word 4).
// Allocates a 0x108-byte holder (Fog alloc, file 0x6f8fd150, line 0x21A), stores #7DC0's result at holder+0, and retries every
// 100 ms while the callback returns nonzero. On failure it frees the holder (Fog free 0x22C) and returns null.
#include <cstdint>

struct D2WinSub7e60Env {
    int32_t* (*fogAlloc)(int32_t size, int32_t line, int32_t flag);   // Fog alloc of the holder (ECX 0x108, EDX file, stack line 0x21A, 0)
    void (*fogFree)(int32_t* p, int32_t line, int32_t flag);   // Fog free of the holder (ECX holder, EDX file, stack line 0x22C, 0)
    char* (*sub_7dc0)(char* buf, uint32_t flag, const void* name);   // 0x7DC0 (buf = holder + 4)
    void (*sleepMs)(uint32_t ms);                              // Sleep
};

extern "C" int32_t* d2win_7E60(const void* name, uint32_t flag, int32_t (*retry)(void), const D2WinSub7e60Env* e) {
    int32_t* h = e->fogAlloc(0x108, 0x21a, 0);
    h[0] = 0;
    for (;;) {
        h[0] = (int32_t)(intptr_t)e->sub_7dc0((char*)h + 4, flag, name);
        if (h[0] != 0) return h;
        if (retry == nullptr || retry() == 0) break;
        e->sleepMs(100);
        if (h[0] != 0) return h;
    }
    e->fogFree(h, 0x22c, 0);
    return nullptr;
}
