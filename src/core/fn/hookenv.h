// Probe environment for the rule templates in src/core/d2game/*.h (a retail recording FN probes of register-convention
// D2Game functions). h() logs the call exactly as the capture's hook recorder does (count, then 10 words:
// rva, eax ecx edx ebx esi, s0..s3) and returns the next value queued for that rva. sc = (rva, value) pairs,
// zero rva ends; per-rva order, last value repeats, none = 0 -- the same rule as the descriptor's `hook` lines.
#pragma once
#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <emscripten/stack.h>

struct HookEnv {
    uint32_t* L; const uint32_t* sc;
    uint32_t tbl = 0, dtPtr = 0, fileStr = 0, bmSlot = 0;     // module globals (addresses of the pinned blobs / scrubbed module words)
    static constexpr uint32_t kLogMax = 64;   // ponytail: a descriptor `blob lg` must hold 4 + 40 * calls bytes; 0x6F930 logs up to 45 calls (blob 2600)
    uint32_t seen[48][2]; int nseen = 0;
    HookEnv(uint32_t* log, const uint32_t* seq) : L(log), sc(seq) { L[0] = 0; }
    static uint32_t rd32(uint32_t a) { uint32_t v; memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
    static int32_t rd16s(uint32_t a) { int16_t v; memcpy(&v, (const void*)(uintptr_t)a, 2); return v; }
    static uint32_t rd8(uint32_t a) { return *(const uint8_t*)(uintptr_t)a; }
    static void wr32(uint32_t a, uint32_t v) { memcpy((void*)(uintptr_t)a, &v, 4); }
    static bool badCodePtr(uint32_t p) { return p == 0; }
    static uint32_t handlerRva(uint32_t p) { return p & 0xFFFFFF; }   // table words are recorded as 0xB2000000|rva
    static bool isStk(uint32_t a) { return a >= (uint32_t)emscripten_stack_get_end() && a < (uint32_t)emscripten_stack_get_base(); }
    uint32_t stk(const void* p) const { return (uint32_t)(uintptr_t)p; }   // a local's address: only the log tags it (0xB3000000)
    uint32_t h(uint32_t rva, uint32_t a = 0, uint32_t c = 0, uint32_t d = 0, uint32_t b = 0, uint32_t s = 0,
               uint32_t s0 = 0, uint32_t s1 = 0, uint32_t s2 = 0, uint32_t s3 = 0, uint32_t s4 = 0) {
        if (L[0] >= kLogMax) abort();   // log capacity exceeded
        uint32_t* e = L + 1 + L[0]++ * 10;
        e[0] = rva; e[1] = a; e[2] = c; e[3] = d; e[4] = b; e[5] = s; e[6] = s0; e[7] = s1; e[8] = s2; e[9] = s3;
        // retail logs a pointer into its stack as 0xB3000000; `hookw` out-parameter writes ride in sc as key ((1+stack word)<<24)|rva
        const uint32_t sw[5] = { s0, s1, s2, s3, s4 };
        for (int i = 1; i < 10; i++) if (isStk(e[i])) e[i] = 0xB3000000u;
        for (const uint32_t* p = sc; p[0]; p += 2) for (uint32_t w = 0; w < 5; w++) if (p[0] == (((1 + w) << 24) | rva)) wr32(sw[w], p[1]);
        int k = 0; while (k < nseen && seen[k][0] != rva) k++;
        if (k == nseen) { if (nseen >= 48) abort(); seen[nseen][0] = rva; seen[nseen++][1] = 0; }
        uint32_t n = seen[k][1]++, last = 0;
        for (const uint32_t* p = sc; p[0]; p += 2) if (p[0] == rva) { last = p[1]; if (!n--) return last; }
        return last;
    }
};
