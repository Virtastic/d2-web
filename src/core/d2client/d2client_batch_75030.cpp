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

// D2Client 750B0 moved to d2client_750b0.cpp (one frame block for all four calls, cel word = g11BFDC).

// 1.13c D2Client+0x75220 (328 B), stdcall(a0, a1, a2, a3): a0/a1 = string ids (a1 looked up only when its low word is nonzero), a2 is the x offset (read after a push, so [esp+0x1B0] is a2 there),
// a3 is the two-line flag (zero: top 0x2a height 0xf; nonzero: top 0x23 height 0x1e). Retail looks the ids up with
// D2Lang 10003 (ECX = the full dword), copies each into a stack buffer and measures it (D2Win 10028); the rectangle (D2Gfx 10014) is
// (x, top - gB9A4, x + w + 10, top - gB9A4 + h, byte gBFF8, 2) with w = max(w1, w2) signed and x = gB9A0 - w/2 (truncating) + a2 - 5.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_75220(uint32_t id1, uint32_t id2, int32_t a2, int32_t a3, int32_t panel_x_b9a0,
                     int32_t panel_y_b9a4, uint32_t col_bff8, const Uni* U,
                     const wch* (*ord10003)(uint32_t), int (*ord10028)(const wch*),
                     void (*ord10014)(int32_t, int32_t, int32_t, int32_t, uint32_t, int)) {
    wch b1[200] = {0}, b2[200] = {0};
    U->strcpy(b1, ord10003(id1));
    int32_t w1 = ord10028(b1), w2 = 0;
    if ((uint16_t)id2) { U->strcpy(b2, ord10003(id2)); w2 = ord10028(b2); }
    int32_t top = 0x23, h = 0x1e;
    if (!a3) { top = 0x2a; h = 15; }
    int32_t w = w1 > w2 ? w1 : w2;
    uint32_t x = (uint32_t)panel_x_b9a0 - (uint32_t)(w / 2) + (uint32_t)a2 - 5u;
    uint32_t t = (uint32_t)top - (uint32_t)panel_y_b9a4;
    ord10014((int32_t)x, (int32_t)t, (int32_t)(x + (uint32_t)w + 10u), (int32_t)(t + (uint32_t)h), col_bff8 & 0xffu, 2);
}

// 1.13c D2Client+0x75370 (91 B): centred text x = param_2 - width(string param_1)/2.
extern "C" EMSCRIPTEN_KEEPALIVE
int32_t d2_client_75370(uint32_t id, int32_t param_2, const Uni* U, const wch* (*ord10003)(uint32_t),
                        int (*ord10028)(const wch*)) {
    wch b[200] = {0};
    U->strcpy(b, ord10003(id));
    return param_2 - ord10028(b) / 2;
}

// 1.13c D2Client+0x753D0 (196 B), cdecl(): head = gBC14 (read once, next at +0x80, id word at +0x22). ids = the dword array at +0x113F38: the first eight are
// set to 0xFFFF, then one dword per node is stored with NO bound (a list of more than eight nodes writes past the array): the node's id when more than
// one list node shares it, else 0xFFFF. Then for q = 0..6, ids[q+1] != ids[q] && ids[q] != 0xFFFF calls 0x75030 with EAX = 0x43 + 0x26*q.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_753d0(uint32_t head, uint32_t* ids, void (*sub_75030)(uint32_t eax)) {
    for (int i = 0; i < 8; i++) ids[i] = 0xffff;
    if (head) {
        uint32_t cur = head, *p = ids;
        do {
            uint16_t id = (uint16_t)rd16(cur + 0x22);
            int n = 0;
            for (uint32_t q = head; q; q = rd32(q + 0x80)) if ((uint16_t)rd16(q + 0x22) == id) n++;
            *p++ = n > 1 ? (uint32_t)id : 0xffffu;
            cur = rd32(cur + 0x80);
        } while (cur);
    }
    for (int q = 0; q < 7; q++)
        if (ids[q + 1] != ids[q] && ids[q] != 0xffff) sub_75030(0x43u + 0x26u * (uint32_t)q);
}

// 1.13c D2Client+0x754A0 (295 B), stdcall(unit, a1, a2): be240(ESI = dword at unit+0xC1) returns the text; it is copied into a 255-char buffer and measured
// (D2Win 10028). Wider than 0x8C: last = b[strlen-1]; d = toUnicode(dots5, 5) with d[3] = last; esi = 0x8B - width(d); if width(b) > esi, zero b from index 253 down
// (one char per pass, unbounded in retail; stops at the buffer start here) until width(b) <= esi; then strcat(b, d). Finally D2Win 10150 draws b at
// (gB9A0 + 0x7E, a1 - gB9A4 + 0xC, a2, 0). dots5 = the five bytes at D2Client+0xD4CA0 ("...." and a NUL).
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_754a0(uint32_t unit, int32_t a1, int32_t a2, int32_t px_b9a0, int32_t py_b9a4, const char* dots5, const Uni* U,
                     const wch* (*sub_be240)(uint32_t esi), int (*ord10028)(const wch*),
                     void (*ord10150)(const wch* ecx, int32_t x, int32_t s0, int32_t s1, int32_t s2)) {
    wch b[255] = {0}, d[5] = {0};
    U->strcpy(b, sub_be240(rd32(unit + 0xc1)));
    if (ord10028(b) > 0x8c) {
        int n = U->strlen(b);
        wch last = n > 0 ? b[n - 1] : 0;
        U->toUnicode(d, dots5, 5);
        d[3] = last;
        int32_t lim = 0x8b - ord10028(d);
        if (ord10028(b) > lim) {
            wch* e = b + 254;
            do { if (e > b) *--e = 0; } while (ord10028(b) > lim);
        }
        U->strcat(b, d);
    }
    ord10150(b, px_b9a0 + 0x7e, a1 - py_b9a4 + 0xc, a2, 0);
}

// 1.13c D2Client+0x755D0 (192 B), cdecl(): head = gBC14. D2Win 10184 is called with ECX = 0 first and ECX = 1 last. When head != 0, n = nodes (next at +0x80, id word
// at +0x22) sharing the head's id is > 1 and the id != 0xFFFF: 0x74FD0 (ECX = n, EDX = 0x23, stack gBFE0, 0xE3, 0); p = D2Lang 10003 (0x1038); w = D2Win 10028 (p);
// x = gB9A0 + 0xE3, plus (0x36 - w) >> 1 (arithmetic) when w < 0x36; D2Win 10150 (ECX = p, EDX = x, stack 0x21 - gB9A4, 2, 0).
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_755d0(uint32_t head, uint32_t col_bfe0, int32_t px_b9a0, int32_t py_b9a4,
                     void (*ord10184)(uint32_t ecx), void (*sub_74fd0)(uint32_t ecx, uint32_t edx, uint32_t s0, uint32_t s1, uint32_t s2),
                     const wch* (*ord10003)(uint32_t ecx), int32_t (*ord10028)(const wch*),
                     void (*ord10150)(const wch* ecx, uint32_t edx, uint32_t s0, uint32_t s1, uint32_t s2)) {
    ord10184(0);
    int32_t n = 0;
    uint16_t id = 0;
    if (head) {
        id = (uint16_t)rd16(head + 0x22);
        for (uint32_t q = head; q; q = rd32(q + 0x80)) if ((uint16_t)rd16(q + 0x22) == id) n++;
    }
    if (head && n > 1 && id != 0xffff) {
        sub_74fd0((uint32_t)n, 0x23, col_bfe0, 0xe3, 0);
        const wch* p = ord10003(0x1038);
        uint32_t x = (uint32_t)px_b9a0 + 0xe3u;
        int32_t w = ord10028(p);
        if (w < 0x36) x += (uint32_t)((int32_t)(0x36u - (uint32_t)w) >> 1);
        ord10150(p, x, 0x21u - (uint32_t)py_b9a4, 2, 0);
    }
    ord10184(1);
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
