// src/core/d2client/d2client_batch_75030.cpp -- 12 UNMAPPED 1.13c D2Client.dll rows (alpha lane, party-panel
// tooltips/labels), from build-wasm/ghidra-d2client/decomp/0x<RVA>.c. Base 0x6FAB0000. Globals are
// parameters; ordinals, Unicode::* and unrecovered callees are host callbacks after the retail arguments.
// GUESSED ARGUMENTS (decompile shows hidden registers / no-arg calls): 75030 (EAX=x), 758C0 (EAX=x),
// 75690 (EAX=mode, ECX=this=x), 760F0 (ECX=unit id), 754A0 (param_3=y), the Ordinal_10003/10028/10150/10041
// argument lists (decompiler shows none; text/width/draw take the previous call's result in registers).
// Rows stay NAMED-grade: no spec, no vector.
#include <cstdint>
#include <emscripten.h>

static inline uint32_t rd32(uint32_t a) { return *(const uint32_t*)(uintptr_t)a; }
static inline int16_t rd16(uint32_t a) { return *(const int16_t*)(uintptr_t)a; }
typedef uint16_t wch;
struct Uni {  // Unicode:: helpers, host-supplied
    void (*strcpy)(wch*, const wch*); void (*strcat)(wch*, const wch*);
    void (*strncpy)(wch*, const wch*, int); int (*strlen)(const wch*);
    void (*toUnicode)(wch*, const char*, int); void (*win2Unicode)(wch*, const char*, int);
};

// 1.13c D2Client+0x75030 (124 B): x in EAX (guess). Draws two Ordinal_10041 button cells at panel+0xc / +0x10b.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_75030(int32_t x_eax, int32_t panel_x_bca0, int32_t panel_y_b9a4, uint32_t col_bfec,
                     void (*ord10041)(int32_t*, int32_t, int32_t, int32_t, int32_t, int32_t)) {
    int32_t a[18] = {0};
    a[13] = 0;  // local_48[0]=0
    int32_t s[4] = {1, 0, 0, 0};
    (void)col_bfec;
    ord10041(a, panel_x_bca0 + 0xc, 0, 0, 0, 0);
    ord10041(s, panel_x_bca0 + 0x10b, x_eax - panel_y_b9a4, -1, 5, 0);
}

// 1.13c D2Client+0x750B0 (245 B): four Ordinal_10041 calls (kinds 0..3) at x=panel / panel+0x100, two rows.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_750b0(int32_t panel_x_b9a0, int32_t panel_y_b9a4, int32_t off_bc4c, uint32_t col_bfdc,
                     void (*ord10041)(int32_t*, int32_t, int32_t, int32_t, int32_t, int32_t)) {
    (void)col_bfdc;
    int32_t y0 = panel_y_b9a4 - 0xe0 + off_bc4c, y1 = panel_y_b9a4 - 0x30 + off_bc4c;
    int32_t k0[18] = {0}, k1 = 1, k2 = 2, k3 = 3;
    ord10041(k0, panel_x_b9a0, y0, -1, 5, 0);
    ord10041(&k1, panel_x_b9a0 + 0x100, y0, -1, 5, 0);
    ord10041(&k2, panel_x_b9a0, y1, -1, 5, 0);
    ord10041(&k3, panel_x_b9a0 + 0x100, y1, -1, 5, 0);
}

// 1.13c D2Client+0x75220 (328 B): tooltip backdrop sized to the wider of two strings (ids param_1, param_2).
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_75220(uint32_t id1, int16_t id2, int32_t param_3, int32_t two_line, int32_t panel_x_b9a0,
                     int32_t panel_y_b9a4, uint32_t col_bff8, const Uni* U,
                     const wch* (*ord10003)(uint32_t), int (*ord10028)(const wch*),
                     void (*ord10014)(int, int, int, int, uint32_t, int)) {
    wch b1[200] = {0}, b2[200] = {0};
    U->strcpy(b1, ord10003(id1));
    int w1 = ord10028(b1), w2 = 0;
    if (id2) { U->strcpy(b2, ord10003((uint32_t)(uint16_t)id2)); w2 = ord10028(b2); }
    int top = 0x23, h = 0x1e;
    if (!two_line) { top = 0x2a; h = 15; }
    int w = w2 < w1 ? w1 : w2;
    int x = (panel_x_b9a0 - w / 2) - 5 + param_3;
    ord10014(x, top - panel_y_b9a4, x + 10 + w, top - panel_y_b9a4 + h, col_bff8, 2);
}

// 1.13c D2Client+0x75370 (91 B): centred text x = param_2 - width(string param_1)/2.
extern "C" EMSCRIPTEN_KEEPALIVE
int32_t d2_client_75370(uint32_t id, int32_t param_2, const Uni* U, const wch* (*ord10003)(uint32_t),
                        int (*ord10028)(const wch*)) {
    wch b[200] = {0};
    U->strcpy(b, ord10003(id));
    return param_2 - ord10028(b) / 2;
}

// 1.13c D2Client+0x753D0 (196 B): from the player list (head list_bc14, next at +0x80, id u16 at +0x22) fill
// ids_3f38[8] with the party id when >=2 members share it else 0xffff; then call 75030 for each slot whose
// successor differs and is not 0xffff.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_753d0(uint32_t list_bc14, uint32_t* ids_3f38 /*8*/, void (*sub_75030)(void)) {
    for (int i = 0; i < 8; i++) ids_3f38[i] = 0xffff;
    uint32_t cur = list_bc14;
    uint32_t* p = ids_3f38;
    while (list_bc14 && cur) {
        uint32_t v = (uint16_t)rd16(cur + 0x22);
        int n = 0;
        for (uint32_t q = list_bc14; q; q = rd32(q + 0x80)) if (rd16(q + 0x22) == rd16(cur + 0x22)) n++;
        if (n < 2) v = 0xffff;
        *p++ = v;  // ponytail: original has no bound; 8 slots by the 3f38..3f54 range
        cur = rd32(cur + 0x80);
        if (p >= ids_3f38 + 8) break;
    }
    for (uint32_t* q = ids_3f38; q < ids_3f38 + 7; q++)  // loop stops at address 0x6fbc3f54 (slot 7 is last compared as q[1])
        if (q[1] != q[0] && q[0] != 0xffff) sub_75030();
}

// 1.13c D2Client+0x754A0 (295 B): copies string 0xBE240(), truncates to width 0x8c with the "..." tail
// (bytes at 84ca0), draws via Ordinal_10150(x-panel_y+0xc, y, 0). param_3 = y (guess).
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_754a0(int32_t param_2, int32_t param_3, int32_t panel_y_b9a4, const char* dots_84ca0, const Uni* U,
                     const wch* (*sub_be240)(void), int (*ord10028)(const wch*),
                     void (*ord10150)(int, int, int)) {
    wch b[255] = {0}, d[5] = {0};
    U->strcpy(b, sub_be240());
    if (ord10028(b) > 0x8c) {
        U->toUnicode(d, dots_84ca0, 5);
        int n = U->strlen(b);
        int dw = ord10028(d);
        while (0x8b - dw < ord10028(b) && n > 0) b[--n] = 0;  // ponytail: bound added; retail trusts width shrink
        U->strcat(b, d);
    }
    ord10150((param_2 - panel_y_b9a4) + 0xc, param_3, 0);
}

// 1.13c D2Client+0x755D0 (192 B): party-count badge for the local player's party, drawn if >=2 and id != -1.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_755d0(uint32_t list_bc14, uint32_t* list_bc14_now, int32_t panel_y_b9a4, uint32_t col_bfe0,
                     const wch* (*ord10003)(uint32_t), int (*ord10028)(const wch*), void (*ord10150)(int, int, int),
                     void (*ord10184)(void), void (*sub_74fd0)(int, int, uint32_t, int, int)) {
    ord10184();
    int n = 0;
    uint32_t head = *list_bc14_now;
    if (list_bc14 && head) {
        int16_t id = rd16(list_bc14 + 0x22);
        for (uint32_t q = head; q; q = rd32(q + 0x80)) if (rd16(q + 0x22) == id) n++;
        if (n > 1 && id != -1) {
            sub_74fd0(n, 0x23, col_bfe0, 0xe3, 0);
            ord10003(0);  // string id unrecovered (decompile shows none)
            int y = 0x21 - panel_y_b9a4;
            ord10028(0);
            ord10150(y, 2, 0);
        }
    }
    ord10184();
}

// 1.13c D2Client+0x75690 (216 B): mode in EAX, x in ECX(this) (guesses). Mode 1 colours by param_1/param_2.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_75690(uint32_t mode_eax, int32_t this_x, int32_t param_1, int16_t param_2, int32_t panel_y_b9a4,
                     const wch* (*ord10003)(uint32_t), int (*ord10028)(const wch*),
                     void (*ord10150)(int, int, int), void (*ord10184)(void)) {
    int32_t col = 0;
    switch (mode_eax) {
    case 0: case 2: case 3: case 4: break;
    case 1:
        if (param_2 == -1) return;
        col = param_1 == 0 ? 8 : 2;
        break;
    default: ord10184(); return;
    }
    ord10003(0);
    int32_t off = -2 - panel_y_b9a4;
    ord10028(0);
    ord10150(this_x + off, col, 0);
}

// 1.13c D2Client+0x75780 (300 B): ASCII name -> Unicode, truncated with "..." to width 0x88 (param_4==0) or 0x60.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_75780(const char* name, int32_t param_2, int32_t param_3, int32_t param_4, int32_t panel_y_b9a4,
                     const Uni* U, int (*ord10028)(const wch*), void (*ord10150)(int, int, int)) {
    wch b[196] = {0}, d[5] = {0};
    U->toUnicode(b, name, 100);
    int lim = param_4 ? 0x60 : 0x88;
    if (ord10028(b) >= lim) {
        int n = U->strlen(b);
        while (ord10028(b) >= lim && n > 0) b[--n] = 0;  // ponytail: bound added
        U->toUnicode(d, "...", 5);
        U->strcat(b, d);
    }
    ord10150((param_2 - panel_y_b9a4) - 2, param_3, 0);
}

// 1.13c D2Client+0x758C0 (236 B): x in EAX (guess). Builds three strings + "%2u" of param_1, draws at y=param_2.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_758c0(int32_t x_eax, uint32_t param_1, int32_t param_2, int32_t panel_y_b9a4, const Uni* U,
                     const wch* (*ord10003)(uint32_t), int (*wsprintfA)(char*, const char*, ...),
                     void (*ord10150)(int, int, int)) {
    wch b[200] = {0}, d[10] = {0};
    char a[100];
    U->strncpy(b, ord10003(0), 0x14);
    U->strcat(b, ord10003(0));
    U->strcat(b, ord10003(0));
    wsprintfA(a, "%2u", param_1);
    U->win2Unicode(d, a, 9);
    U->strcat(b, d);
    ord10150(x_eax - panel_y_b9a4, param_2, 0);
}

// 1.13c D2Client+0x75D30 (790 B): hover tooltips for the party panel: the local party badge (box
// panel_x+0xe3..+0x118, y 0x18..0x23 minus panel_y) and each list entry button (+4/+8/+0x14/+0x18, next at +0xcd).
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_75d30(uint32_t btn_bfd8, uint32_t pl_bc14, int32_t mx_b828, int32_t my_b824, int32_t px_b9a0,
                     int32_t py_b9a4, uint32_t* col_bff8,
                     uint32_t (*ord10190)(int, int, int), void (*ord10003)(int, int, int), void (*ord10150)(void),
                     void (*sub_75220)(uint32_t, int, int, int), int32_t (*sub_75370)(uint32_t, int)) {
    *col_bff8 = ord10190(0, 0, 0);
    int n = 0;
    uint32_t nxt = pl_bc14;
    if (pl_bc14) {
        for (uint32_t q = pl_bc14; q; q = rd32(q + 0x80)) if (rd16(q + 0x22) == rd16(pl_bc14 + 0x22)) n++;
        if (n > 1 && rd16(pl_bc14 + 0x22) != -1 && px_b9a0 + 0xe3 <= mx_b828 && mx_b828 <= px_b9a0 + 0x118 &&
            0x18 - py_b9a4 <= my_b824 && my_b824 <= 0x23 - py_b9a4) {
            sub_75220(0x1018, 0, 0xd5, 0);
            sub_75370(0x1018, 0xd5);
            ord10003(0x37 - py_b9a4, 0, 0);
            ord10150();
        }
        nxt = rd32(pl_bc14 + 0x80);
    }
    for (uint32_t b = btn_bfd8; b; b = rd32(b + 0xcd)) {
        int32_t x = (int32_t)rd32(b + 4) + px_b9a0, y = (int32_t)rd32(b + 8) - py_b9a4;
        if (x <= mx_b828 && mx_b828 <= x + 0x35 && y - 0x14 <= my_b824 && my_b824 <= y && rd32(b + 0x14) == 0) {
            uint32_t k = rd32(b + 0x18), s;
            if (k == 0) s = 0x1004; else if (k == 2) s = 0x1006; else if (k == 4) s = 0x1008; else continue;
            sub_75220(s, s + 1, 0xda, 1);
            sub_75370(s, 0xda);
            ord10003(0x32 - py_b9a4, 0, 0);
            ord10150();
            sub_75370(s + 1, 0xda);
            ord10003(0x41 - py_b9a4, 0, 0);
            ord10150();
        }
        nxt = nxt ? rd32(nxt + 0x80) : 0;  // retail advances a second list cursor in lockstep; unused afterwards
    }
}

// 1.13c D2Client+0x760F0 (207 B): ECX = unit id (guess). If the local unit exists and 0x9D850 returns a name,
// draw it (width-limited by unicodenwidth: >=11 wide -> Ordinal_10184 first) at y=0x1f-panel_y.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_760f0(uint32_t unit_ecx, uint32_t local_bbfc, int32_t panel_y_b9a4, const Uni* U,
                     const char* (*sub_9d850)(uint32_t), uint32_t (*unicodenwidth)(const char*, int),
                     const wch* (*ord10042)(int, int), void (*ord10043)(int, int), int (*ord10028)(const wch*),
                     void (*ord10150)(int, int, int), void (*ord10184)(void)) {
    if (!local_bbfc) return;
    const char* s = sub_9d850(unit_ecx);
    if (!s) return;
    int len = 0;
    while (s[len]) len++;
    uint32_t w = unicodenwidth(s, len);
    if (w + 1 > 0xb) ord10184();
    U->toUnicode((wch*)ord10042(0x211, 0), s, (int)w + 1);
    int y = 0x1f - panel_y_b9a4;
    ord10028(0);
    ord10150(y, 0, 0);
    ord10184();
    ord10043(0x21a, 0);
}
