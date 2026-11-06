// 1.13c D2Multi+0x10C0 _atol with its per-thread locale lookups as seams (ptd, locinfo refresh, __isctype_mt).
#include <cstdint>
#include <cstring>

struct D2MultiAtolEnv {
    uint8_t* (*getptd)(); uint8_t* (*updatetlocinfo)(); int (*isctype_mt)(uint8_t*, int, int); uint32_t cur_locinfo;
};

static inline uint32_t rd32(const uint8_t* p) { uint32_t v; std::memcpy(&v, p, 4); return v; }

extern "C" int32_t d2multi_atol_env(const uint8_t* s, const D2MultiAtolEnv* env) {
    uint8_t* ptd = env->getptd();
    uint8_t* loc = (uint8_t*)(uintptr_t)rd32(ptd + 0x64);
    if (rd32(ptd + 0x64) != env->cur_locinfo) loc = env->updatetlocinfo();
    auto space = [&](uint32_t c) -> bool {
        if ((int32_t)rd32(loc + 0x28) > 1) return env->isctype_mt(loc, (int)c, 8) != 0;
        const uint8_t* t = (const uint8_t*)(uintptr_t)rd32(loc + 0x48);
        uint16_t w; std::memcpy(&w, t + c * 2, 2);
        return (w & 8) != 0;
    };
    uint32_t c = *s++;
    while (space(c)) c = *s++;
    uint32_t sign = c;
    if (c == '-' || c == '+') c = *s++;
    int32_t n = 0;
    while (c >= '0' && c <= '9') { n = (int32_t)((uint32_t)n * 10u + (c - '0')); c = *s++; }
    return sign == '-' ? -n : n;
}
