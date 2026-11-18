// src/core/storm/storm_3400.cpp -- 1.13c Storm+0x3400 (StormExplicitListRealloc_3400). Env-injected: the retail callees are
// 0x1B80 (element dtor), 0x34C0 (element copy, thiscall), 0x1C760 (realloc), 0x1C580 (alloc) and 0x1B960 (free).
// Layout: list [0] capacity, [1] used count, [2] data pointer; 12-byte elements.
#include <cstdint>

struct Storm3400Env {
    void (*dtor)(void* elem);                                                                   // 0x1B80
    void (*copy)(void* dst, const void* src);                                                   // 0x34C0 (thiscall, ECX = dst)
    void* (*realloc_)(void* p, uint32_t size, const char* tag, int32_t line, uint32_t flags);  // 0x1C760
    void* (*alloc)(uint32_t size, const char* tag, int32_t line, uint32_t flags);              // 0x1C580
    void (*free_)(void* p, const char* tag, int32_t line);                                      // 0x1B960
};

static const char kTag3400[] = ".?AV?$TSExplicitList@URGN@@$0?CCCCCCD@@";

// 1.13c Storm+0x3400: EAX = new count n, ESI = list. Returns nothing (retail keeps the list in ESI).
extern "C" void Storm_Sub3400(const Storm3400Env& e, uint32_t* list, uint32_t n) {
    uint8_t* data = (uint8_t*)(uintptr_t)list[2];
    uint32_t used = list[1];
    list[0] = n;
    if (n < used)
        for (uint32_t i = n; i < used; i++) e.dtor(data + i * 12);
    void* np = e.realloc_(data, n * 12, kTag3400, -2, 0x10);
    list[2] = (uint32_t)(uintptr_t)np;
    if (!np) {
        np = e.alloc(n * 12, kTag3400, -2, 0);
        list[2] = (uint32_t)(uintptr_t)np;
        if (data) {
            uint32_t m = n < used ? n : used;
            for (uint32_t i = 0; i < m; i++) {
                e.copy((uint8_t*)np + i * 12, data + i * 12);
                e.dtor(data + i * 12);
            }
            e.free_(data, kTag3400, -2);
        }
    }
}
