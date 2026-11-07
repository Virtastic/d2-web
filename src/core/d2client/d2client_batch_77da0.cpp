// src/core/d2client/d2client_batch_77da0.cpp -- 12 UNMAPPED 1.13c D2Client.dll rows (alpha lane),
// from build-wasm/ghidra-d2client/decomp/0x<RVA>.c. Base 0x6FAB0000. Globals are parameters; ordinals and
// unrecovered callees are host callbacks after the retail arguments.
// GUESSED ARGUMENTS (decompile shows unaff_/in_ registers): 77F30 (iStack_8 = class, uninitialised),
// 79C80 (EDI=ctx, EBX=obj, ESI=x, EBP=y), 79D30 (EAX=obj, EBX=y, EBP=x; ECX=ctx), 7A0D0 (ESI=ctx, EBP),
// 7A180/7A260 (EAX=obj, EBX=ctx), 7A500 (ECX=ctx, EAX=obj, EBX=list). Void ordinal calls whose arguments are
// in hidden registers (10003, 10028) take none. 7A0D0's return is the leftover mul-high operand, kept as int32.
#include <cstdint>
#include <emscripten.h>
#include "d2client_callee.h"

static inline uint32_t rd32(uint32_t a) { return *(const uint32_t*)(uintptr_t)a; }
static inline void wr32(uint32_t a, uint32_t v) { *(uint32_t*)(uintptr_t)a = v; }
static inline int16_t rd16(uint32_t a) { return *(const int16_t*)(uintptr_t)a; }

// old form of D2Client 77DA0 (now d2client_77da0.cpp) (56 B): clear every state==1 slot of the table at 0x6FBC3D38.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_77da0(uint32_t unit_bbfc, int32_t* tab_3d38, int (*ord11120)(uint32_t)) {
    if (!unit_bbfc) return;
    for (int i = 0; i < ord11120(rd32(unit_bbfc + 4)); i++)
        if (tab_3d38[i] == 1) tab_3d38[i] = 0;
}

// 1.13c D2Client+0x77F30 (665 B): hit-test the mouse against the three button rows for `cls` and draw
// the highlight box + three text calls on a hit.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_77f30(int32_t cls, int32_t mouse_x_b828, int32_t mouse_y_b824, int32_t w_bc48, int32_t x0_b9a0,
                     int32_t y0_b9a4, int32_t y1_bc4c, uint8_t* col_bfcc,
                     uint8_t (*ord10190)(int, int, int), void (*ord10184)(void),
                     void (*ord10014)(int, int, int, int, int), void (*ord10003)(void),
                     void (*ord10028)(void), void (*ord10150)(int, int, int)) {
    int32_t x = mouse_x_b828, y = mouse_y_b824, sel = 0;
    bool hit = false;
    int32_t L = w_bc48 - x0_b9a0, base = y0_b9a4 + y1_bc4c;
    *col_bfcc = ord10190(0, 0, 0);
    if (x >= L - 0x59 && x <= L + 1) {
        int32_t v;
        if (cls == 1) {
            if (y <= base - 0x9c && base - 0x108 <= y) { hit = true; sel = base - 0x108; }
            else if (!(base - 0x108 < y || y < base - 0x174)) { hit = true; sel = base - 0x174; }
        } else if (cls == 2 || cls == 3) {
            v = base - 0x9c;
            if (cls == 2) {
                if (y <= base - 0x108 && base - 0x174 <= y) { hit = true; sel = base - 0x174; }
                else if (y <= base - 0x30 && v <= y) { hit = true; sel = v; }
            } else {
                if (y <= v && base - 0x108 <= y) { hit = true; sel = base - 0x108; }
                else if (y <= base - 0x30 && v <= y) { hit = true; sel = v; }
            }
        }
    }
    ord10184();
    if (hit) {
        uint32_t c = *col_bfcc;
        ord10014(L - 0x59, sel + 5, L + 1, sel + 0x19, c);
        ord10003(); ord10028(); ord10150(c + 0x14, 0, 0);
    }
}

// 1.13c D2Client+0x781D0 (77 B) fastcall(ecx unused, edx=off): one text draw at off-0x1e0+base.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_781d0(int32_t off, int32_t y0_b9a4, int32_t y1_bc4c, void (*ord10028)(void),
                     void (*ord10150)(int, int, int)) {
    int32_t base = y0_b9a4 + y1_bc4c;
    ord10028();
    ord10150(off - 0x1e0 + base, 0, 0);
}

// 1.13c D2Client+0x78330 (1174 B): three centred labels (D2Lang strings 0x1083..0x1085, drawn through D2Win 0xD37E), then a
// per-class list of (string id, x) pairs, each drawn by sub_781D0 (EDX = x, EBX = the D2Lang string).
// cls = byte [0x11991C], b8bc4c = [0xDBC4C], b9a0/b9a4 = [0x11B9A0]/[0x11B9A4], f9e14 = [0xF9E14].
struct L78330 { uint16_t id, x; };
static void lbl78330(const D2cSvc& v, uint32_t id, int32_t yb, int32_t b8bc4c, int32_t b9a0, int32_t b9a4, int32_t f9e14) {
    uint32_t text = d2c_call(v, 0xd1ec, 0, id, 0, 0, 0);
    int32_t y = b9a4 + b8bc4c - yb, ax = f9e14 - b9a0, x = ax - 0x5a, w = 0x5b;
    int32_t tw = (int32_t)d2c_call(v, 0xd35a, 0, text, 0, 0, 0);
    if (tw < w) x += (w - tw) >> 1;
    d2c_call(v, 0xd37e, 0, text, (uint32_t)x, 0, 0, (uint32_t)y, 0, 0);
}
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_78330(const D2cSvc& v, uint8_t cls, int32_t b8bc4c, int32_t b9a0, int32_t b9a4, int32_t f9e14) {
    lbl78330(v, 0x1083, 0x1c7, b8bc4c, b9a0, b9a4, f9e14);
    lbl78330(v, 0x1084, 0x1bb, b8bc4c, b9a0, b9a4, f9e14);
    lbl78330(v, 0x1085, 0x1af, b8bc4c, b9a0, b9a4, f9e14);
    static const L78330 c0[] = { {0x1088,0x96},{0x1089,0xa2},{0x1086,0xae},{0x108a,0x102},{0x108b,0x10e},{0x1086,0x11a},{0x108c,0x167},{0x108d,0x173},{0x1086,0x17f} };
    static const L78330 c3[] = { {0x108e,0x9c},{0x108f,0xa8},{0x1090,0x108},{0x108f,0x114},{0x1091,0x172},{0x1086,0x17e} };
    static const L78330 c2[] = { {0x1092,0x9c},{0x1087,0xa8},{0x1093,0x102},{0x1094,0x10e},{0x1087,0x11a},{0x1095,0x178} };
    static const L78330 c4[] = { {0x1096,0xa2},{0x1097,0x108},{0x1098,0x114},{0x1097,0x172},{0x1086,0x17e} };
    static const L78330 c1[] = { {0x1099,0x9c},{0x1087,0xa8},{0x109a,0x108},{0x1087,0x114},{0x109b,0x172},{0x1087,0x17e} };
    static const L78330 c6[] = { {0x57f4,0x96},{0x57f5,0xa2},{0x57f2,0x108},{0x57f3,0x114},{0x57f1,0x172} };
    static const L78330 c5[] = { {0x57f0,0xa2},{0x57ee,0x108},{0x57ef,0x114},{0x57ed,0x172} };
    const L78330* l = nullptr; unsigned n = 0;
    switch (cls) {
    case 0: l = c0; n = sizeof c0 / sizeof *c0; break;
    case 3: l = c3; n = sizeof c3 / sizeof *c3; break;
    case 2: l = c2; n = sizeof c2 / sizeof *c2; break;
    case 4: l = c4; n = sizeof c4 / sizeof *c4; break;
    case 1: l = c1; n = sizeof c1 / sizeof *c1; break;
    case 6: l = c6; n = sizeof c6 / sizeof *c6; break;
    case 5: l = c5; n = sizeof c5 / sizeof *c5; break;
    }
    for (unsigned k = 0; k < n; ++k) {
        uint32_t text = d2c_call(v, 0xd1ec, 0, l[k].id, 0, 0, 0);
        d2c_call(v, 0x781d0, 0, 0, l[k].x, text, 0);
    }
}

// 1.13c D2Client+0x78AA0 (432 B, cdecl, no input): the skills-page slot scan. unit = [0xBBFC] (0 = nothing), flag = [0xBFC0],
// x/y from [0xDBC48] [0xDBC4C] [0x11B9A0] [0x11B9A4], page = [0x924F8], ds = the D2Common data-table struct (reached through the
// IAT slot at 0xCE474; +0xB8C skill-desc rows (0x120 B, page byte +2), +0xB94 their count, +0xB98 skill rows (0x23C B, word +0x194
// = desc index), +0xBA0 their count), a3d38/a3b38 = the two per-slot arrays at 0x113D38 / 0x113B38 (written).
// Callees by RVA: 0xCB38 (inventory slot count, 1 stack word), 0xCB3E (skill id of slot k), 0xC4EA/0xC718 (skill lookup / level), 0xD35A/0xD37E (text), 0x78820.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_78aa0(const D2cSvc& v, uint32_t unit, int32_t flag, int32_t b9a4, int32_t bc4c, int32_t bc48, int32_t b9a0,
                     uint32_t page, const uint8_t* ds, uint32_t* a3d38, uint32_t* a3b38) {
    if (!unit) return;
    if (flag == 0) {
        uint16_t wbuf[10] = { '0', 0 };   // sprintf("%d", 0) widened by toUnicode(.., 9)
        int32_t x = bc48 - b9a0 - 0x41, y = b9a4 + bc4c - 0x190;
        int32_t w = (int32_t)d2c_call(v, 0xd35a, 0, (uint32_t)(uintptr_t)wbuf, 0, 0, 0);
        if (w < 0x29) x += (0x29 - w) >> 1;
        d2c_call(v, 0xd37e, 0, (uint32_t)(uintptr_t)wbuf, (uint32_t)x, 0, 0, (uint32_t)y, 0, 0);
    }
    auto rd = [&](uint32_t off) { return *(const uint32_t*)(ds + off); };
    for (int32_t i = 0; i < (int32_t)d2c_call(v, 0xcb38, 0, 0, 0, 0, 0, *(const uint32_t*)(uintptr_t)(unit + 4)); ++i) {
        int32_t id = (int32_t)d2c_call(v, 0xcb3e, 0, 0, 0, 0, 0, *(const uint32_t*)(uintptr_t)(unit + 4), (uint32_t)i);
        if (id < 0 || id >= (int32_t)rd(0xba0)) continue;
        const uint8_t* e = (const uint8_t*)(uintptr_t)rd(0xb98) + (uint32_t)id * 0x23c;
        int32_t k = (int16_t)(e[0x194] | e[0x195] << 8);
        if (k < 0 || k >= (int32_t)rd(0xb94)) continue;
        const uint8_t* r = (const uint8_t*)(uintptr_t)rd(0xb8c) + (uint32_t)k * 0x120;
        if (page != (uint32_t)(int32_t)(int8_t)r[2]) continue;
        a3d38[i] = 0xffffffffu; a3b38[i] = 5;
        uint32_t esi = 0;
        uint32_t c4 = d2c_call(v, 0xc4ea, 0, unit, (uint32_t)id, 0, 0, 0xffffffffu);
        if (c4) {
            esi = d2c_call(v, 0xc718, 0, 0, 0, 0, 0, unit, c4, 1);
            if ((int32_t)esi > 0) { a3d38[i] = 0; a3b38[i] = 0; }
        }
        d2c_call(v, 0x78820, 0, 0, 0, 0, 0, (uint32_t)id, (uint32_t)(uintptr_t)e, esi, a3d38[i], a3b38[i]);
    }
}

// 1.13c D2Client+0x78F20 (149 B, cdecl, no input): the trade/skills panel draw driver. unit = [0xBBFC], cls = byte [0x11991C],
// page = [0xE24F8], tbl = the 0x22-stride dword table at 0xE2500, bfbc/bfc0/bfc8 = [0x11BFBC] [0x11BFC0] [0x11BFC8] (written).
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_78f20(const D2cSvc& v, uint32_t unit, uint8_t cls, uint32_t page, const uint8_t* tbl,
                     uint32_t* bfbc, uint32_t* bfc0, uint32_t* bfc8) {
    d2c_call(v, 0xd3a8, 0, 1, 0, 0, 0);
    *bfc8 = 1;
    d2c_call(v, 0x77a50, 0, 0, 0, 0, 0, *(const uint32_t*)(tbl + (uint32_t)cls * 0x22), page);
    d2c_call(v, 0x78330, 0, 0, 0, 0, 0);
    if ((int32_t)d2c_call(v, 0xc250, 0, 0, 0, 0, 0, unit, 5, 0) > 0) { *bfbc = 1; *bfc0 = 1; d2c_call(v, 0x78c50, 0, 0, 0, 0, 0); }
    else { *bfbc = 0; *bfc0 = 0; d2c_call(v, 0x78aa0, 0, 0, 0, 0, 0); }
    d2c_call(v, 0x78240, 0, 0, 0, 0, 0);
    d2c_call(v, 0x77de0, 0, 0, 0, 0, 0);
    d2c_call(v, 0x77f30, 0, 0, 0, 0, 0, page);
}

// Shared: reserve a 0x14-byte draw-list entry in ctx (count at +0xEA64, entries at +4), max 3000.
static uint32_t dl_alloc(uint32_t ctx) {
    uint32_t n = rd32(ctx + 0xea64);
    wr32(ctx + 0xea64, n + 1);
    return n < 3000 ? ctx + 4 + n * 0x14 : 0;
}
static void dl_append(uint32_t list_slot, uint32_t e) {
    uint32_t c = rd32(list_slot);
    if (!c) { wr32(list_slot, e); return; }
    while (rd32(c + 0x10)) c = rd32(c + 0x10);
    wr32(c + 0x10, e);
}

// 1.13c D2Client+0x79C80 (166 B): kind-1 draw entry for obj at (x + obj[2]*5, y + obj[3]*5)*8; mask bit 3.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_79c80(uint32_t ctx_edi, uint32_t obj_ebx, int32_t x_esi, int32_t y_ebp, uint32_t list_param1,
                     uint32_t arg2, uint32_t mask_c, void (*ord10561)(uint32_t, void*)) {
    uint32_t e = dl_alloc(ctx_edi);
    if (!e) return;
    uint8_t tmp[32];
    wr32(e, 1); wr32(e + 0xc, obj_ebx); wr32(e + 0x10, 0);
    ord10561(arg2, tmp);
    wr32(e + 4, (uint32_t)((x_esi + (int32_t)rd32(obj_ebx + 8) * 5) * 8));
    wr32(e + 8, (uint32_t)((y_ebp + (int32_t)rd32(obj_ebx + 0xc) * 5) * 8));
    wr32(ctx_edi, rd32(ctx_edi) | mask_c); // gdwBitMasks[3]
    dl_append(list_param1 + 0x10, e);
}

// 1.13c D2Client+0x79D30 (231 B) thiscall: as 79C80 (kind 1, mask bit 1) but inserted into list+0x1c
// sorted by (obj[0x14]>>14)&7.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_79d30(uint32_t ctx, uint32_t list_param1, uint32_t arg2, uint32_t obj_eax, int32_t y_ebx,
                     int32_t x_ebp, uint32_t mask_4, void (*ord10561)(uint32_t, void*)) {
    uint32_t e = dl_alloc(ctx);
    if (!e) return;
    uint8_t tmp[32];
    wr32(e, 1); wr32(e + 0xc, obj_eax); wr32(e + 0x10, 0);
    ord10561(arg2, tmp);
    wr32(e + 4, (uint32_t)((x_ebp + (int32_t)rd32(obj_eax + 8) * 5) * 8));
    wr32(e + 8, (uint32_t)((y_ebx + (int32_t)rd32(obj_eax + 0xc) * 5) * 8));
    wr32(ctx, rd32(ctx) | mask_4); // gdwBitMasks[1]
    uint32_t slot = list_param1 + 0x1c, head = rd32(slot);
    if (!head) { wr32(slot, e); return; }
    uint32_t cur = head, prev = 0, nx = rd32(head + 0x10), c4 = cur;
    for (; (c4 = cur, nx != 0); nx = rd32(nx + 0x10)) {
        if (((rd32(rd32(e + 0xc) + 0x14) >> 14) & 7) < ((rd32(rd32(c4 + 0xc) + 0x14) >> 14) & 7)) {
            wr32(e + 0x10, c4);
            if (!prev) wr32(slot, e); else wr32(prev + 0x10, e);
            break;
        }
        cur = nx; prev = c4;
    }
    if (!rd32(c4 + 0x10)) wr32(c4 + 0x10, e);
}

// 1.13c D2Client+0x7A0D0 (162 B): recompute cached grid size from bounds at ctx+4..+0x10; (re)alloc the
// string at +0xEAA8 when size^2 changes. Stores +0xEAB4, +0xEAB0, +0xEAB8.
extern "C" EMSCRIPTEN_KEEPALIVE
int32_t d2_client_7a0d0(uint32_t ctx_esi, int32_t ebp_in, uint32_t (*ord10042)(int, int), void (*ord10043)(int, int)) {
    int32_t a = ((int32_t)rd32(ctx_esi + 0xc) - (int32_t)rd32(ctx_esi + 4)) / 0xa0 + 0xb;
    int32_t d = (int32_t)rd32(ctx_esi + 0x10) - (int32_t)rd32(ctx_esi + 8);
    int32_t r3 = (int32_t)((uint32_t)d * 0x66666667u), q = d / 0x50, b = q + 0xb + a;
    if (rd32(ctx_esi + 0xeaa8)) {
        q += 0xc;
        if (b * b - (int32_t)rd32(ctx_esi + 0xeaac) == 0) goto done;
        ord10043(0x3a, 0);
    }
    r3 = (int32_t)ord10042(0x3d, 0);
    wr32(ctx_esi + 0xeaa8, (uint32_t)r3);
    wr32(ctx_esi + 0xeaac, (uint32_t)(b * b));
    q = ebp_in;
done:
    wr32(ctx_esi + 0xeab4, (uint32_t)a);
    wr32(ctx_esi + 0xeab0, (uint32_t)b);
    wr32(ctx_esi + 0xeab8, (uint32_t)q);
    return r3;
}

// Shared: fill entry position from unit obj (types 2,4,5 read +0xb->0xc/0x10; else ord10867/10750).
static void dl_unit_pos(uint32_t e, uint32_t obj, int32_t (*ord10867)(uint32_t), int32_t (*ord10750)(uint32_t)) {
    int32_t t = (int32_t)rd32(obj), s = (int32_t)rd32(obj + 0x2c), v;
    bool direct = t == 2 || (t > 3 && t < 6);
    v = direct ? (int32_t)rd32(s + 0xc) : s ? ord10867(s) : 0;
    wr32(e + 4, (uint32_t)v * 8);
    v = direct ? (int32_t)rd32(s + 0x10) : s ? ord10750(s) : 0;
    wr32(e + 8, (uint32_t)v << 3);
}

static void dl_unit(uint32_t ctx, uint32_t obj, uint32_t list_param1, uint32_t kind, uint32_t mask,
                    int32_t (*o67)(uint32_t), int32_t (*o50)(uint32_t), uint32_t slot_off, bool use_mask) {
    uint32_t e = dl_alloc(ctx);
    if (!e) return;
    wr32(e, kind); wr32(e + 0xc, obj); wr32(e + 0x10, 0);
    dl_unit_pos(e, obj, o67, o50);
    if (use_mask) wr32(ctx, rd32(ctx) | mask);
    dl_append(list_param1 + slot_off, e);
}

// 1.13c D2Client+0x7A180 (211 B): kind-2 entry for unit obj, mask bit 3, list+0x10.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_7a180(uint32_t obj_eax, uint32_t ctx_ebx, uint32_t list_param1, uint32_t mask_c,
                     int32_t (*ord10867)(uint32_t), int32_t (*ord10750)(uint32_t)) {
    dl_unit(ctx_ebx, obj_eax, list_param1, 2, mask_c, ord10867, ord10750, 0x10, true);
}

// 1.13c D2Client+0x7A260 (207 B): as 7A180 with kind 0.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_7a260(uint32_t obj_eax, uint32_t ctx_ebx, uint32_t list_param1, uint32_t mask_c,
                     int32_t (*ord10867)(uint32_t), int32_t (*ord10750)(uint32_t)) {
    dl_unit(ctx_ebx, obj_eax, list_param1, 0, mask_c, ord10867, ord10750, 0x10, true);
}

// 1.13c D2Client+0x7A500 (193 B) fastcall(ecx=ctx): kind-0 entry, no mask update, list at EBX+0x18.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_7a500(uint32_t ctx_ecx, uint32_t obj_eax, uint32_t list_ebx,
                     int32_t (*ord10867)(uint32_t), int32_t (*ord10750)(uint32_t)) {
    dl_unit(ctx_ecx, obj_eax, list_ebx, 0, 0, ord10867, ord10750, 0x18, false);
}
