// Seam environment for the D2Game engine helpers: HookEnv plus
//  - hs(): a hook with up to eight stack words (log s0..s3 like the recorder), an optional `hookblk` block log, and
//    `hookw` out-pointer writes through any stack word (HookEnv::h only reaches words 0..4);
//  - tok(): the capture tags a dword that points into descriptor blob k as 0xB1<k><offset> (log tokenisation).
// Blob 0 = sc (rva,value pairs, zero rva ends), blob 1 = hook log; user blobs follow at r[14+2k], size r[15+2k].
#pragma once
#include "fn_bloblen.h"
#include "hookenv.h"
#include <initializer_list>
struct GE : HookEnv {
    const uint32_t* r; int nb;
    GE(const uint32_t* rr, int nblobs) : HookEnv((uint32_t*)(uintptr_t)rr[14 + 2 * 1], (const uint32_t*)(uintptr_t)rr[14]), r(rr), nb(nblobs) {}
    uint32_t B(int k) const { return r[14 + 2 * k]; }
    uint32_t hs(uint32_t rva, uint32_t a, uint32_t c, uint32_t d, uint32_t b, uint32_t si, std::initializer_list<uint32_t> w,
                int blkIdx = -1, uint32_t blkN = 0, uint32_t blkMask = ~0u) {
        uint32_t s[8] = {}; int n = 0; for (uint32_t v : w) s[n++] = v;
        const uint32_t v = h(rva, a, c, d, b, si, s[0], s[1], s[2], s[3], 0);
        if (blkIdx >= 0) blk(rva, (const uint32_t*)(uintptr_t)s[blkIdx], blkN, blkMask);
        for (uint32_t i = 4; i < 8; i++) {
            const uint32_t key = ((1u + i) << 24) | rva; uint32_t off = 0;
            for (const uint32_t* p = sc; p[0]; p += 2) if (p[0] == key) { wr32(s[i] + off, p[1]); off += 4; }
        }
        return v;
    }
    // sgptDataTables skill record: the table at T (a descriptor blob reached through `gptr`): +0xBA0 count, +0xB98 records (0x23C each); 0 = out of range
    uint32_t skillRec(uint32_t T, int32_t idx) const {
        if (idx < 0 || idx >= (int32_t)rd32(T + 0xBA0)) return 0;
        return rd32(T + 0xB98) + (uint32_t)idx * 0x23C;
    }
    // a unit's tile position through the type switch the 0x260D0 family inlines: types 2, 4, 5 read the pData word (+0x2C -> +0xC / +0x10),
    // the others ask 0xA454 / 0xA3B8 (0 with no pData)
    uint32_t unitX(uint32_t u) { const uint32_t t = rd32(u), p = rd32(u + 0x2C); if (t == 2 || (t > 3 && t <= 5)) return rd32(p + 0xC); return p ? h(0xA454, 0, 0, 0, 0, 0, p) : 0; }
    uint32_t unitY(uint32_t u) { const uint32_t t = rd32(u), p = rd32(u + 0x2C); if (t == 2 || (t > 3 && t <= 5)) return rd32(p + 0x10); return p ? h(0xA3B8, 0, 0, 0, 0, 0, p) : 0; }
    void tok() {
        for (uint32_t i = 0; i < L[0]; i++)
            for (int w = 1; w < 10; w++) {
                uint32_t& v = L[1 + i * 10 + w];
                for (int k = 2; k < nb; k++) if (v >= B(k) && v < B(k) + fnBlobLen(r, k)) { v = 0xB1000000u | (uint32_t)(k << 16) | (v - B(k)); break; }
            }
    }
};
