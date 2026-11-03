#include <emscripten.h>
#include "d2cf.h"
using namespace d2cf;
// 1.13c D2Client+0x69410: EAX = monster-type index, EDI = out dword*, stack (mode, buf*). rec = sgptDataTables
// ->+0xA78 + idx*0x1A8 (idx < ->+0xA80); st = 0x1260(idx); cb = Ordinal_10470(mode, 1) (null is fatal 0xE86). *EDI = (mode 0 or 0xC and not (*gdwBitMasks & st[6]))
// ? 'ht ' : st[0x10]. V = cb[0x20], overridden by every DAT_F9EAC[i] pair whose key (+4) is mode. Writes
// "DATA\\GLOBAL\\MONSTERS\\A\\COF\\ABC.COF" (A = rec[0x10]'s first 3 chars, B = V's, C = *EDI's; a space ends a name) into buf.
extern "C" EMSCRIPTEN_KEEPALIVE void d2cf_69410(uint32_t idx, uint32_t outp, uint32_t mode, uint32_t buf, uint32_t tab, uint32_t bm, uint32_t gl, const Env* e) {
    if ((int32_t)idx < 0 || (int32_t)idx >= (int32_t)R(tab, 0xA80)) return;
    uint32_t rec = R(tab, 0xA78) + idx * 0x1A8;
    if (!rec) return;
    uint32_t st = e->call(0x1260, idx, 0, 0, 0, 0, 0, 0);
    if (!st) return;
    uint32_t u = R(rec, 0x10);
    uint32_t cb = e->call(0xCBD4, 0, 0, 0, 0, 0, mode, 1);
    if (!cb) { e->fatal(0xE86); return; }
    uint32_t v = R(cb, 0x20);
    if (mode == 0 || mode == 0xC) W(outp, 0) = (B(bm, 0) & B(st, 6)) ? R(st, 0x10) : 0x20687468u;
    else W(outp, 0) = R(st, 0x10);
    for (uint32_t i = 0; i < R(gl, 0); ++i) if (R(gl, 4 + i * 8 + 4) == mode) v = R(gl, 4 + i * 8);
    char a[4], b[4], c[4];
    for (int i = 0; i < 3; ++i) {
        uint8_t x = (uint8_t)(u >> (i * 8)), y = (uint8_t)(v >> (i * 8)), z = B(outp, i);
        a[i] = (char)(x == 0x20 ? 0 : x); b[i] = (char)(y == 0x20 ? 0 : y); c[i] = (char)(z == 0x20 ? 0 : z);
    }
    a[3] = b[3] = c[3] = 0;
    uint32_t o = 0;
    auto cat = [&](const char* s) { while (*s) B(buf, o++) = (uint8_t)*s++; };
    cat("DATA\\GLOBAL\\MONSTERS\\"); cat(a); cat("\\COF\\"); cat(a); cat(b); cat(c); cat(".COF");
    B(buf, o) = 0;
}
