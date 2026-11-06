// 1.13c D2Game+0x70240, transcribed from the disassembly (the Ghidra decompile and the earlier port indexed the condition row by the inventory argument and
// started the visited counter from the table record: both wrong).
// Convention: ESI = condition table T (rows of 8 bytes at T+0x14; header bytes T+4 type, T+8 id, T+0xC threshold, T+0x4C flags, T+0x54 mode), five stack
// words, ret 0x14: a1 = the txt record (+0x6D level, +0x70 flag), a2 = the inventory walked by #10460 (zero: result 0), a3 = SLOT (the row),
// a4 = the three 12-byte output records, a5 = the visited array (0x30 dwords, entry n-1 for the n-th item examined).
// Every callee goes through Ops::k (rva, eax, stack words s[0..3]); [[0x6FD1829C]] is read through Ops::dtSlot (host address of the word holding the
// first pointer). Returns 1 when the slot passes: exact mode (cleared when #A940 answers non-zero for a counted item) wants count == need, else >= need.
#include <emscripten.h>
#include <cstdint>
#include <cstring>

struct D2Game70240Ops {
    void* ctx;
    uint32_t (*k)(void* ctx, uint32_t rva, uint32_t eax, const uint32_t* s);
    uint32_t dtSlot;
};

namespace {
inline uint32_t R32(uint32_t p, uint32_t o) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)(p + o), 4); return v; }
inline uint32_t R16(uint32_t p, uint32_t o) { uint16_t v; std::memcpy(&v, (const void*)(uintptr_t)(p + o), 2); return v; }
inline uint32_t R8(uint32_t p, uint32_t o) { return *(const uint8_t*)(uintptr_t)(p + o); }
inline void W32(uint32_t p, uint32_t o, uint32_t v) { std::memcpy((void*)(uintptr_t)(p + o), &v, 4); }
}

extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t d2_fn_d2game_70240_port(uint32_t esi, uint32_t a1, uint32_t a2, uint32_t slot, uint32_t a4, uint32_t a5, const D2Game70240Ops* o)
{
    auto K = [&](uint32_t rva, uint32_t eax, uint32_t s0, uint32_t s1, uint32_t s2) {
        const uint32_t s[4] = { s0, s1, s2, 0 };
        return o->k(o->ctx, rva, eax, s);
    };
    auto dt = [&]() { return R32(R32(o->dtSlot, 0), 0); };
    if (a2 == 0) return 0;
    const uint32_t row = esi + 8 * slot;
    uint32_t c = R16(row, 0x14);
    if ((c & 3) == 0) return 1;
    int32_t need = (int32_t)R8(row, 0x1b);
    if (!(need > 0)) need = 1;
    uint32_t exact = 1;
    int32_t count = 0, idx = 0;
    uint32_t recA = 0;
    if ((c & 1) && R16(row, 0x16) != 0xffff) recA = K(0xA53E, 0, R16(row, 0x16), 0, 0);
    uint32_t node = K(0xA3DC, 0, a2, 0, 0);
    if (!node) return count == need;
    while (node) {
        const uint32_t cur = K(0xA46C, 0, node, 0, 0);
        node = K(0xA4CC, 0, node, 0, 0);
        if (!cur) continue;
        if ((K(0xA6C4, 0, cur, 0, 0) & 0xff) != 3) continue;
        idx++;
        const uint32_t r9 = K(0xA53E, 0, R32(cur, 4), 0, 0);
        if (!r9) continue;
        const uint32_t cls = K(0xA532, 0, cur, 0, 0);
        c = R16(row, 0x14);
        if (c & 1) {
            const uint32_t arg = R16(row, 0x16);
            if (arg != 0xffff) {
                if ((int8_t)(c & 0xff) < 0) {
                    const uint32_t c84 = R32(r9, 0x84);
                    if (cls == c84) {
                        if (R32(recA, 0x80) != c84) continue;
                    } else {
                        const uint32_t c88 = R32(r9, 0x88);
                        if (cls == c88) {
                            const uint32_t m = R32(recA, 0x80);
                            if (m != c84 && m != c88) continue;
                        } else {
                            const uint32_t c8c = R32(r9, 0x8c);
                            if (cls != c8c) continue;
                            const uint32_t m = R32(recA, 0x80);
                            if (m != c84 && m != c88 && m != c8c) continue;
                        }
                    }
                } else if (arg != R32(cur, 4)) continue;
            }
        } else if (c & 2) {
            if (!K(0xA430, 0, cur, R16(row, 0x16), 0)) continue;
        }
        const uint32_t b1a = R8(row, 0x1a);
        if (b1a) {
            if (K(0xA898, 0, cur, 0, 0) != b1a) {
                if (b1a == 9) K(0xB096, 0, cur, 0, 0);
                continue;
            }
        }
        if (R8(row, 0x14) & 0x40) {
            if ((int32_t)K(0xA874, 0, cur, 0, 0) != (int32_t)R16(row, 0x18) - 1) continue;
        }
        c = R16(row, 0x14);
        if (c & 4) { if ((K(0xAA84, 0, cur, 0, 0) & 0xff) != 0) continue; }
        else if (c & 8) { if ((K(0xAA84, 0, cur, 0, 0) & 0xff) == 0) continue; }
        c = R16(row, 0x14);
        if (c & 0x20) { if (K(0xABD4, 0, cur, 0, 0) != 0) continue; }
        else if (c & 0x10) { if (K(0xABD4, 0, cur, 0, 0) == 0) continue; }
        c = R16(row, 0x14);
        if (c & 0x100) { if (cls != R32(r9, 0x84)) continue; }
        else if (c & 0x200) { if (cls != R32(r9, 0x88)) continue; }
        else if (c & 0x400) { if (cls != R32(r9, 0x8c)) continue; }
        if (c & 0x800) { if (K(0xA412, 0, cur, 0, 0) & 0x4000000) continue; }
        if (R8(esi, 4) == 0x1c && R8(r9, 0x12a) != 0 && R8(r9, 0x12b) != 0) {
            if ((int32_t)K(0xA42A, 0, cur, 0x164, 0) < (int32_t)R8(a1, 0x6d)) continue;
        }
        if (slot == 0) {
            const uint32_t m = R8(esi, 0x54);
            if ((m == 0xff || m == 0xfe) && (R8(esi, 0x4c) & 2)) {
                if ((K(0xAC4C, 0, cur, 0, 0) & 0xff) == 0) continue;
            }
        }
        if (idx < 0x30) { if (R32(a5, 4 * idx - 4) != 0) continue; }
        if (slot == 0) {
            const uint32_t t = R8(esi, 4);
            if (t != 0 && t - 0xf <= 0xc) {
                const uint32_t e = t - 0xf;
                if (e == 12) {
                    if ((int32_t)K(0xA874, 0, cur, 0, 0) == (int32_t)R32(esi, 0xc)) continue;
                } else {
                    const int32_t id = (int32_t)R32(esi, 8);
                    if (id >= 0 && id <= (int32_t)R32(dt(), 0xbd4)) {
                        const uint32_t rec = K(0x10D0, (uint32_t)id, 0, 0, 0);
                        if (!rec) continue;
                        const int32_t thr = (int32_t)R32(esi, 0xc) >> (R8(rec, 0x18) & 0x1f);
                        const uint32_t fn = e < 4 ? 0xA42A : e < 8 ? 0xA3E2 : 0xB0CC;
                        const int32_t val = (int32_t)K(fn, 0, cur, (uint32_t)id, 0);
                        const uint32_t kind = e & 3;
                        const bool skip = kind == 0 ? val < thr : kind == 1 ? val > thr : kind == 2 ? val == thr : val != thr;
                        if (skip) continue;
                    }
                }
            }
            uint32_t p = a4 + 4;
            for (int n = 3; n; n--, p += 12) {
                W32(p, 4, K(0xA8F8, 0, cur, 0, 0));
                if (R8(esi, 0x4c) & 1) W32(p, (uint32_t)-4, cur);
                const uint32_t m = R8(esi, 0x54);
                if (m != 0xff && m != 0xfe) continue;
                W32(p, (uint32_t)-4, cur);
                W32(p, 0, m == 0xff ? R32(cur, 4) : 0xffffffffu);
                const uint32_t w = R16(esi, 0x4c);
                uint32_t code;
                if ((int8_t)(w & 0xff) < 0) code = R32(r9, 0x88);
                else if (w & 0x100) code = R32(r9, 0x8c);
                else continue;
                if (code == 0x20202020) continue;
                const int32_t id2 = (int32_t)K(0xA2FE, 0, R32(dt(), 0x94), code, 0);
                if (id2 < 0) continue;
                const uint32_t r2 = K(0xA53E, 0, (uint32_t)id2, 0, 0);
                if (!r2) continue;
                if (R32(a1, 0x70) != 0 || R16(r2, 0xf6) < 0x64) W32(p, 0, (uint32_t)id2);
            }
        }
        if (idx < 0x30) W32(a5, 4 * idx - 4, 1);
        count++;
        if (K(0xA940, 0, cur, 0, 0) != 0) exact = 0;
    }
    return exact ? (count == need) : (count >= need);
}
