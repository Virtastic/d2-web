// 1.13c Storm+0x04520 -- drain the list headed at param+8 (sub_1310), then unlink the node embedded at param+4.
#include <cstdint>
#include <cstring>
static inline uint32_t R(const uint8_t* p) { uint32_t v; memcpy(&v, p, 4); return v; }
static inline void W(uint8_t* p, uint32_t v) { memcpy(p, &v, 4); }
// node: [0]=prev slot value, [4]=link word (<0: ~addr of slot; else offset from node)
static void Unlink4520(uint8_t* n) {
    uint32_t prev = R(n);
    if (!prev) return;
    uint32_t link = R(n + 4);
    uint8_t* slot = (int32_t)link < 0 ? (uint8_t*)(uintptr_t)~link : n + (link - R((uint8_t*)(uintptr_t)prev + 4));
    W(slot, prev);
    W((uint8_t*)(uintptr_t)prev + 4, link);
    W(n, 0);
    W(n + 4, 0);
}
void Storm_Sub4520_Own(uint8_t* p) {
    for (;;) { uint32_t h = R(p + 8); if ((int32_t)h < 1) break; Unlink4520((uint8_t*)(uintptr_t)h); }
    // tail: node at p+4, link word at p+8 (retail reads param_1+8 as the link)
    uint8_t* n = p + 4;
    uint32_t prev = R(n);
    if (!prev) return;
    uint32_t link = R(p + 8);
    uint8_t* slot = (int32_t)link < 0 ? (uint8_t*)(uintptr_t)~link : n + (link - R((uint8_t*)(uintptr_t)prev + 4));
    W(slot, prev);
    W((uint8_t*)(uintptr_t)prev + 4, link);
    W(n, 0);
    W(p + 8, 0);
}
