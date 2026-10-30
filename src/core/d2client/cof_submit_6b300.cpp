// 1.13c D2Client+0x6B300 -- COF component submit (env-seam port; callees/globals injected, no retail data).
// Stack ABI (ret 0x1c): a0 unit, a1 type, a2 rand-shift-8, a3..a5 node 8/c/10 (a3 -> +0x38 for type 8),
// a6 rand-shift-4; EAX = component selector.
#include <cstdint>
#include <emscripten.h>
typedef int32_t i32; typedef uint32_t u32;

struct D2cCofEnv {
    u32 (*rd32)(u32 addr);
    void (*wr32)(u32 addr, u32 v);
    i32 (*ord10227)();                                   // 0x6fabbf8c
    u32 (*f1510)(u32 a, u32 b);                          // D2Client+0x1510 (eax,edx)
    u32 (*f19130)(u32 unit, u32 sel);                    // 0x69130
    void (*f19660)(u32 sel);                             // 0x69660 (edx)
    u32 (*alloc)(u32 size, u32 line);                    // Fog alloc (ECX = 0xa8 bytes; 0xc48 is the source line)
    u32 (*f1e9c0)(u32 z, u32 node, u32 unit);            // 0x6E9C0
    i32 (*ord10172)(u32 unit);
    u32 (*f1380)(u32 seedPtr, u32 mul);                  // D2Client+0x1380 (ecx,esi)
    u32 (*f58c20)(u32 unit, u32 one, u32 a, u32 b, u32 c, u32 d, u32 e); // 0x58C20 (EDI = unit, EBX = 1, 5 stack)
    void (*f58be0)(u32 v);
    void (*f18ae0)(u32 node);
    void (*fatal)(i32 line);                             // does not return in retail
};

extern "C" {
// 1.13c D2Client+0x6B300
EMSCRIPTEN_KEEPALIVE void d2c_6b300(const D2cCofEnv* e, u32 sel, u32 unit, u32 type, u32 a2, u32 a3,
                                    u32 a4, u32 a5, u32 a6) {
    if ((i32)sel < 0) return;
    u32 dt = e->rd32(e->rd32(0x6fb7e474));
    if ((i32)sel >= (i32)e->rd32(dt + 0xbc0)) return;
    u32 row = sel * 0x84 + e->rd32(dt + 0xbbc);
    if (!row) return;
    if (!e->ord10227() && (int16_t)(e->rd32(row + 0x40) >> 16) >= 100) return;
    u32 g = e->rd32(0x6fbcbbfc);
    if (unit) {
        if ((e->rd32(unit + 0xc8) >> 0x12) & 1) return;
        if (e->rd32(unit) == 1 && e->f1510(e->rd32(unit + 4), 2)) return;
    }
    if (type == 8 && e->f19130(unit, sel)) return;
    if ((i32)sel < 0x8c || (sel > 0x8e && sel != 0x9e)) e->f19660(sel);
    u32 n = e->alloc(0xa8, 0xc48);
    if (!n) { e->fatal(0xc49); return; }
    for (u32 i = 0; i < 0xa8; i += 4) e->wr32(n + i, 0);
    e->wr32(n, type); e->wr32(n + 4, sel);
    e->wr32(n + 0x1c, e->f1e9c0(0, n, unit));
    if (type == 2) e->wr32(n + 8, 1);
    else if (type == 6) e->wr32(n + 0x18, e->f1380(g + 0x20, e->rd32(n + 0x1c)));
    else { e->wr32(n + 8, a3); e->wr32(n + 0xc, a4); e->wr32(n + 0x10, a5); }
    e->wr32(n + 0x20, e->rd32(row + 0x54));
    u32 y = e->rd32(row + 0x58);
    e->wr32(n + 0x24, y);
    switch (e->ord10172(unit)) {
    case -1: y += 0x4b; break;
    case 0: y += e->rd32(row + 0x5c); break;
    case 1: y += e->rd32(row + 0x60); break;
    case 2: y += e->rd32(row + 0x64); break;
    case 3: y += e->rd32(row + 0x68); break;
    default: break;
    }
    e->wr32(n + 0x24, y);
    if (e->rd32(row + 0x74)) {
        u32 r = e->f58c20(unit, 1, e->rd32(row + 0x70), 0xff, e->rd32(row + 0x7d) & 0xff, e->rd32(row + 0x7e) & 0xff,
                          e->rd32(row + 0x7f) & 0xff);
        e->wr32(n + 0x40, r);
        if (e->rd32(row + 0x74) != e->rd32(row + 0x70)) e->f58be0(r);
    }
    u32 pool = unit ? e->rd32(unit + 0x54) : 0;
    if (!pool) { e->fatal(0xc7a); return; }
    e->wr32(n + 0x34, e->rd32(row + 0x48) & 0xff);
    e->wr32(n + 0xa4, e->rd32(pool + 0x2c));
    e->wr32(pool + 0x2c, n);
    if (type == 8) { e->wr32(n + 0x38, a3); e->f18ae0(n); }
    if (a2) e->wr32(n + 0x18, e->f1380(g + 0x20, a2 << 8));
    u32 f = e->rd32(row + 0x6c) << 4;
    if (a6) f += e->f1380(g + 0x20, a6 << 4);
    e->wr32(n + 0x14, f);
    e->wr32(n + 0x28, e->rd32(row + 0x78));
}
}
