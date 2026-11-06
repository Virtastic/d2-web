// 1.13c D2Common+0x5E870 (ordinal 11023): tear down every loaded table in a fixed order. The callees and the three Fog thunks
// are reached through D2Common5E870Env so a replay can log them; slots are the retail module words (D2Common5E870Slots).
// Delta vs 1.10f: none claimed, shape only (D2MOO DATATBLS_UnloadAllBins); order checked against the 1.13c decompile.
#include "d2common_5e870.h"

namespace {
struct Ctx { D2Common5E870Slots* s; const D2Common5E870Env* e; };
// The guarded "still allocated" free of a DataTbls.cpp block: line 0x8F8 and the block itself while the flag is clear,
// line 0x8F6 and block-4 while it is set.
void leak(const Ctx& c, uint32_t v) {
    if (!v) return;
    if (c.s->flag == 0) c.e->free9228(v, D2C5E870_STR_DATA, 0x8f8);
    else c.e->free9228(v - 4, D2C5E870_STR_DATA, 0x8f6);
}
void guarded(const Ctx& c, uint32_t v, uint32_t str, uint32_t line) { if (v) c.e->free9228(v, str, line); }
}

// 1.13c D2Common+0x5E870
extern "C" void d2_seam_d2common_5e870(D2Common5E870Slots* s, const D2Common5E870Env* e) {
    const Ctx c{ s, e };
    e->call(0x5de00);
    leak(c, s->s_a9c);
    e->call(0x7c4f0);
    e->free9246(s->s_a90);
    leak(c, s->s_a94); s->s_a94 = 0;
    e->call(0x5db60); e->call(0x71330); e->call(0x5d8b0);
    leak(c, s->s_ac0);
    e->free9246(s->s_ac4);
    e->call(0x71a50);
    leak(c, s->s_f7c);
    e->free9246(s->s_f80);
    guarded(c, s->s_bbc, D2C5E870_STR_ITEM, 0x2b7); s->s_bbc = 0;
    guarded(c, s->s_be0, D2C5E870_STR_ITEM, 0x30b); s->s_be0 = 0;
    e->free9246(s->s_af8);
    leak(c, s->s_afc); s->s_b00 = 0;
    e->call(0x716b0);
    leak(c, s->s_d94); leak(c, s->s_d9c); leak(c, s->s_bd0); leak(c, s->s_da4); leak(c, s->s_d8c);
    if (s->s_bf4) { e->free9228(s->s_bf4, D2C5E870_STR_ITEM, 0x80d); s->s_bf4 = 0; s->s_bf0 = 0; }
    e->call(0x71180); e->call(0x53bd0); e->call(0x18f90);
    leak(c, s->s_b50);
    if (s->s_b4c) {
        e->anim922e(e->rd32(s->s_b4c), D2C5E870_STR_ANIM, 0xbb, 0);
        e->free9228(s->s_b4c, D2C5E870_STR_ANIM, 0xbc);
    }
    e->call(0x54200); e->call(0x6c7e0);
    leak(c, s->s_b38);
    guarded(c, s->s_1420, D2C5E870_STR_LVL, 0x1d9);
    leak(c, s->s_141c);
    e->call(0x6c9c0);
    leak(c, s->s_142c); leak(c, s->s_1434);
    guarded(c, s->s_1448, D2C5E870_STR_LVL, 0x44b);
    e->call(0x6c840);
    guarded(c, s->s_1454, D2C5E870_STR_LVL, 0x562); s->s_1454 = 0;
    leak(c, s->s_b94); s->s_b94 = 0; s->s_b98 = 0;
    leak(c, s->s_b9c); s->s_b9c = 0; s->s_ba0 = 0;
    leak(c, s->s_b8c); s->s_b8c = 0; s->s_b90 = 0;
    leak(c, s->s_fa5c); leak(c, s->s_bd8);
    e->free9246(s->s_f88);
    guarded(c, s->s_1404, D2C5E870_STR_FIELD, 0x5b);
    s->s_1404 = 0; s->s_140c = 0; s->s_1408 = 0; s->s_1410 = 0; s->s_1414 = 0; s->s_1418 = 0;
    leak(c, s->s_13dc); leak(c, s->s_b60); leak(c, s->s_1400); s->s_1400 = 0;
    leak(c, s->s_1464); leak(c, s->s_13f8); s->s_13f8 = 0;
    leak(c, s->s_b54);
}
