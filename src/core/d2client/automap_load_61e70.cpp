// Implementation batch (alpha lane): D2Client+0x61E70, automap save-file section loader. Decompile+disassembly derived; not yet vectored.
// Callees go through D2cHost in retail's call order. Seam: 0x5F3E0 open->handle (-1 fail), 0xC034 read(ecx=h,edx=buf,s0=n,s1=&got,s2=0),
// 0xC028 seek(ecx=h,edx=off,s0,s1), 0xC03A close(ecx=h), 0xBF7A alloc(ecx=n,edx=file,s0=line,s1=0), 0xBF86 free(ecx=p,edx=file,s0=line,s1=0),
// 0xBF92 assert(s0=line), 0x5F290(esi=h,edi=&hdr,s0=pos) abort-clear, 0x5F6B0(ecx=root+off) new 20-byte entry, 0x61320(ecx=node,edx=tree) AVL insert.
#include <cstdint>
#include <cstring>
#include <emscripten.h>
#include "client_host.h"
typedef int32_t i32; typedef uint32_t u32; typedef uint16_t u16;
struct D2cLoadG {
    i32 **root /*1CC1C4: -> header {+0 key,+8..+0x14 four tree roots}*/, *thr /*1CC1D8: 4 thresholds*/, **c3bc /*1CC3BC*/;
    const char* file; /* "Source\\D2Client\\UI\\automap.cpp" string as retail's edx */
};
namespace {
inline u32 go(D2cHost h, u32 rva, u32 eax, u32 ecx, u32 edx = 0, u32 s0 = 0, u32 s1 = 0, u32 s2 = 0, u32 esi = 0, u32 edi = 0) {
    D2cCall c = {}; c.rva = rva; c.eax = eax; c.ecx = ecx; c.edx = edx; c.esi = esi; c.s[0] = s0; c.s[1] = s1; c.s[2] = s2; (void)edi; return h(&c);
}
inline u32 P(const void* p) { return (u32)(uintptr_t)p; }
}  // namespace
extern "C" {
// 1.13c D2Client+0x61E70
EMSCRIPTEN_KEEPALIVE void d2c_61e70(D2cHost h, D2cLoadG* g) {
    if (*g->root == nullptr) return;
    i32 f = (i32)go(h, 0x5F3E0, 0, 0);
    if (f == -1) return;
    i32 hdr[100] = {}, got = 0;
    go(h, 0xC034, 0, f, P(hdr), 0x190, P(&got), 0);
    if (got != 0x190) { go(h, 0xC03A, 0, f); return; }
    i32 next = hdr[**g->root], pos = 0;
    if (next == 0) { go(h, 0xC03A, 0, f); return; }
    static const u32 secOff[4] = {8, 0xC, 0x10, 0x14};
    static const u32 lines[4][3] = {{0x575, 0x57a, 0x586}, {0x58a, 0x58f, 0x59b}, {0x5a2, 0x5a7, 0x5b3}, {0x5bd, 0x5c2, 0x5ce}};
    do {
        i32 v[8] = {};  // v[0]=next v[1]=key v[2]=sel v[3]=tag v[4..7]=section byte sizes
        go(h, 0xC028, 0, f, (u32)next, 0, 0);
        for (int k = 0; k < 8; k++) { got = 0; go(h, 0xC034, 0, f, P(&v[k]), 4, P(&got), 0); }
        if (v[1] != **g->root) { go(h, 0x5F290, 0, 0, 0, (u32)pos, 0, 0, (u32)f, P(hdr)); go(h, 0xC03A, 0, f); return; }
        for (int s = 0; s < 4; s++) {
            i32 sz = v[4 + s];
            if (sz == 0) continue;
            if (s == 2 && v[3] != (*g->c3bc)[1]) { go(h, 0xC028, 0, f, (u32)P(*g->c3bc), 0, 1); continue; }
            u16* b = (u16*)(uintptr_t)go(h, 0xBF7A, 0, (u32)sz, P(g->file), lines[s][0], 0);
            got = 0;
            go(h, 0xC034, 0, f, P(b), (u32)sz, P(&got), 0);
            if (got != sz) { go(h, 0xBF92, 0, 0, 0, lines[s][1]); return; }
            i32 thr = s == 3 ? g->thr[v[2]] : (s == 2 ? g->thr[0] : g->thr[0]);
            for (u32 i = 0; i < (u32)sz / 2; i += 3) {
                if ((int16_t)b[i] >= thr) { go(h, 0x5F290, 0, 0, 0, (u32)pos, 0, 0, (u32)f, P(hdr)); break; }
                u16* n = (u16*)(uintptr_t)go(h, 0x5F6B0, 0, 0);
                n[2] = b[i]; n[3] = b[i + 1]; n[4] = b[i + 2]; *(i32*)n = 1;
                go(h, 0x61320, 0, P(n), P(*g->root) + secOff[s]);
            }
            go(h, 0xBF86, 0, P(b), P(g->file), lines[s][2], 0);
        }
        pos = next; next = v[0];
    } while (next != 0);
    go(h, 0xC03A, 0, f);
}
}
