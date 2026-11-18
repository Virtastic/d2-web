// 1.13c D2Client+0x8CFB0 stat-point / skill-point button count text (stack kind: 0 stat 0xF, 1 skill 0xE, other; ret 4). Globals (values in G): viewed unit 0x11BBFC, left
// 0x11B9A0, bottom 0x11B9A4, screen width 0xDBC48, height 0xDBC4C, panel flag 0x119854, mouse x 0x11B828 / y 0x11B824, flags
// 0x11BE7C (kind 1) / 0x11BE80 (kind 0), 0x11BC7C (word 13 of the 0xD096 block). Order: old = 0xD3A8(ECX 1); a 60-char and later a
// 20-char Unicode buffer (??_FUnicode, IAT 0xCEB00); an 18-word block zeroed. Kind 0: v = 0xC1C6(unit, 0xF, 0); x = L + 0xA5,
// y = L + 0x49; s = B + H, top = s - 0x1B8 / bottom = s - 0x1B6 with the panel flag, else s - 0xF4 / s - 0xF2; the mouse in
// [L+0x49, L+0x5D] x [bottom-0x12, bottom] runs 0xD384(ECX 0xD1EC(0x101C), EDX L+0x35; s - 0x1CA or s - 0x106, 0, 0). Kind 1:
// v = 0xC1C6(unit, 0xE, 0); d = W - L, x = d - 0xD4, y = d - 0xED, top = B + H - 0x48, bottom = B + H - 0x45. Other kinds:
// v = 0xC1C6(unit, 0xF, 0), x = 0xD35A(ECX 0xD1EC(0xCF3)) + L + 0x15, top = B + H - 0x6A, y = bottom = 0; kind 3 skips the text.
// Text: Storm sprintf (0xBEA2, cdecl)(buf, 0x14, "%d", v), win2Unicode (IAT 0xCEAC0; ECX wide, EDX buf; 0x13), kind 2 places it
// at L - 0xD35A(text) + 0xC6, then 0xD37E(ECX text, EDX x; top, 0, 0). Kinds 0 / 1: the block's word 0 = the flag, word 13 =
// [0x11BC7C], 0xD096(&block, y + 2 / y + 1, bottom - 2 (+1 with the flag), -1, 5, 0). Others: 0xD37E(ECX 0xD1EC(0xCF3), EDX
// L + 0x15; top, 0, 0). Last 0xD3A8(ECX old).
#include <cstdint>
#include <emscripten.h>
#include "d2client_callee.h"

struct D2c8cfb0G { uint32_t unit; int32_t L, B, W, H; uint32_t panel; int32_t mx, my; uint32_t f7c, f80, c7c; };
static uint32_t P(const void* p) { return (uint32_t)(uintptr_t)p; }

extern "C" EMSCRIPTEN_KEEPALIVE void d2c_8cfb0(const D2cSvc& v, const D2c8cfb0G* g, uint32_t kind) {
    const uint32_t old = d2c_call(v, 0xd3a8, 0, 1, 0, 0, 0);
    char16_t big[0x3c] = {}, txt[0x14] = {};
    char buf[0x14] = {};
    uint32_t blk[0x12] = {};
    (void)big;
    uint32_t val, flag = 0;
    int32_t x, top, y = 0, bot = 0;
    const int32_t L = g->L;
    bool text = true;
    if (kind == 0) {
        val = d2c_call(v, 0xc1c6, 0, 0, 0, 0, 0, g->unit, 0xf, 0);
        x = L + 0xa5;
        const int32_t s = g->B + g->H;
        if (g->panel) { top = s - 0x1b8; bot = s - 0x1b6; } else { top = s - 0xf4; bot = s - 0xf2; }
        flag = g->f80 != 0;
        y = L + 0x49;
        if (g->mx >= y && g->mx <= y + 0x14 && g->my >= bot - 0x12 && g->my <= bot) {
            const uint32_t e = d2c_call(v, 0xd1ec, 0, 0x101c, 0, 0, 0);
            d2c_call(v, 0xd384, 0, e, (uint32_t)(L + 0x35), 0, 0, (uint32_t)(g->panel ? s - 0x1ca : s - 0x106), 0, 0);
        }
    } else if (kind == 1) {
        val = d2c_call(v, 0xc1c6, 0, 0, 0, 0, 0, g->unit, 0xe, 0);
        const int32_t d = g->W - L;
        x = d - 0xd4; top = g->B + g->H - 0x48; y = d - 0xed; bot = g->B + g->H - 0x45;
        flag = g->f7c != 0;
    } else {
        val = d2c_call(v, 0xc1c6, 0, 0, 0, 0, 0, g->unit, 0xf, 0);
        const uint32_t e = d2c_call(v, 0xd1ec, 0, 0xcf3, 0, 0, 0);
        x = (int32_t)d2c_call(v, 0xd35a, 0, e, 0, 0, 0) + L + 0x15;
        top = g->B + g->H - 0x6a;
        text = kind != 3;
    }
    if (text) {
        d2c_call(v, 0xbea2, 0, 0, 0, 0, 0, P(buf), 0x14, 0xB20D057Cu, val);   // "%d" at D2Client+0xD057C
        d2c_call(v, 0xceac0, 0, P(txt), P(buf), 0, 0, 0x13);
        const int32_t tx = kind == 2 ? L - (int32_t)d2c_call(v, 0xd35a, 0, P(txt), 0, 0, 0) + 0xc6 : x;
        d2c_call(v, 0xd37e, 0, P(txt), (uint32_t)tx, 0, 0, (uint32_t)top, 0, 0);
    }
    if (kind == 0 || kind == 1) {
        if (kind == 0 ? g->f80 : g->f7c) ++bot;
        bot -= 2; y += kind == 0 ? 2 : 1;
        blk[0] = flag; blk[13] = g->c7c;
        d2c_call(v, 0xd096, 0, 0, 0, 0, 0, P(blk), (uint32_t)y, (uint32_t)bot, 0xffffffffu, 5, 0);
    } else {
        const uint32_t e = d2c_call(v, 0xd1ec, 0, 0xcf3, 0, 0, 0);
        d2c_call(v, 0xd37e, 0, e, (uint32_t)(L + 0x15), 0, 0, (uint32_t)top, 0, 0);
    }
    d2c_call(v, 0xd3a8, 0, old, 0, 0, 0);
}
