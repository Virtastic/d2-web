// src/core/d2sound/d2sound_8eb0.cpp -- 1.13c D2sound+0x8EB0
// Release (vtable slot 2, stdcall, `this` the one word) and clear the three EAX-property COM globals DAT_6f9c64b8, 6524, 6528, in that order.
// The globals are guest words (pointer-width-independent): the caller passes the addresses of the three words.
#include <cstdint>

typedef uint32_t (*D2sndRelease)(uint32_t);   // IUnknown::Release

static inline uint32_t d2snd_rdw(uint32_t a) { return *(const uint32_t*)(uintptr_t)a; }

// 1.13c D2sound+0x8EB0
void D2sound_8EB0(uint32_t* g64b8, uint32_t* g6524, uint32_t* g6528) {
    uint32_t* const slots[3] = { g64b8, g6524, g6528 };
    for (uint32_t* s : slots) {
        const uint32_t itf = *s;
        if (!itf) continue;
        ((D2sndRelease)(uintptr_t)d2snd_rdw(d2snd_rdw(itf) + 8))(itf);
        *s = 0;
    }
}
