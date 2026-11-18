// Storm block-cache fill: read one cache block at an aligned offset.
// The file, cache and info records are retail 32-bit layouts (pointer slots are 32-bit words): file +0x110 cache,
// +0x114 info; cache +0x118 owner, +0x11C offset, +0x120 block size, +0x124 data, +0x128 result; info +8 size,
// +0xC flags. On wasm64 an address fits in 32 bits, so a slot is widened on read and narrowed on write.
#include <cstdint>
#include <cstring>
// seam: Storm+0x28450 (file, off, dst, len) -> bytes read; unmapped, empty provider state
uint32_t Seam28450_28DF0(void*, uint32_t, uint8_t*, uint32_t) { return 0; }

static uint32_t rd(const uint8_t* p, uint32_t o) { uint32_t v; memcpy(&v, p + o, 4); return v; }
static void wr(uint8_t* p, uint32_t o, uint32_t v) { memcpy(p + o, &v, 4); }
static uint8_t* at(uint32_t a) { return (uint8_t*)(uintptr_t)a; }

// 1.13c Storm+0x28DF0: ESI = file, EDX = offset; returns the cache result.
uint32_t Storm_28DF0(uint8_t* f, uint32_t off) {
    const uint32_t self = (uint32_t)(uintptr_t)f;
    uint8_t* c = at(rd(f, 0x110));
    if (rd(c, 0x118) == self && rd(c, 0x11c) == off) return rd(c, 0x128);
    uint32_t len = rd(c, 0x120);
    const uint8_t* info = at(rd(f, 0x114));
    const uint32_t size = rd(info, 8);
    if (size < len + off && ((rd(info, 0xc) >> 8) & 0xff) == 0) len = size - off;
    wr(c, 0x118, self);
    wr(c, 0x11c, off);
    wr(c, 0x128, Seam28450_28DF0(f, off, at(rd(c, 0x124)), len));
    return rd(c, 0x128);
}

// FN replay entry: f = the file struct, off = the second argument.
extern "C" uint32_t storm_28df0_fnrun(void* f, uint32_t off) { return Storm_28DF0((uint8_t*)f, off); }
