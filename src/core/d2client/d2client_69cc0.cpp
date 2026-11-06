// 1.13c D2Client+0x69CC0 spawnRecords, written from the disassembly.
// Stack a, b, c (a struct pointer: word [c], byte [c+3] = count), d (flag), ret 0x10. A 0x104-byte name buffer is filled by
// 0x69410(EAX=sign-extended word [c], EDI=&out; b, &name); the name is looked up in table 0x1144C8 by 0x69350(EDX=&name; table) then
// 0x69970(EAX=&name; table) -- neither: fatal line 0xEC3. entry[+0x14] (null: 0xEC9) is the layer list; 0x6D6D0(EAX=list; 0, 0) gives the
// bytes (null: 0xED0). For each of the list's [byte +0] layers (layer >= 0x10: 0xED7) one 0x48-byte record {byte +5 = layer} per
// j in 0..c[3] (or one with j = 0 when c[3] is 0) is filled by 0x6DD30(EAX=out, ECX=entry, EDX=j, ESI=&rec; b, c, a), and when it
// succeeds is opened by 0xC0D0(&rec, 0, 0), or with d set by 0xC0D0(&rec, 1, 1) and, on 0, rec[+0x34] = 0 and 0xC118(&rec).
#include <cstring>
#include "cl_env.h"
namespace {
void fatal(const cl::Env& e, uint32_t line) {
    const uint32_t m = e.H(0xbf92, line);
    e.H(0xbf80, 0xB20D359Fu, m);
    e.H(0x3664, 0xffffffffu);
}
}
extern "C" void d2c_69cc0(uint32_t a, uint32_t b, uint32_t c, uint32_t d, const cl::Env& e) {
    uint32_t nb[65];
    memset(nb, 0, sizeof nb);
    uint32_t ev = 0;
    const uint32_t np = (uint32_t)(uintptr_t)nb, evp = (uint32_t)(uintptr_t)&ev;
    e.Ed(evp);
    e.H(0x69410, b, np, 0, 0, 0, (uint32_t)e.R16s(c), 0, 0, 0, 0);
    e.Edj(0x69410, evp);
    const uint32_t tbl = 0xB21144C8u;
    uint32_t entry = e.H(0x69350, tbl, 0, 0, 0, 0, 0, 0, np);
    if (e.blk) e.blk(e.ctx, 0x69350, np, 16);
    if (!entry) {
        entry = e.H(0x69970, tbl, 0, 0, 0, 0, np);
        if (e.blk) e.blk(e.ctx, 0x69970, np, 16);
        if (!entry) { fatal(e, 0xec3); return; }
    }
    const uint32_t list = e.R(entry + 0x14);
    if (!list) { fatal(e, 0xec9); return; }
    const uint32_t res = e.H(0x6d6d0, 0, 0, 0, 0, 0, list);
    if (!res) { fatal(e, 0xed0); return; }
    auto rec = [&](uint32_t layer, uint32_t j, uint32_t* fr, uint32_t* snap) {
        memset(fr, 0, 18 * 4);
        ((uint8_t*)fr)[5] = (uint8_t)layer;
        const uint32_t fp = (uint32_t)(uintptr_t)fr, sp = (uint32_t)(uintptr_t)snap;
        memcpy(snap, fr, 18 * 4);
        const uint32_t ok = e.H(0x6dd30, b, c, a, 0, 0, ev, entry, j, 0, fp);
        if (e.blk) e.blk(e.ctx, 0x6dd30, sp, 18);
        if (!ok) return;
        memcpy(snap, fr, 18 * 4);
        if (d) {
            const uint32_t r = e.H(0xc0d0, fp, 1, 1);
            snap[1] = 0;   // word 1 (byte 6) moves between runs in retail: masked in the capture
            if (e.blk) e.blk(e.ctx, 0xc0d0, sp, 18);
            if (!r) {
                fr[0x34 / 4] = 0;
                memcpy(snap, fr, 18 * 4);
                e.H(0xc118, fp);
                snap[1] = 0;
                if (e.blk) e.blk(e.ctx, 0xc118, sp, 18);
            }
        } else {
            e.H(0xc0d0, fp, 0, 0);
            snap[1] = 0;
            if (e.blk) e.blk(e.ctx, 0xc0d0, sp, 18);
        }
    };
    uint32_t fr[18], snap[18];
    const uint32_t n = e.R8(list);
    for (int32_t i = 0; n && i < (int32_t)n; ++i) {
        const uint32_t layer = e.R8(res + (uint32_t)i);
        if ((int32_t)layer >= 0x10) { fatal(e, 0xed7); return; }
        const uint32_t c3 = e.R8(c + 3);
        if (c3) {
            for (uint32_t j = 0; j < e.R8(c + 3) || j == 0; ) {
                rec(layer, j, fr, snap);
                ++j;
                if (!(j < e.R8(c + 3))) break;
            }
        } else {
            rec(layer, 0, fr, snap);
        }
    }
}
