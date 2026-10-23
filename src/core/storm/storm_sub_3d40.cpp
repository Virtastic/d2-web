// 1.13c Storm+0x3D40: resize a {cap,len,buf} buffer; realloc in place, else alloc+copy+free.
#include <cstdint>

struct StormBufEnv {
    uint8_t* (*realloc_)(uint8_t* p, uint32_t n, const char* file, int line, uint32_t flags); // Storm+0x1C760
    uint8_t* (*alloc)(uint32_t n, const char* file, int line, uint32_t flags);               // Storm+0x1C580
    void (*free_)(uint8_t* p, const char* file, int line);                                   // Storm+0x1B960
};
struct StormBuf { uint32_t cap, len; uint8_t* buf; };

// 1.13c Storm+0x3D40 (EAX=n, ESI=b in retail)
extern "C" void Storm_sub_3D40(StormBufEnv& e, StormBuf* b, uint32_t n, const char* file) {
    uint8_t* old = b->buf;
    b->cap = n;
    b->buf = e.realloc_(old, n, file, -2, 0x10);
    if (b->buf) return;
    b->buf = e.alloc(n, file, -2, 0);
    if (!old) return;
    if (b->len < n) n = b->len;
    for (uint32_t i = 0; i < n; i++) if (b->buf) b->buf[i] = old[i];
    e.free_(old, file, -2);
}
