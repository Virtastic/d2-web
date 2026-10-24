// 1.13c Storm+0x22C10/0x22C80/0x22CB0/0x4790/0x4C20: memzero, memcpy, memcmp, 2-dword clear, list unlink.
#include <emscripten.h>
#include <cstdint>
#include <cstddef>
#include <cstring>
// 1.13c Storm+0x22C10: zero n bytes
void Storm_Sub22C10(uint8_t* p, uint32_t n) { while (n--) *p++ = 0; }
// 1.13c Storm+0x22C80: copy n bytes
void Storm_Sub22C80(uint8_t* d, const uint8_t* s, uint32_t n) { while (n--) *d++ = *s++; }
// 1.13c Storm+0x22CB0: 0 if equal over n, else -1 (a<b unsigned at first difference) or 1
int32_t Storm_Sub22CB0(const uint8_t* a, const uint8_t* b, uint32_t n) {
    for (; n; n--, a++, b++) if (*a != *b) return *a < *b ? -1 : 1;
    return 0;
}
struct StormNode { uint32_t link, tag; };  // retail 32-bit layout
static_assert(offsetof(StormNode, tag) == 4, "tag at +4");
// 1.13c Storm+0x4790: p[0]=p[1]=0
void Storm_Sub4790(uint32_t* p) { p[0] = 0; p[1] = 0; }
// also Storm 0x04C20: node {link,tag}; link!=0: *(tag<0 ? ~tag : node+(tag-link[1])) = link; link[1]=tag; clear node
void Storm_Sub4C20(uint8_t* node) {
    auto read = [](const uint8_t* p) { uint32_t v; std::memcpy(&v, p, 4); return v; };
    const uint32_t link = read(node);
    if (!link) return;
    const uint32_t tag = read(node + 4);
    // x86 wraps the subtraction at 32 bits; lift its signed displacement onto the native base.
    const uintptr_t target = (int32_t)tag < 0 ? (uintptr_t)(uint32_t)~tag
        : (uintptr_t)node + (intptr_t)(int32_t)(uint32_t)(tag - read((const uint8_t*)(uintptr_t)link + 4));
    std::memcpy((void*)target, &link, 4);
    // The first store may alias either node dword: retail loads both again afterwards.
    const uint32_t next = read(node), nextTag = read(node + 4);
    std::memcpy((uint8_t*)(uintptr_t)next + 4, &nextTag, 4);
    std::memset(node, 0, 8);
}
#define P(T, i) ((T)(uintptr_t)r[i])
extern "C" {
EMSCRIPTEN_KEEPALIVE void d2_fn_storm_22c10(uint32_t* r) { Storm_Sub22C10(P(uint8_t*, 0), r[1]); r[4] = 0; }
EMSCRIPTEN_KEEPALIVE void d2_fn_storm_22c80(uint32_t* r) { Storm_Sub22C80(P(uint8_t*, 0), P(const uint8_t*, 1), r[2]); r[4] = 0; }
EMSCRIPTEN_KEEPALIVE void d2_fn_storm_22cb0(uint32_t* r) { r[4] = (uint32_t)Storm_Sub22CB0(P(const uint8_t*, 0), P(const uint8_t*, 1), r[2]); }
EMSCRIPTEN_KEEPALIVE void d2_fn_storm_4790(uint32_t* r) { Storm_Sub4790(P(uint32_t*, 0)); r[4] = 0; }
EMSCRIPTEN_KEEPALIVE void d2_fn_storm_4c20(uint32_t* r) { Storm_Sub4C20(P(uint8_t*, 0)); r[4] = 0; }
}
