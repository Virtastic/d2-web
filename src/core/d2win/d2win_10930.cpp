// 1.13c D2Win+0x10930 (286 bytes): per-tick state step of a type-2 control. ESI = control (implicit
// register argument; the caller 0x10A50 holds it there). Seams: hit = D2Win+0x107C0, cmp10086 =
// D2CMP #10086 (frame count), tick = GetTickCount. Type != 2 is fatal 0x9A (Fog #10265/#10024/_exit).
#include <cstdint>
#include <cstring>
#include <unistd.h>
#include <emscripten.h>

static inline uint32_t rd32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
static inline void wr32(void* p, uint32_t o, uint32_t v) { std::memcpy((uint8_t*)p + o, &v, 4); }

struct D2Win10930Env {
    int32_t  (*hit)(void* obj);
    int32_t  (*cmp10086)(uint32_t p);
    uint32_t (*tick)();
};

// 1.13c D2Win+0x10930
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2win_sub_10930(void* obj, const D2Win10930Env* env) {
    if (rd32(obj, 0x00) != 2) _exit(-1);
    const uint32_t base = rd32(obj, 0x40);
    if (base == 0) return 0;
    auto first = [&](uint32_t st) { // *(*(base + st*16)) or 0
        uint32_t e = rd32((void*)(uintptr_t)(base + st * 16), 0);
        return e ? rd32((void*)(uintptr_t)e, 0) : 0u;
    };
    const uint32_t s0 = rd32(obj, 0x50);
    if (s0 == 0 || s0 == 1) {
        const uint32_t ns = env->hit(obj) != 0;
        wr32(obj, 0x50, ns);
        const uint32_t cb = rd32(obj, 0x58);
        if (s0 != ns && cb) ((void (*)(void*))(uintptr_t)cb)(obj);
    }
    int32_t n = 1;
    if (uint32_t f = first(rd32(obj, 0x50))) n = env->cmp10086(f);
    const uint32_t s1 = rd32(obj, 0x50);
    const int32_t cnt = (int32_t)rd32(obj, 0x4C);
    bool reset = false;
    if (s1 == 2) { if (cnt >= n - 2) { wr32(obj, 0x50, 3); reset = true; } }
    else if (s1 == 4 && cnt >= n - 2) { wr32(obj, 0x50, 1); reset = true; }
    if (reset) { wr32(obj, 0x4C, 0); wr32(obj, 0x48, env->tick()); }
    if (s1 != rd32(obj, 0x50)) {
        if (uint32_t f = first(rd32(obj, 0x50))) env->cmp10086(f);
    }
    if (n < (int32_t)rd32(obj, 0x4C)) { wr32(obj, 0x4C, 0); wr32(obj, 0x48, env->tick()); }
    return 1;
}
