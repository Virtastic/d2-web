// 1.13c D2Win+0x139A0 -- edit box: delete the selection [min(+0x54,+0x58), max) from the UTF-16
// text at +0x5C, set the cursor (+0x25C) to min (clamped to the new length), then re-validate via
// +0x13770. Null (string 0x1C2) or type != 1 (0x1C4) is fatal: d2win_exit.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

extern "C" void d2win_exit(uint32_t code);
extern "C" void d2win_sub_13770(void* p);

extern "C" EMSCRIPTEN_KEEPALIVE
void d2win_sub_139a0(void* p) {
    uint8_t* b = (uint8_t*)p;
    uint32_t type = 0;
    if (b) std::memcpy(&type, b, 4);
    if (!b || type != 1) d2win_exit((uint32_t)-1);
    int32_t s0, s1; std::memcpy(&s0, b + 0x54, 4); std::memcpy(&s1, b + 0x58, 4);
    int32_t lo = s0 < s1 ? s0 : s1, hi = s0 < s1 ? s1 : s0;
    uint16_t head[256], tail[256];
    if (lo == 0) head[0] = 0;
    else { std::memcpy(head, b + 0x5C, (size_t)lo * 2); head[lo] = 0; }  // strncpy: no pad needed
    uint16_t* t = (uint16_t*)(b + 0x5C);
    size_t n = 0; while (t[hi + n]) n++;
    std::memcpy(tail, t + hi, n * 2); tail[n] = 0;
    size_t h = 0; while (h < 256 && head[h]) h++;
    std::memcpy(t, head, h * 2);
    std::memcpy(t + h, tail, (n + 1) * 2);
    int32_t len = (int32_t)(h + n);
    int32_t cur = lo <= len ? lo : len;
    std::memcpy(b + 0x25C, &cur, 4);
    d2win_sub_13770(p);
}
