// Probe environment for the rule templates in src/core/d2game/*.h (a retail recording FN probes of register-convention
// D2Game functions). h() logs the call exactly as the capture's hook recorder does (count, then 10 words:
// rva, eax ecx edx ebx esi, s0..s3) and returns the next value queued for that rva. sc = (rva, value) pairs,
// zero rva ends; per-rva order, last value repeats, none = 0 -- the same rule as the descriptor's `hook` lines.
#pragma once
#include <cstdint>
#include <cstring>
#include <cstdlib>

struct HookEnv {
    uint32_t* L; const uint32_t* sc;
    static constexpr uint32_t kLogMax = 27;   // ponytail: largest `blob lg` in the descriptors is 1100 bytes = 27 entries; a smaller blob needs a per-probe cap
    uint32_t tbl = 0, dtPtr = 0, fileStr = 0, bmSlot = 0;     // module globals (addresses of the pinned blobs / scrubbed module words)
    uint32_t seen[16][2]; int nseen = 0;
    HookEnv(uint32_t* log, const uint32_t* seq) : L(log), sc(seq) { L[0] = 0; }
    static uint32_t rd32(uint32_t a) { uint32_t v; memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
    static int32_t rd16s(uint32_t a) { int16_t v; memcpy(&v, (const void*)(uintptr_t)a, 2); return v; }
    static uint32_t rd8(uint32_t a) { return *(const uint8_t*)(uintptr_t)a; }
    static void wr32(uint32_t a, uint32_t v) { memcpy((void*)(uintptr_t)a, &v, 4); }
    static bool badCodePtr(uint32_t p) { return p == 0; }
    static uint32_t handlerRva(uint32_t p) { return p & 0xFFFFFF; }   // table words are recorded as 0xB2000000|rva
    uint32_t h(uint32_t rva, uint32_t a = 0, uint32_t c = 0, uint32_t d = 0, uint32_t b = 0, uint32_t s = 0,
               uint32_t s0 = 0, uint32_t s1 = 0, uint32_t s2 = 0, uint32_t s3 = 0) {
        if (L[0] >= kLogMax) abort();   // log capacity exceeded
        uint32_t* e = L + 1 + L[0]++ * 10;
        e[0] = rva; e[1] = a; e[2] = c; e[3] = d; e[4] = b; e[5] = s; e[6] = s0; e[7] = s1; e[8] = s2; e[9] = s3;
        int k = 0; while (k < nseen && seen[k][0] != rva) k++;
        if (k == nseen) { if (nseen >= 16) abort(); seen[nseen][0] = rva; seen[nseen++][1] = 0; }
        uint32_t n = seen[k][1]++, last = 0;
        for (const uint32_t* p = sc; p[0]; p += 2) if (p[0] == rva) { last = p[1]; if (!n--) return last; }
        return last;
    }
};
