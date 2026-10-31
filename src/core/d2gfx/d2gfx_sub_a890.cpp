#include <cstdint>
#include <cstdlib>
#include "d2gfx_blit.h"

extern "C" void d2gfx_sub_8e30(const uint8_t*, const uint8_t*, uint32_t, uint8_t*, int32_t);
struct D2GfxRleClip { int32_t stride, top, bottom, left, right; const uint8_t* palette; };
void d2gfx_rle_clipped(const uint8_t*, int32_t, int32_t, uint8_t*, int32_t, const D2GfxRleClip&);

// The callees as seams: 0x8E30 (EAX = stream, ECX = rows, ESI = palette, [dst]; pitch from its global), 0xA310
// (ECX = stream, ESI = palette, [x, y, dst, rows]), and the null-surface report 0x783C/0x7836 (string 0x373) + 0x150B.
struct D2GfxA890Env {
    void (*blit)(const uint8_t* src, const uint8_t* pal, uint32_t rows, uint8_t* dst, int32_t pitch);
    void (*clip)(const uint8_t* src, const uint8_t* pal, int32_t x, int32_t y, uint8_t* dst, uint32_t rows,
                 const D2GfxBlitState* st);
    void (*error)(uint32_t id);
    void (*exit_)(int32_t code);
};

// 1.13c D2Gfx+0x0A890: draw one 32-row RLE tile at (x, y) on the 8-bit surface. fastcall(ECX = stream, EDX = x,
// [y], [sel]) ret 8; the palette is the shade-table row tab + (sel >> 3) * 256 (tab = 0x6FA91678, sel's low byte).
// Outside the outer clip: nothing. A null surface base reports string 0x373 and exits (past the call retail would run
// on clobbered EDX/ECX, so there is nothing to follow). Inside the inner rect: 0x8E30, else the per-run clipper 0xA310.
// 1.13c D2Gfx+0x0A890
extern "C" void d2gfx_sub_a890_fn(const uint8_t* src, int32_t x, int32_t y, uint32_t sel, const uint8_t* tab,
                                  const D2GfxBlitState* st, const D2GfxA890Env* e) {
    if (y < st->yTop || y >= st->yBot || x < st->xLeft || x >= st->xRight) return;
    const uint8_t* pal = tab + (((sel & 0xff) >> 3) << 8);
    uintptr_t base = (uintptr_t)st->base;
    uint8_t* dst = (uint8_t*)(base + (uintptr_t)(intptr_t)st->rowOff[y] + (uintptr_t)(intptr_t)x);
    if (!base) { e->error(0x373); e->exit_(-1); return; }
    if (y >= st->inY0 && y < st->inY1 && x >= st->inX0 && x < st->inX1) { e->blit(src, pal, 0x20, dst, st->pitch); return; }
    e->clip(src, pal, x, y, dst, 0x20, st);
}

namespace {
void a890Blit(const uint8_t* s, const uint8_t* p, uint32_t n, uint8_t* d, int32_t pitch) { d2gfx_sub_8e30(s, p, n, d, pitch); }
void a890Clip(const uint8_t* s, const uint8_t* p, int32_t x, int32_t y, uint8_t* d, uint32_t n, const D2GfxBlitState* st) {
    D2GfxRleClip c{st->pitch, st->c2Top, st->c2Bottom, st->c2Left, st->c2Right, p};
    d2gfx_rle_clipped(s, x, y, d, (int32_t)n, c);
}
void a890Error(uint32_t) {}
void a890Exit(int32_t) { abort(); }
}

// The play-path entry: the caller has already picked the palette row (pal = tab + (sel >> 3) * 256).
extern "C" void d2gfx_sub_a890(const uint8_t* src, int32_t x, int32_t y, const uint8_t* palette, const D2GfxBlitState* st) {
    static const D2GfxA890Env env{a890Blit, a890Clip, a890Error, a890Exit};
    d2gfx_sub_a890_fn(src, x, y, 0, palette, st, &env);
}
