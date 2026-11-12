// 1.13c D2Gfx+0xAE60: sets the software draw target (ECX = buffer, EDX = pitch, two stack words; ret 8).
// Stores buffer, pitch and the two stack words in the four dwords at 0x14778. A pitch that differs from the cached one
// (0x1D684) is cached and the 732-entry row offset table at 0x148D0 is rebuilt: entry i = (i - 32) * pitch. When the
// mode dword at 0x11270 differs from the one at 0x10D68, 0x8970 runs with EAX = pitch, ECX = the mode, EDX = the first
// stack word and its answer is returned; else the pitch is returned.
#include <cstdint>

struct D2GfxAe60State {
    uint32_t* target;    // the four dwords at 0x14778
    uint32_t* pitch;     // the dword at 0x1D684
    uint32_t* rows;      // the 732 dwords at 0x148D0
    uint32_t mode;       // the dword at 0x11270
    uint32_t modeRef;    // the dword at 0x10D68
    void* ctx;
    uint32_t (*apply)(void* ctx, uint32_t pitch, uint32_t mode, uint32_t a);   // 0x8970
};

extern "C" uint32_t d2gfx_ae60(uint32_t buffer, uint32_t pitch, uint32_t a, uint32_t b, D2GfxAe60State* s) {
    s->target[0] = buffer;
    s->target[1] = pitch;
    s->target[2] = a;
    s->target[3] = b;
    if (pitch != *s->pitch) {
        *s->pitch = pitch;
        uint32_t v = (0u - pitch) << 5;
        for (int i = 0; i < 732; i++, v += pitch) s->rows[i] = v;
    }
    if (s->mode != s->modeRef) return s->apply(s->ctx, pitch, s->mode, a);
    return pitch;
}
