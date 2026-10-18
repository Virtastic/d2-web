// Prior art used: D2MOO's D2Gfx module has no CRT heap body; cross-checked the
// corresponding D2Client group allocator against D2Gfx's cached 1.13c decompile.
#include <cstdint>
#include <emscripten/emscripten.h>

struct D2GfxSbhGroupEnv {
    uint32_t (*virtual_alloc)(uint32_t address, uint32_t size, uint32_t type, uint32_t protection);
};

namespace {
uint32_t& word(uint32_t address) {
    return *reinterpret_cast<uint32_t*>(static_cast<uintptr_t>(address));
}
uint8_t& byte(uint32_t address) {
    return *reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(address));
}
}

// 1.13c D2Gfx+0x047A2
// First available group, highest bit first. List heads are initialized before
// committing memory, so a failed commit leaves those writes but no bitmap change.
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2gfx_sbh_alloc_new_group(
    uint32_t region, const D2GfxSbhGroupEnv* env) {
    const uint32_t control = word(region + 0x10);
    uint32_t group = 0;
    for (uint32_t mask = word(region + 8); (mask & 0x80000000u) == 0; mask <<= 1)
        ++group; // Caller guarantees a nonzero available-group mask.
    const uint32_t heads = control + 0x144 + group * 0x204;
    for (uint32_t bucket = 0; bucket < 63; ++bucket) {
        const uint32_t head = heads + bucket * 8;
        word(head + 8) = head;
        word(head + 4) = head;
    }
    const uint32_t base = word(region + 0x0c) + group * 0x8000;
    if (env->virtual_alloc(base, 0x8000, 0x1000, 4) == 0) return -1;
    // Retain the retail unsigned address-wrap gate, as well as its write order.
    if (base <= base + 0x7000u) {
        for (uint32_t page = 0; page < 8; ++page) {
            const uint32_t links = base + page * 0x1000 + 0x10;
            word(links - 8) = 0xffffffffu;
            word(links + 0xfec) = 0xffffffffu;
            word(links) = links + 0xffc;
            word(links - 4) = 0xff0;
            word(links + 4) = links - 0x1004;
            word(links + 0xfe8) = 0xff0;
        }
    }
    word(heads + 0x1fc) = base + 0x0c;
    word(base + 0x14) = heads + 0x1f8;
    word(heads + 0x200) = base + 0x700c;
    word(base + 0x7010) = heads + 0x1f8;
    word(control + 0x44 + group * 4) = 0;
    word(control + 0xc4 + group * 4) = 1;
    const uint8_t previous = byte(control + 0x43);
    byte(control + 0x43) = static_cast<uint8_t>(previous + 1);
    if (previous == 0) word(region + 4) |= 1;
    word(region + 8) &= ~(0x80000000u >> (group & 31));
    return static_cast<int32_t>(group);
}
