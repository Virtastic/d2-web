// src/core/d2client/d2client_batch_77da0.cpp -- 12 UNMAPPED 1.13c D2Client.dll rows (alpha lane),
// from build-wasm/ghidra-d2client/decomp/0x<RVA>.c. Base 0x6FAB0000. Globals are parameters; ordinals and
// unrecovered callees are host callbacks after the retail arguments.
// GUESSED ARGUMENTS (decompile shows unaff_/in_ registers): 77F30 (iStack_8 = class, uninitialised),
// 79C80 (EDI=ctx, EBX=obj, ESI=x, EBP=y), 79D30 (EAX=obj, EBX=y, EBP=x; ECX=ctx), 7A0D0 (ESI=ctx, EBP),
// 7A180/7A260 (EAX=obj, EBX=ctx), 7A500 (ECX=ctx, EAX=obj, EBX=list). Void ordinal calls whose arguments are
// in hidden registers (10003, 10028) take none. 7A0D0's return is the leftover mul-high operand, kept as int32.
#include <cstdint>
#include <emscripten.h>

static inline uint32_t rd32(uint32_t a) { return *(const uint32_t*)(uintptr_t)a; }
static inline void wr32(uint32_t a, uint32_t v) { *(uint32_t*)(uintptr_t)a = v; }
static inline int16_t rd16(uint32_t a) { return *(const int16_t*)(uintptr_t)a; }

// 1.13c D2Client+0x77DA0 (56 B): clear every state==1 slot of the table at 0x6FBC3D38.
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

// 1.13c D2Client+0x78330 (1174 B): three fixed text draws, then the per-class offset list through 0x781D0.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_78330(uint8_t cls_c991c, int32_t y0_b9a4, int32_t y1_bc4c, void (*ord10003)(void),
                     void (*ord10028)(void), void (*ord10150)(int, int, int), void (*sub_781d0)(int32_t)) {
    static const int16_t T0[] = {0x96, 0xa2, 0xae, 0x102, 0x10e, 0x11a, 0x167, 0x173, 0x17f, 0};
    static const int16_t T1[] = {0x9c, 0xa8, 0x108, 0x114, 0x172, 0x17e, 0};
    static const int16_t T2[] = {0x9c, 0xa8, 0x102, 0x10e, 0x11a, 0x178, 0};
    static const int16_t T3[] = {0x9c, 0xa8, 0x108, 0x114, 0x172, 0x17e, 0};
    static const int16_t T4[] = {0xa2, 0x108, 0x114, 0x172, 0x17e, 0};
    static const int16_t T5[] = {0xa2, 0x108, 0x114, 0x172, 0};
    static const int16_t T6[] = {0x96, 0xa2, 0x108, 0x114, 0x172, 0};
    static const int16_t* const T[7] = {T0, T1, T2, T3, T4, T5, T6};
    static const int16_t pre[3] = {0x1c7, 0x1bb, 0x1af};
    for (int i = 0; i < 3; i++) {
        ord10003();
        int32_t a = y0_b9a4 - pre[i] + y1_bc4c;
        ord10028(); ord10150(a, 0, 0);
    }
    if (cls_c991c > 6) return;
    for (const int16_t* p = T[cls_c991c]; *p; p++) { ord10003(); sub_781d0(*p); }
}

// 1.13c D2Client+0x78AA0 (432 B): draw "0" when 0xBFC0==0; then for each slot whose datatable chain matches
// 0x6FB924F8 mark 3D38=-1,3B38=5, try ord10306, clear on success, and call 0x78820.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_78aa0(uint32_t unit_bbfc, int32_t flag_bfc0, int32_t y0_b9a4, int32_t y1_bc4c,
                     uint32_t dt_sgpt, int32_t* tab_3d38, int32_t* tab_3b38, uint8_t want_24f8,
                     int (*ord11120)(uint32_t), int (*ord10044)(uint32_t, int), int (*ord10708)(int),
                     int (*ord10306)(uint32_t, int, int), void (*to_unicode_zero)(void),
                     void (*ord10028)(void), void (*ord10150)(int, int, int), void (*sub_78820)(void)) {
    if (!unit_bbfc) return;
    if (flag_bfc0 == 0) {
        to_unicode_zero(); // wsprintfA("%d",0) + Unicode::toUnicode(buf,.,9)
        ord10028(); ord10150(y0_b9a4 - 400 + y1_bc4c, 0, 0);
    }
    for (int i = 0; i < ord11120(rd32(unit_bbfc + 4)); i++) {
        int32_t id = ord10044(rd32(unit_bbfc + 4), i);
        if (id < 0 || id >= (int32_t)rd32(dt_sgpt + 0xba0)) continue;
        uint32_t e = rd32(dt_sgpt + 0xb98) + (uint32_t)id * 0x23c;
        if (!e) continue;
        int32_t k = rd16(e + 0x194);
        if (k < 0 || k >= (int32_t)rd32(dt_sgpt + 0xb94)) continue;
        uint32_t r = (uint32_t)k * 0x120 + rd32(dt_sgpt + 0xb8c);
        if (!r || want_24f8 != *(const uint8_t*)(uintptr_t)(r + 2)) continue;
        tab_3d38[i] = -1; tab_3b38[i] = 5;
        int v = ord10708(-1);
        if (v && ord10306(unit_bbfc, v, 1) > 0) { tab_3d38[i] = 0; tab_3b38[i] = 0; }
        sub_78820();
    }
}

// 1.13c D2Client+0x78F20 (149 B): panel draw driver.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_78f20(uint32_t unit_bbfc, uint8_t cls_c991c, const uint32_t* tab_2500_stride34,
                     uint8_t* f_bfc8, uint32_t* f_bfbc, uint32_t* f_bfc0,
                     void (*ord10184)(void), void (*sub_77a50)(uint32_t), void (*sub_78330)(void),
                     int (*ord10587)(uint32_t, int, int), void (*sub_78aa0)(void), void (*sub_78c50)(void),
                     void (*sub_78240)(void), void (*sub_77de0)(void), void (*sub_77f30)(void)) {
    ord10184();
    *f_bfc8 = 1; // DAT_6fbcbfc8 (byte-width unrecovered; word stored)
    sub_77a50(*(const uint32_t*)((const uint8_t*)tab_2500_stride34 + (uint32_t)cls_c991c * 0x22));
    sub_78330();
    if (ord10587(unit_bbfc, 5, 0) < 1) { *f_bfbc = 0; *f_bfc0 = 0; sub_78aa0(); }
    else { *f_bfbc = 1; *f_bfc0 = 1; sub_78c50(); }
    sub_78240(); sub_77de0(); sub_77f30();
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
