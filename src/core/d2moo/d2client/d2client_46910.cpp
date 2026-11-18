// D2Client 0x46910: roll a random entry index for a record (EDI), up to 10 tries (callers 0x46E00, 0x4A3D0).
#include <cstdint>
#include <cstring>
#include <emscripten.h>
#include "client_host.h"
static uint32_t rd46910(const uint8_t* p) { uint32_t v; std::memcpy(&v, p, 4); return v; }
// 1.13c D2Client+0x46910 ; rec = EDI (+5 table, +9 count, +0xD chosen index, all unaligned dwords), tbl = rec+5 resolved,
// f2c08 = the module table at 0xF2C08 (identity of tbl against it is the retail `cmp eax,0x6FBA2C08`), unit = [0x11BBFC]
// (+4 class, seed +0x20 lo / +0x24 hi; a null unit faults in retail on the first roll). Entries are 0xF bytes: +0 word,
// +2 byte, +3 dword want, +7 dword arg, +0xB dword class. Ordinal_10174 (thunk 0xC58C, 3 stack words) goes through host.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_46910(uint8_t* rec, const uint8_t* tbl, const uint8_t* f2c08, uint8_t* unit,
                                                    uint32_t h973b, uint32_t h973f, D2cHost host) {
    int32_t cls = unit ? (int32_t)rd46910(unit + 4) : -1;
    for (int32_t tries = 10;;) {
        --tries;
        int32_t n = (int32_t)rd46910(rec + 9);
        uint32_t idx = 0;
        if (n > 0) {
            uint64_t s = (uint64_t)rd46910(unit + 0x20) * 0x6AC690C5u + rd46910(unit + 0x24);
            uint32_t lo = (uint32_t)s, hi = (uint32_t)(s >> 32);
            std::memcpy(unit + 0x20, &lo, 4); std::memcpy(unit + 0x24, &hi, 4);
            idx = ((uint32_t)n & ((uint32_t)n - 1)) == 0 ? (lo & ((uint32_t)n - 1)) : lo % (uint32_t)n;
        }
        std::memcpy(rec + 0xD, &idx, 4);
        if ((int32_t)idx >= 2) {
            const uint8_t* e = tbl + idx * 0xF;
            int32_t ec = (int32_t)rd46910(e + 0xB);
            if (ec == cls || ec == 7) {
                if (!e[2]) return;
                D2cCall c = {}; c.rva = 0xC58C; c.s[0] = h973b; c.s[1] = rd46910(e + 7); c.s[2] = 0;
                uint32_t r = host(&c);
                if (r == rd46910(tbl + rd46910(rec + 0xD) * 0xF + 3)) {
                    if (tbl != f2c08) return;
                    uint16_t w; std::memcpy(&w, tbl + rd46910(rec + 0xD) * 0xF, 2);
                    if (w != 0xFF) return;
                    D2cCall d = {}; d.rva = 0xC58C; d.s[0] = h973f; d.s[1] = 0xC; d.s[2] = 0xD;
                    if (host(&d) == 1) { uint32_t two = 2; std::memcpy(rec + 0xD, &two, 4); }
                    return;
                }
            }
        }
        if (tries == 0) { uint32_t two = 2; std::memcpy(rec + 0xD, &two, 4); return; }
    }
}
