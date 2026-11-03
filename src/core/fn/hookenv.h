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
    // the `C` block records of the capture: n dwords at p in 9-word records (rva|0x40000000, d0..d8), stack pointers as 0xB3000000
    void blk(uint32_t rva, uint32_t p, uint32_t n) {
        for (uint32_t o = 0; o < n; o += 9) {
            if (L[0] >= kLogMax) abort();
            uint32_t* e = L + 1 + L[0]++ * 10; e[0] = rva | 0x40000000u;
            for (uint32_t j = 0; j < 9; j++) { uint32_t v = o + j < n ? rd32(p + 4 * (o + j)) : 0; e[1 + j] = isStk(v) ? 0xB3000000u : v; }
        }
    }
    // REGS `J`: EDI at hook entry as an argument, one record (rva|0x20000000, edi, 0...) straight after the call's own record; a stack pointer is 0xB3000000.
    void j(uint32_t rva, uint32_t edi) {
        if (L[0] >= kLogMax) abort();
        uint32_t* e = L + 1 + L[0]++ * 10; e[0] = rva | 0x20000000u; e[1] = isStk(edi) ? 0xB3000000u : edi;
        for (int i = 2; i < 10; i++) e[i] = 0;
    }
    uint32_t* L; const uint32_t* sc;
    uint32_t tbl = 0, dtPtr = 0, fileStr = 0, bmSlot = 0;     // module globals (addresses of the pinned blobs / scrubbed module words)
    static constexpr uint32_t kLogMax = 512;   // 512 (main had 400): 3C850 and 40CA0 log 6 records per step over a 64-step missile ring, past 400; a larger cap only raises the limit. ponytail: a descriptor `blob lg` must hold 4 + 40 * calls bytes; 0x6F930 logs up to 45 calls (blob 2600)
    uint32_t seen[48][2]; int nseen = 0;
    HookEnv(uint32_t* log, const uint32_t* seq) : L(log), sc(seq) { L[0] = 0; }
    static uint32_t rd32(uint32_t a) { uint32_t v; memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
    static int32_t rd16s(uint32_t a) { int16_t v; memcpy(&v, (const void*)(uintptr_t)a, 2); return v; }
    static uint32_t rd8(uint32_t a) { return *(const uint8_t*)(uintptr_t)a; }
    static void wr8(uint32_t a, uint32_t v) { *(uint8_t*)(uintptr_t)a = (uint8_t)v; }
    static void wr32(uint32_t a, uint32_t v) { memcpy((void*)(uintptr_t)a, &v, 4); }
    static bool badCodePtr(uint32_t p) { return p == 0; }
    static uint32_t handlerRva(uint32_t p) { return p & 0xFFFFFF; }   // table words are recorded as 0xB2000000|rva
    static bool isStk(uint32_t a) { return a >= (uint32_t)emscripten_stack_get_end() && a < (uint32_t)emscripten_stack_get_base(); }
    uint32_t stk(const void* p) const { return (uint32_t)(uintptr_t)p; }   // a local's address: only the log tags it (0xB3000000)
    // hookblk: log n dwords of a stack block the callee was handed, 9 per record (rva|0x40000000); a clear MASK bit (word < 32) records 0
    void blk(uint32_t rva, const uint32_t* b, uint32_t n, uint32_t mask = ~0u) {
        for (uint32_t o = 0; o < n; o += 9) {
            if (L[0] >= kLogMax) abort();
            uint32_t* e = L + 1 + L[0]++ * 10;
            e[0] = rva | 0x40000000u;
            for (uint32_t j = 0; j < 9; j++) { uint32_t i = o + j, v = i < n ? b[i] : 0; if (i < 32 && !(mask >> i & 1)) v = 0; e[1 + j] = isStk(v) ? 0xB3000000u : v; }
        }
    }
    // REGS `D`: the 9 dwords at [EDI] at hook entry (the event record handed to 0x33CE0), one record (rva|0x40000000, d0..d8). `bytemask` bit i keeps byte i
    // of the 36 (a clear bit zeroes it: stack residue the callee never wrote); a stack pointer is 0xB3000000. Call it where retail calls the callee, after h().
    void edi9(uint32_t rva, const void* rec, uint64_t bytemask = ~0ull) {
        if (L[0] >= kLogMax) abort();
        uint32_t* e = L + 1 + L[0]++ * 10; e[0] = rva | 0x40000000u;
        const uint8_t* b = (const uint8_t*)rec;
        for (uint32_t j = 0; j < 9; j++) { uint32_t v = 0; for (uint32_t q = 0; q < 4; q++) if (bytemask >> (4 * j + q) & 1) v |= (uint32_t)b[4 * j + q] << (8 * q); e[1 + j] = isStk(v) ? 0xB3000000u : v; }
    }
    uint32_t h(uint32_t rva, uint32_t a = 0, uint32_t c = 0, uint32_t d = 0, uint32_t b = 0, uint32_t s = 0,
               uint32_t s0 = 0, uint32_t s1 = 0, uint32_t s2 = 0, uint32_t s3 = 0, uint32_t s4 = 0, uint32_t s5 = 0, uint32_t s6 = 0) {
        if (L[0] >= kLogMax) abort();   // log capacity exceeded
        uint32_t* e = L + 1 + L[0]++ * 10;
        e[0] = rva; e[1] = a; e[2] = c; e[3] = d; e[4] = b; e[5] = s; e[6] = s0; e[7] = s1; e[8] = s2; e[9] = s3;
        // retail logs a pointer into its stack as 0xB3000000; `hookw` out-parameter writes ride in sc as key ((1+stack word)<<24)|rva, or ((0x10+r)<<24)|rva for register r (a c d b s = 0..4); several pairs with one key fill consecutive dwords of that pointee
        const uint32_t sw[7] = { s0, s1, s2, s3, s4, s5, s6 };
        for (int i = 1; i < 10; i++) if (isStk(e[i])) e[i] = 0xB3000000u;
        const uint32_t rg[5] = { a, c, d, b, s };
        for (int kind = 0; kind < 2; kind++) for (uint32_t w = 0; w < (kind ? 5u : 7u); w++) {
            const uint32_t key = ((kind ? 0x10u + w : 1u + w) << 24) | rva, ptr = kind ? rg[w] : sw[w]; uint32_t off = 0;
            for (const uint32_t* p = sc; p[0]; p += 2) if (p[0] == key) { wr32(ptr + off, p[1]); off += 4; }
        }
        int k = 0; while (k < nseen && seen[k][0] != rva) k++;
        if (k == nseen) { if (nseen >= 48) abort(); seen[nseen][0] = rva; seen[nseen++][1] = 0; }
        uint32_t n = seen[k][1]++, last = 0;
        for (const uint32_t* p = sc; p[0]; p += 2) if (p[0] == rva) { last = p[1]; if (!n--) return last; }
        return last;
    }
};

// Mirror of the descriptor's `gvar NAME RVA LEN [V...]` (the FN descriptor README): the capture pins blob NAME onto the
// module range [RVA, RVA+LEN) before the call and reads it back after. The probe registers each blob with add(rva, blobAddr, len)
// (blob k's address is r[14 + 2k]) and reaches a pinned global by its retail RVA: at(rva) is the pointer to those bytes (null outside every
// pin), rd/wr read and write a dword. The probe copies the pinned bytes into the port's own state before the call and back after it, so the
// blob holds what retail's global would hold. A range is looked up by RVA, so a probe can address a word inside a larger pin (0x110C08+4*i).
struct GlobalPins {
    struct R { uint32_t rva, len; uint8_t* p; };
    R r[8]; int n = 0;
    void add(uint32_t rva, uint32_t blobAddr, uint32_t len) { if (n >= 8) abort(); r[n++] = { rva, len, (uint8_t*)(uintptr_t)blobAddr }; }
    uint8_t* at(uint32_t rva, uint32_t len = 4) const { for (int i = 0; i < n; i++) if (rva >= r[i].rva && rva + len <= r[i].rva + r[i].len) return r[i].p + (rva - r[i].rva); return nullptr; }
    uint32_t rd(uint32_t rva) const { uint8_t* q = at(rva); if (!q) abort(); uint32_t v; memcpy(&v, q, 4); return v; }
    void wr(uint32_t rva, uint32_t v) const { uint8_t* q = at(rva); if (!q) abort(); memcpy(q, &v, 4); }
};
