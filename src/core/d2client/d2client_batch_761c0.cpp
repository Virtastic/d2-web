// src/core/d2client/d2client_batch_761c0.cpp -- 12 UNMAPPED 1.13c D2Client.dll rows (alpha lane, party
// panel at 0x761C0-0x77D30), from build-wasm/ghidra-d2client/decomp/0x<RVA>.c. Base 0x6FAB0000.
// Globals are parameters; unrecovered callees and ordinals are host callbacks after the retail arguments.
// GUESSED arguments (decompile shows unaff_/in_/extraout_ register values or dropped ordinal args):
//   761C0 edx_guess (extraout_EDX compare), 4D630 ecx;  763C0 player=EBX, Ordinal_10003 args (none shown,
//   passed 0), 4DD20/..: none;  769E0 in_EAX=unit, unaff_EBX=x_base, unaff_ESI=entry;  76B20 unaff_ESI/EDI
//   (starts 0 / entry), Ordinal_10003/10150 args dropped by decompiler;  77170 unaff_EBP=list tail;
//   77C50/77CA0 in_AL=index. Ordinal_10041 = draw cel (rec,x,y,-1,5,0); 10014 = rect; 10150 = text.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

static inline uint32_t rd32(uint32_t a) { uint32_t v; memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
static inline void wr32(uint32_t a, uint32_t v) { memcpy((void*)(uintptr_t)a, &v, 4); }
static inline uint16_t rd16(uint32_t a) { uint16_t v; memcpy(&v, (const void*)(uintptr_t)a, 2); return v; }

// 1.13c D2Client+0x761C0 (490 B) NAMED: draws the 5 button cels of one party entry (+ red box on the
// invite button when not allowed). entry = party panel record (stride 32 B from +8).
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_761c0(uint32_t entry, uint32_t local_unit_11bbfc, int32_t org_x_9a0, int32_t org_y_9a4,
                     uint32_t cfg_980, uint32_t player_list_c14, uint8_t* col_bff8, uint32_t edx_guess,
                     void (*ord10041)(uint32_t*, int32_t, int32_t, int32_t, int32_t, int32_t),
                     uint32_t (*ord10331)(uint32_t), int (*ord10057)(uint32_t),
                     uint32_t (*ord10190)(int, int, int),
                     void (*ord10014)(int32_t, int32_t, int32_t, int32_t, uint32_t, int),
                     uint32_t (*sub_4d630)(uint32_t, int32_t)) {
    uint32_t rec[18] = {0};
    uint32_t p = entry + 8;
    for (int n = 5; n; --n, p += 32) {
        int32_t kind = (int32_t)rd32(p + 12);
        if (kind != 0 || (int32_t)rd32(p + 16) != 1) {
            rec[13] = rd32(p + 4);
            rec[0] = rd32(p + 8);
            bool draw = true;
            switch (kind) {
            case 1: if (rec[0] < 4 || rec[0] > 7) rec[0] = 4; break;
            case 2:
                if (rec[0] < 0xc || rec[0] > 0xf) rec[0] = 0xc;
                if (!(*(const uint8_t*)(uintptr_t)(cfg_980 + 0x1eb) & 4)) draw = false;
                break;
            case 3: if (rec[0] > 3) rec[0] = 0; break;
            case 4: if (rec[0] < 8 || rec[0] > 0xb) rec[0] = 8; break;
            }
            if (draw) ord10041(rec, (int32_t)rd32(p - 4) + org_x_9a0, (int32_t)rd32(p) - org_y_9a4, -1, 5, 0);
        }
        if (kind == 1) {
            bool bad = false;
            uint32_t u = ord10331(local_unit_11bbfc);
            if (u == 0 || ord10057(u) == 0) bad = true;
            if ((int32_t)rd32(entry + 0xbd) < 9) {
                int32_t id = local_unit_11bbfc ? (int32_t)rd32(local_unit_11bbfc + 0xc) : -1;
                int32_t other = (int32_t)rd32(entry + 0xc5);
                uint32_t r = sub_4d630(0, id);
                if (r && rd32(r + 0x34) && edx_guess != (uint32_t)other) {
                    for (uint32_t q = rd32(rd32(r + 0x34)); q; q = rd32(q + 8))
                        if ((int32_t)rd32(q) == other) { if (rd32(q + 4) & 8) goto skip; break; }
                }
                bad = true;
            }
        skip:
            if (!player_list_c14 || rd16(player_list_c14 + 0x20) < 9 || bad) {
                uint32_t c = ord10190(0xff, 0, 0);
                *col_bff8 = (uint8_t)c;
                ord10014((int32_t)rd32(p - 4) + org_x_9a0, (int32_t)rd32(p) - org_y_9a4 - 0x13,
                         (int32_t)rd32(p - 4) + org_x_9a0 + 0x14, (int32_t)rd32(p) - org_y_9a4 + 1, c, 1);
            }
        }
    }
}

// 1.13c D2Client+0x763C0 (961 B) NAMED: hover tooltip box for a party entry. player = entry's unit/player
// record in EBX (guess): name at +0x66, id +0x10, relation +0x1c. Strings via Unicode callbacks.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_763c0(uint32_t player, uint32_t local_unit_11bbfc, int32_t org_x_9a0, int32_t org_y_9a4,
                     uint32_t cfg_980, uint8_t col_bff8,
                     uint32_t (*sub_4fd90)(uint32_t, uint32_t),
                     char16_t* (*ord10003)(char16_t*), void (*uni_toUnicode)(char16_t*, const char*, int),
                     void (*uni_strcpy)(char16_t*, const char16_t*),
                     void (*uni_sprintf)(int, char16_t*, const char16_t*, const char16_t*),
                     int (*ord10028)(void),
                     void (*ord10014)(int32_t, int32_t, int32_t, int32_t, uint32_t, int),
                     void (*ord10150)(int32_t, int32_t, int32_t)) {
    char16_t a[256] = {0}, b[256] = {0}, name[256] = {0};
    int32_t h = 0x10;
    uni_toUnicode(name, (const char*)(uintptr_t)(player + 0x66), 0x80);
    uint32_t u = local_unit_11bbfc;
    int32_t id = u ? (int32_t)rd32(u + 0xc) : 0, pid = (int32_t)rd32(player + 0x10);
    int self8 = u && sub_4fd90(id, 8), self1 = u && sub_4fd90(id, 1);
    int oth8 = u && sub_4fd90(pid, 8), oth1 = u && sub_4fd90(pid, 1);
    int32_t rel = (int32_t)rd32(player + 0x1c);
    if (self8 == oth8) uni_strcpy(a, ord10003(0)); else uni_sprintf(0x80, a, ord10003(name), name);
    if (*(const uint8_t*)(uintptr_t)(cfg_980 + 0x1eb) & 4) {
        (void)rel; h = 0x1e;
        if (self1 == oth1) uni_strcpy(b, ord10003(0)); else uni_sprintf(0x80, b, ord10003(name), name);
    }
    int32_t w1 = ord10028(), w2 = ord10028();
    int32_t w = (w1 + 10 <= w2 + 10) ? w2 + 10 : w1 + 10;
    int32_t x = org_x_9a0 + 0x24;
    ord10014(x, 0x23 - org_y_9a4, x + w, h + 0x23 - org_y_9a4, col_bff8, 2);
    ord10150(0x32 - org_y_9a4, 2, 0);
    ord10150(0x41 - org_y_9a4, x, 0);
}

// 1.13c D2Client+0x76790 (582 B): draws every party row (max 8) after the local player's.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_76790(uint32_t list_fd8, uint32_t player_list_c14, uint32_t local_unit_11bbfc,
                     uint32_t (*sub_4d630)(uint32_t, int32_t), void (*ord10184)(void),
                     void (*sub_5780)(uint32_t, int32_t, uint32_t, uint32_t), void (*sub_4ed0)(uint32_t, uint32_t),
                     void (*sub_58c0)(uint32_t, uint32_t), void (*sub_54a0)(uint32_t, int32_t, uint32_t),
                     void (*sub_761c0)(uint32_t), void (*sub_5690)(int32_t, uint32_t, int16_t)) {
    uint32_t row = list_fd8;
    int32_t y = 0x5a;
    auto count = [&](int16_t pty) { int n = 0; for (uint32_t p = player_list_c14; p; p = rd32(p + 0x80)) if ((int16_t)rd16(p + 0x22) == pty) n++; return n; };
    int16_t mine = -1;
    if (player_list_c14) { int16_t t = (int16_t)rd16(player_list_c14 + 0x22); if (count(t) > 1) mine = t; }
    uint32_t pl = player_list_c14 ? rd32(player_list_c14 + 0x80) : 0;
    for (int i = 0; row && pl && i <= 7; i++) {
        uint32_t mode = 0; bool same = false;
        int16_t pt = (int16_t)rd16(pl + 0x22), sv;
        int n = count(pt);
        sv = (player_list_c14 && n >= 2) ? pt : (int16_t)-1;
        if (player_list_c14 && n > 1 && pt != -1) {
            if (mine == sv) { mode = 2; same = true; } else mode = 8;
        }
        if (local_unit_11bbfc) {
            int32_t me = (int32_t)rd32(local_unit_11bbfc + 0xc);
            uint32_t r = sub_4d630(0, (int32_t)rd32(pl + 0x10));
            if (r && rd32(r + 0x34))
                for (uint32_t q = rd32(rd32(r + 0x34)); q; q = rd32(q + 8))
                    if ((int32_t)rd32(q) == me) { if (rd32(q + 4) & 8) mode = 1; break; }
        }
        ord10184();
        sub_5780(row + 0xa0, y, mode, sv != -1);
        sub_4ed0(rd32(row + 0xb9), mode);
        sub_58c0(rd32(row + 0xbd), mode);
        if (same && rd32(row + 0xc1)) sub_54a0(row, y, mode);
        sub_761c0(row);
        sub_5690(y, mine == sv, sv);
        row = rd32(row + 0xcd); y += 0x26; pl = rd32(pl + 0x80);
        ord10184();
    }
}

// 1.13c D2Client+0x769E0 (320 B) NAMED: lays out one party entry (5 button records, stride 32 B at +0),
// entry in ESI, x_base EBX, unit in EAX (all guesses). Stores row y base at +0xc9, unit id at +0xc5.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_769e0(uint32_t entry, int32_t x_base, uint32_t unit, uint32_t g_bfe0, uint32_t g_bfe4,
                     int (*sub_4dd20)(void), uint32_t (*sub_4de20)(void), uint32_t (*sub_4ddd0)(void),
                     uint32_t (*sub_4dd80)(void)) {
    wr32(entry + 0xc9, x_base + 0xd);
    uint32_t* e = (uint32_t*)(uintptr_t)entry;
    e[6] = unit ? rd32(unit + 0x30) : 1;
    e[0] = 1; e[1] = 0xbe; e[2] = x_base; e[3] = g_bfe0; e[4] = 0; e[5] = 0;
    e[0xc] = (sub_4dd20() ? 2 : 0) + 4;
    e[8] = 0; e[9] = 0xe; e[10] = x_base + 7; e[11] = g_bfe4; e[13] = 1; e[14] = 0;
    e[0x14] = (sub_4de20() ? 2 : 0) + 0xc;
    e[0x10] = 0; e[0x11] = 0xf5; e[0x12] = x_base; e[0x13] = g_bfe4; e[0x15] = 2; e[0x16] = 0;
    e[0x1c] = sub_4ddd0() ? 2 : 0;
    e[0x18] = 0; e[0x19] = 0x10a; e[0x1a] = x_base; e[0x1b] = g_bfe4; e[0x1d] = 3; e[0x1e] = 0;
    uint32_t v = sub_4dd80();
    e[0x20] = 0; e[0x21] = 0x11f; e[0x22] = x_base; e[0x23] = g_bfe4;
    e[0x24] = (v ? 2 : 0) + 8; e[0x25] = 4; e[0x26] = 0;
    wr32(entry + 0xc5, unit ? rd32(unit + 0x10) : 0xffffffffu);
}

// 1.13c D2Client+0x76B20 (1602 B) NAMED: party panel hover/tooltip dispatch. Per row: inside the row name
// band -> 763C0; else over one of the 5 buttons (kind at +0x34+32*i) -> tooltip pair (string ids 0x1019-0x101b,
// 0x100a-0x1017) by relation. ord10003/10150 args dropped by the decompiler (text call with y,flags).
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_76b20(uint32_t list_fd8, uint32_t local_unit_11bbfc, uint32_t player_list_c14, uint32_t cfg_980,
                     int32_t org_x_9a0, int32_t org_y_9a4, int32_t mouse_y_824, int32_t mouse_x_828, uint8_t* col_bff8,
                     uint32_t (*ord10331)(void), uint32_t (*ord10190)(int, int, int), int (*ord10057)(uint32_t),
                     void (*ord10003)(int32_t, int, int), void (*ord10150)(void),
                     void (*sub_5220)(int, int, int, int), void (*sub_5370)(int, int),
                     int (*sub_4dd20)(void), uint32_t (*sub_4ddd0)(void), uint32_t (*sub_4de20)(void),
                     uint32_t (*sub_4dd80)(void), void (*sub_763c0)(void)) {
    uint32_t row = list_fd8, plr = local_unit_11bbfc;
    int has_unit = 0;
    uint32_t u = ord10331();
    *col_bff8 = (uint8_t)ord10190(0, 0, 0);
    if (u && ord10057(u)) has_unit = 1;
    if (!row) return;
    auto pair = [&](int a, int b, int off, int ykey, int flag) {
        sub_5220(a, b, off, flag); sub_5370(a, off);
        ord10003(0x32 - org_y_9a4, 0, 0); ord10150();
        sub_5370(b, off); ord10003(ykey - org_y_9a4, 0, 0); ord10150();
    };
    auto single = [&](int a, int off, int yk, int flag2) {
        sub_5220(a, 0, off, 0); sub_5370(a, off); ord10003(yk - org_y_9a4, flag2, 0); ord10150();
    };
    do {
        int32_t ry = (int32_t)rd32(row + 0xc9) - org_y_9a4;
        if (mouse_x_828 < org_x_9a0 + 0x24 || org_x_9a0 + 0x8c < mouse_x_828 || mouse_y_824 < ry - 0x1a || ry < mouse_y_824) {
            for (int i = 1; i < 5; i++) {
                uint32_t b = row + 0x24 + 32 * (i - 1) + 0x10;   // piVar5 = row+0x34 stepping 32 B
                b = row + 0x34 + 32 * (i - 1);
                int32_t bx = (int32_t)rd32(b - 16), by = (int32_t)rd32(b - 12), kind = (int32_t)rd32(b);
                if (org_x_9a0 + bx <= mouse_x_828 && mouse_x_828 <= org_x_9a0 + 0x14 + bx &&
                    by - org_y_9a4 - 0x14 <= mouse_y_824 && mouse_y_824 <= by - org_y_9a4) {
                    if (kind == 1) {
                        if (!player_list_c14 || rd16(player_list_c14 + 0x20) < 9 || (int32_t)rd32(row + 0xbd) < 9) {
                            pair(0x101a, 0x101b, 0xa0, 0x41, 1);
                        } else if (!has_unit) {
                            single(0x1019, 0x6e, 0x37, 1);
                        } else if (!sub_4dd20()) pair(0x100a, 0x100b, 0x6e, 0x41, 1);
                        else pair(0x100c, 0x100d, 0x6e, 0x41, 1);
                        return;
                    }
                    if (kind == 2) {
                        if (*(const uint8_t*)(uintptr_t)(cfg_980 + 0x1eb) & 4) {
                            if (!sub_4de20()) pair(0x100e, 0x100f, 0xff, 0x41, 1);
                            else pair(0x1010, 0x1011, 0xff, 0x41, 1);
                            return;
                        }
                    } else if (kind == 3) {
                        if (sub_4ddd0()) pair(0x1012, 0x1013, 0x114, 0x41, 1);
                        else pair(0x1014, 0x1015, 0x114, 0x41, 1);
                        return;
                    } else if (kind == 4) {
                        if (!sub_4dd80()) single(0x1016, 0x128, 0x37, 0);
                        else single(0x1017, 0x128, 0x37, 0);
                        return;
                    }
                }
            }
        } else {
            sub_763c0();
        }
        row = rd32(row + 0xcd);
        if (plr) plr = rd32(plr + 0x80);
    } while (row);
}

// 1.13c D2Client+0x77170 (214 B) NAMED: append a new party entry (0x14d-byte string-table alloc zeroed)
// for player; init via 769E0, copy name (+0x66), relation +0x1c, level +0x20, +0x24. tail = EBP (guess).
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_77170(uint32_t player, uint32_t tail_ebp, uint32_t* list_fd8,
                     uint32_t (*ord10042)(int, int), void (*sub_769e0)(uint32_t),
                     uint32_t (*ord10265)(int), void (*ord10024)(uint32_t, uint32_t), void (*exit_fn)(int)) {
    uint32_t e = ord10042(0x14d, 0);
    memset((void*)(uintptr_t)e, 0, 0xd1);
    sub_769e0(e);
    if (!player) { ord10024(0, ord10265(0x171)); exit_fn(-1); return; }
    const char* s = (const char*)(uintptr_t)(player + 0x66);
    char* d = (char*)(uintptr_t)(e + 0xa0);
    do *d++ = *s; while (*s++);
    wr32(e + 0xb9, rd32(player + 0x1c));
    wr32(e + 0xbd, rd16(player + 0x20));
    wr32(e + 0xc1, rd32(player + 0x24));
    if (tail_ebp) {
        while (rd32(tail_ebp + 0xcd)) tail_ebp = rd32(tail_ebp + 0xcd);
        wr32(tail_ebp + 0xcd, e);
        return;
    }
    *list_fd8 = e;
}

// 1.13c D2Client+0x77250 (144 B): party panel draw+hover, in order; then the close-button hover (rect
// x 0x116..0x136, y 0x185..0x1a5) -> Ordinal_10003/10085.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_77250(int32_t org_x_9a0, int32_t org_y_9a4, int32_t mouse_y_824, int32_t mouse_x_828,
                     void (*sub_50b0)(void), void (*sub_60f0)(void), void (*sub_6790)(void), void (*sub_55d0)(void),
                     void (*sub_6050)(void), void (*sub_5d30)(void), void (*sub_6b20)(void), void (*sub_53d0)(void),
                     void (*ord10003)(int32_t, int, int), void (*ord10085)(void)) {
    sub_50b0(); sub_60f0(); sub_6790(); sub_55d0(); sub_6050(); sub_5d30(); sub_6b20(); sub_53d0();
    if (org_x_9a0 + 0x116 <= mouse_x_828 && mouse_x_828 <= org_x_9a0 + 0x136 &&
        0x185 - org_y_9a4 <= mouse_y_824 && mouse_y_824 <= 0x1a5 - org_y_9a4) {
        ord10003(0x182 - org_y_9a4, 0, 1); ord10085();
    }
}

// 1.13c D2Client+0x772E0 (78 B): rebuild party list: 74F90 clears, then 77170 per player after the first
// (loop exactly as retail: ends when the node is 0 and counter <= 8).
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_772e0(uint32_t player_list_c14, void (*sub_4f90)(void), void (*sub_7170)(uint32_t)) {
    sub_4f90();
    uint32_t p = player_list_c14 ? rd32(player_list_c14 + 0x80) : 0;
    int n = 0;
    while (p != 0 || n > 8) {
        sub_7170(p);
        if (p == 0) { n++; p = player_list_c14; }
        else { n++; p = rd32(p + 0x80); }
    }
}

// 1.13c D2Client+0x77A50 (505 B): draws the 8 corner cels (frames 0-3, then 4*5+0..3 = 20-23) of the panel
// frame at 4 positions {x0,x1} x {y0,y1}; rec[13]=param_1 (palette/mode). Frame ids 0..3 then (5*4)+0..3.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_77a50(uint32_t param_1, int32_t org_x_9a0, int32_t org_y_9a4, int32_t base_x_bc4c, int32_t base_y_bc48,
                     void (*ord10041)(uint32_t*, int32_t, int32_t, int32_t, int32_t, int32_t)) {
    uint32_t rec[18] = {0};
    rec[13] = param_1;
    int32_t xa = org_y_9a4 - 0xe0 + base_x_bc4c, xb = org_y_9a4 - 0x30 + base_x_bc4c;
    int32_t ya = base_y_bc48 - org_x_9a0 - 0x140, yb = base_y_bc48 - org_x_9a0 - 0x40;
    // note: retail pairs DAT_6fbcb9a4 with the cel X and (bc48 - 9a0) with Y; names kept as the retail globals
    int32_t px[4] = {xa, xa, xb, xb}, py[4] = {ya, yb, ya, yb};
    for (int g = 0; g < 2; g++)
        for (int k = 0; k < 4; k++) {
            rec[0] = g ? 20 + k : k;
            ord10041(rec, py[k], px[k], -1, 5, 0);
        }
}

// 1.13c D2Client+0x77C50 (66 B) NAMED: in_AL=index (guess). Returns row y offset for index 1..3.
extern "C" EMSCRIPTEN_KEEPALIVE
int32_t d2_client_77c50(uint8_t idx, int32_t base_bc48, int32_t org_9a0) {
    int32_t b = base_bc48 - org_9a0;
    return idx == 1 ? b - 0x131 : idx == 2 ? b - 0xec : idx == 3 ? b - 0xa7 : 0;
}

// 1.13c D2Client+0x77CA0 (140 B) NAMED: in_AL=index (guess). Returns column x offset for index 1..6.
extern "C" EMSCRIPTEN_KEEPALIVE
int32_t d2_client_77ca0(uint8_t idx, int32_t org_9a4, int32_t base_bc4c) {
    static const int32_t d[7] = {0, -0x1a2, -0x15e, -0x11a, -0xd6, -0x91, -0x4d};
    return (idx >= 1 && idx <= 6) ? org_9a4 + d[idx] + base_bc4c : 0;
}

// 1.13c D2Client+0x77D30 (69 B): free the cached cel slot [0x6fbc991c * 0x22 dwords] in the 6FB92500 table.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_77d30(uint32_t* slot_table_92500, uint8_t idx_c991c,
                     void (*ord10065)(uint32_t), void (*ord10043)(int, int)) {
    uint32_t* s = (uint32_t*)((uint8_t*)slot_table_92500 + (uint32_t)idx_c991c * 0x22);
    uint32_t v = *s;
    if (v) { ord10065(v); ord10043(0x7a, 0); *s = 0; }
}
