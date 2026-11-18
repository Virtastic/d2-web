// src/core/fog/fog_sub_1b240.cpp -- Fog+0x1B240, register a pool block in the manager's sorted block table.
// Retail 32-bit layout, read and written by offset: a pointer field is a 32-bit word (the engine is wasm64, so a C++
// pointer member would be 8 bytes and shift every field after it).
#include <cstdint>
#include <cstdlib>
#include <cstring>

namespace {
inline uint32_t ld(const uint8_t* p, uint32_t off) { uint32_t v; memcpy(&v, p + off, 4); return v; }
inline void st(uint8_t* p, uint32_t off, uint32_t v) { memcpy(p + off, &v, 4); }
inline uint8_t* at(uint32_t a) { return reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(a)); }
}  // namespace

// 1.13c Fog+0x1B240
// Custom convention: EAX = manager, stack word = block. Manager: +0x7A0 count, +0x7A4 capacity, +0x7A8 table of
// {block, key} pairs (8 bytes each), +0x17A8 total bytes. Block: +0 key, +0x14 info (info+0x1C bits, info+0x20 data size).
// Grows the table by 0x20 entries when full (a failed realloc is fatal), adds dataSize + 0x18 + ((bits + 0x1F) >> 5) * 4
// to the total and inserts {block, key} at the lower bound of the key.
void fog_sub_1B240(uint8_t* m, uint8_t* b) {
    int32_t cnt = (int32_t)ld(m, 0x7a0), cap = (int32_t)ld(m, 0x7a4);
    if (cap <= cnt) {
        cap += 0x20;
        st(m, 0x7a4, (uint32_t)cap);
        void* t = realloc(at(ld(m, 0x7a8)), (size_t)cap * 8);
        if (!t) abort();  // retail: log "(1) Pool Blocks overflowed", error 0x261, _exit(-1)
        st(m, 0x7a8, (uint32_t)(uintptr_t)t);
    }
    const uint8_t* info = at(ld(b, 0x14));
    st(m, 0x17a8, ld(m, 0x17a8) + ld(info, 0x20) + 0x18 + (((int32_t)ld(info, 0x1c) + 0x1f) >> 5) * 4);
    uint8_t* tab = at(ld(m, 0x7a8));
    const uint32_t key = ld(b, 0);
    int lo = 0, hi = cnt;
    while (lo < hi) {
        int mid = lo + ((hi - lo) >> 1);
        if (ld(tab, (uint32_t)mid * 8 + 4) < key) lo = mid + 1; else hi = mid;
    }
    if (lo < cnt) memmove(tab + (lo + 1) * 8, tab + lo * 8, (size_t)(cnt - lo) * 8);
    st(tab, (uint32_t)lo * 8, (uint32_t)(uintptr_t)b);
    st(tab, (uint32_t)lo * 8 + 4, key);
    st(m, 0x7a0, (uint32_t)(cnt + 1));
}
