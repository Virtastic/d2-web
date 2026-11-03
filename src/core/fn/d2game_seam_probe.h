// Probe-side half of d2game_seam4e.h: a D2Seam whose callees are HookEnv recorders. ret(rva, v) overrides a hook's return with a
// blob address (a descriptor `hook RVA N REGS @blob` value; the log still records the call).
#pragma once
#include "hookenv.h"
#include "../d2game/d2game_seam4e.h"
struct SeamProbe {
    HookEnv e; uint32_t ov[8][2]; int nov = 0;
    SeamProbe(uint32_t* lg, const uint32_t* sc) : e(lg, sc) {}
    void ret(uint32_t rva, uint32_t v) { ov[nov][0] = rva; ov[nov++][1] = v; }
    static uint32_t thunk(void* c, uint32_t rva, uint32_t a, uint32_t cc, uint32_t d, uint32_t b, uint32_t s, uint32_t s0, uint32_t s1, uint32_t s2, uint32_t s3, uint32_t s4, uint32_t s5, uint32_t s6, uint32_t) {
        SeamProbe* p = (SeamProbe*)c;
        uint32_t v = p->e.h(rva, a, cc, d, b, s, s0, s1, s2, s3, s4, s5, s6);
        for (int i = 0; i < p->nov; i++) if (p->ov[i][0] == rva) return p->ov[i][1];
        return v;
    }
    static void blkthunk(void* c, uint32_t rva, uint32_t p, uint32_t n) { ((SeamProbe*)c)->e.blk(rva, p, n); }
    D2Seam seam() { D2Seam s{ this, thunk }; s.bl = blkthunk; return s; }
};

// src/core/fn/d2game_seam_probe.h -- the recorder a D2Game FN probe binds to a port's D2GameSeam. It does what the
// capture's `hook RVA NST REGS V` does: appends (rva, eax, ecx, edx, ebx, esi, s0..s3) to the hooklog blob, logging only
// the registers/stack words the descriptor names, and returns the value the descriptor hands out. Addresses that fall in
// a probe blob are written as 0xB1<blob><offset>, as the capture records them.
#include <cstdint>
#include "../d2game/d2game_seam.h"
#include "hookenv.h"
struct D2ProbeHook { uint32_t rva; const char* regs; int nst; uint32_t ret; const uint32_t* seq = nullptr; int nseq = 0; mutable int used = 0; };
struct D2ProbeSeam {
    const uint32_t* r;            // the probe ABI array: blob k at r[14 + 2k]
    const uint32_t* size;         // blob sizes, by index
    int nblob;
    uint32_t* lg;                 // the hooklog blob
    const D2ProbeHook* hooks; int nhooks;
    uint32_t tag(uint32_t a) const {
        for (int k = 0; k < nblob; ++k) if (a >= r[14 + 2 * k] && a < r[14 + 2 * k] + size[k]) return 0xB1000000u | (uint32_t)k << 16 | (a - r[14 + 2 * k]);
        return HookEnv::isStk(a) ? 0xB3000000u : a;   // a caller-stack pointer (the request block) is logged as retail logs it
    }
    static uint32_t thunk(void* c, uint32_t rva, uint32_t eax, uint32_t ecx, uint32_t edx, uint32_t ebx, uint32_t esi,
                          uint32_t s0, uint32_t s1, uint32_t s2, uint32_t s3) {
        D2ProbeSeam* p = (D2ProbeSeam*)c;
        for (int i = 0; i < p->nhooks; ++i) {
            const D2ProbeHook& h = p->hooks[i];
            if (h.rva != rva) continue;
            uint32_t* w = p->lg + 1 + p->lg[0]++ * 10;
            for (int j = 0; j < 10; ++j) w[j] = 0;
            w[0] = rva;
            const uint32_t reg[5] = { eax, ecx, edx, ebx, esi }, st[4] = { s0, s1, s2, s3 };
            const char* names = "acdbs";
            for (int j = 0; j < 5; ++j) for (const char* q = h.regs; *q; ++q) if (*q == names[j]) w[1 + j] = p->tag(reg[j]);
            for (int j = 0; j < h.nst && j < 4; ++j) w[6 + j] = p->tag(st[j]);
            if (h.nseq) { const int k = h.used < h.nseq ? h.used : h.nseq - 1; h.used++; return h.seq[k]; }
            return h.ret;
        }
        return 0;
    }
    D2GameSeam seam() { return D2GameSeam{ this, thunk }; }
};
