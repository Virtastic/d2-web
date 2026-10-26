// 1.13c Storm+0x2890/0x2950/0x29E0/0x3AE0/0x3BA0: {cap,len,buf} array resize / byte-array assign.
// Allocator seams are explicit (Storm+0x1C760 realloc, 0x1C580 alloc, 0x1B960 free), as in sub_3D40.
#include <cstdint>
#include <cstring>

struct StormArrEnv {
    uint8_t* (*realloc_)(uint8_t* p, uint32_t n, const char* tag, int line, uint32_t flags);
    uint8_t* (*alloc)(uint32_t n, const char* tag, int line, uint32_t flags);
    void (*free_)(uint8_t* p, const char* tag, int line);
};
struct StormArr { uint32_t cap, len; uint8_t* buf; };

// shared body of 2890 (0x1C, "__AU_SOURCE__"), 2950 (0x10, "__AURECTF__"), 29E0 (8, "__AU_FOUNDPARAM__")
static void resize(StormArrEnv& e, StormArr* a, uint32_t n, uint32_t esz, const char* tag) {
    uint8_t* old = a->buf;
    a->cap = n;
    a->buf = e.realloc_(old, n * esz, tag, -2, 0x10);
    if (a->buf) return;
    a->buf = e.alloc(n * esz, tag, -2, 0);
    if (!old) return;
    uint32_t m = a->len < n ? a->len : n;
    if (a->buf) memcpy(a->buf, old, m * esz);
    e.free_(old, tag, -2);
}
// 1.13c Storm+0x2890 (EAX=n, EBX=a)
extern "C" void Storm_sub_2890(StormArrEnv& e, StormArr* a, uint32_t n) { resize(e, a, n, 0x1C, "__AU_SOURCE__"); }
// 1.13c Storm+0x2950 (stack=n, EDI=a)
extern "C" void Storm_sub_2950(StormArrEnv& e, StormArr* a, uint32_t n) { resize(e, a, n, 0x10, "__AURECTF__"); }
// 1.13c Storm+0x29E0 (stack=n, ESI=a)
extern "C" void Storm_sub_29E0(StormArrEnv& e, StormArr* a, uint32_t n) { resize(e, a, n, 8, "__AU_FOUNDPARAM__"); }

static void assign(StormArrEnv& e, StormArr* a, uint32_t n, const uint8_t* src, const char* tag) {
    a->cap = n;
    if (a->buf || n) a->buf = e.realloc_(a->buf, n, tag, -2, 0);
    for (uint32_t i = 0; i < n; i++) if (a->buf) a->buf[i] = src[i];
    a->len = n;
}
// 1.13c Storm+0x3AE0 (ESI=n, EDI=a, stack=src)
extern "C" void Storm_sub_3AE0(StormArrEnv& e, StormArr* a, uint32_t n, const uint8_t* src) { assign(e, a, n, src, "_E"); }
// 1.13c Storm+0x3BA0 (ESI=n, EDI=a, stack=src)
extern "C" void Storm_sub_3BA0(StormArrEnv& e, StormArr* a, uint32_t n, const uint8_t* src) { assign(e, a, n, src, "_D"); }
