// src/core/d2client/d2client_batch_7b730.cpp -- twelve UNMAPPED 1.13c D2Client.dll rows (alpha lane),
// from build-wasm/ghidra-d2client/decomp/0x<RVA>.c. Base 0x6FAB0000; RVAs are retail's.
// Globals are parameters (caller reads the live value); unrecovered callees (ordinals into D2Win/D2Lang/
// D2Gfx, FUN_ helpers) are host callbacks taking the retail arguments. Node addresses are 32-bit guest values.
#include <cstdint>
#include <emscripten.h>

static inline uint32_t& W(uint32_t a, uint32_t off) { return *(uint32_t*)(uintptr_t)(a + off); }

// 1.13c D2Client+0x7B730 (641 B): draw the queued message list (nodes: 6 string ptrs at +0, tick at +0x1E,
// next at +0x22, color at +0x1A), then expire nodes whose tick < now.
struct Msg7B730 {
    void (*lock)(void);                                   // Ordinal_10184
    uint32_t (*wrap)(uint32_t str, uint32_t len);         // Ordinal_10059(300) -> {text,next} list
    uint32_t (*width)(uint32_t str, uint32_t len);        // Ordinal_10171
    void (*rect)(int32_t, int32_t, int32_t, int32_t, int32_t, int32_t);  // Ordinal_10014
    void (*text)(int32_t y, uint32_t color, int32_t);     // Ordinal_10150
    void (*free_)(uint32_t id, uint32_t);                 // Ordinal_10043
    uint32_t (*ulen)(uint32_t str);                       // Unicode::strlen
    uint32_t (*now)(void);                                // GetTickCount
};
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_7b730(const Msg7B730* c, uint32_t* head_c3aa4, int32_t* count_c3aa8, int32_t flag_aadcc,
                     int32_t mode_cc414, int32_t width_8bc48) {
    if (*count_c3aa8 == 0) return;
    c->lock();
    int32_t y0 = flag_aadcc != 0 ? 0x5f : 0x14;
    int32_t x = 0xf, lines = 0;
    bool wrap = false;
    if (mode_cc414 == 1) wrap = true;
    else if (mode_cc414 == 2) { x = width_8bc48 / 2 + 0xf; wrap = true; }
    for (uint32_t n = *head_c3aa4; n != 0; n = W(n, 0x22)) {
        for (int i = 0; i < 6; i++) {
            uint32_t s = W(n, i * 4);
            if (!s) continue;
            if (wrap) {
                uint32_t l = c->wrap(s, 300);
                int32_t y = lines * 0xf - 0xe + y0;
                while (l) {
                    uint32_t next = W(l, 4);
                    uint32_t w = c->width(W(l, 0), c->ulen(W(l, 0)));
                    if (w) {
                        c->rect(x - 5, y, x + 5 + (int32_t)w, y + 0x10, 0, 1);
                        c->text(y + 0xe, W(n, 0x1a), 0);
                        lines++; y += 0xf;
                    }
                    c->free_(0xa43, 0); c->free_(0xa44, 0);
                    l = next;
                }
            } else {
                uint32_t w = c->width(s, c->ulen(s));
                if (w) {
                    int32_t y = lines * 0xf + y0;
                    c->rect(x - 4, y - 0xe, x + 4 + (int32_t)w, y + 2, 0, 1);
                    c->text(y, W(n, 0x1a), 0);
                    lines++;
                }
            }
        }
    }
    uint32_t t = c->now(), prev = 0, n = *head_c3aa4;
    while (n) {
        if (W(n, 0x1e) < t) {
            (*count_c3aa8)--;
            uint32_t nx = W(n, 0x22);
            if (*head_c3aa4 == n) *head_c3aa4 = nx;
            for (int i = 0; i < 6; i++) if (W(n, i * 4)) c->free_(0x6af, 0);
            c->free_(0x6b2, 0);
            n = nx;
            if (prev) W(prev, 0x22) = nx;
        } else { prev = n; n = W(n, 0x22); }
    }
    c->lock();
}

// 1.13c D2Client+0x7B9C0 (118 B): push a fresh node (0x7B120) on the list at [0x11C3AB0]; at >= 0x80
// entries drop the tail first (0x7B330) and unlink it.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_7b9c0(uint32_t* head_c3ab0, uint16_t* count_c3ab4,   // count: the WORD at 0x113AB4 (cmp/dec/inc word ptr)
                     uint32_t (*alloc_7b120)(void), void (*free_7b330)(void)) {
    uint32_t node = alloc_7b120();
    if (*count_c3ab4 >= 0x80 && *head_c3ab0 != 0) {
        uint32_t prev = 0, cur = *head_c3ab0;
        for (uint32_t nx = W(cur, 0x22); nx; nx = W(nx, 0x22)) { prev = cur; cur = nx; }
        free_7b330();
        if (prev) W(prev, 0x22) = 0;
        if (*count_c3ab4) (*count_c3ab4)--;
    }
    W(node, 0x22) = *head_c3ab0;
    (*count_c3ab4)++;
    *head_c3ab0 = node;
}

// 1.13c D2Client+0x7BAD0 (104 B): node in ECX, point {x,y} in ESI. State 0 -> 1, state 3 -> return 1;
// mode 1 skips when x > w/2-10, mode 2 when x < w/2+10, mode 3 always; else place via 0x7B240. Returns 1.
extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t d2_client_7bad0(uint32_t node, const int32_t* pt, int32_t mode_cc414, int32_t width_8bc48,
                         void (*place_7b240)(uint32_t node, int32_t x, int32_t y)) {
    if (W(node, 0x16) == 0) W(node, 0x16) = 1;
    else if (W(node, 0x16) == 3) return 1;
    if (mode_cc414 == 1) { if (width_8bc48 / 2 - 10 < pt[0]) return 1; }
    else if (mode_cc414 == 2) { if (pt[0] < width_8bc48 / 2 + 10) return 1; }
    else if (mode_cc414 == 3) return 1;
    place_7b240(node, pt[0], pt[1]);
    return 1;
}

// retired, see d2client_7beb0.cpp: D2Client 0x7BEB0 (133 B): list {head; u16 count} -- free every node (6 string frees + node free),
// head = 0; the freed count must equal the u16 count (else fatal 0x6c9); then count = 0.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_7beb0(uint32_t* list, void (*free_)(uint32_t id, uint32_t), void (*fatal)(uint32_t code)) {
    int16_t n = 0;
    uint32_t node = list[0];
    if (node) {
        do {
            uint32_t nx = W(node, 0x22);
            for (int i = 0; i < 6; i++) if (W(node, i * 4)) free_(0x6af, 0);
            free_(0x6b2, 0);
            n++;
            node = nx;
        } while (node);
        list[0] = 0;
    }
    if (n == (int16_t)list[1]) { *(uint16_t*)&list[1] = 0; return; }
    fatal(0x6c9);   // Ordinal_10265 + Ordinal_10024 + _exit(-1)
}

// retired, see d2client_7c380.cpp: D2Client 0x7C380.

// 1.13c D2Client+0x7C580 (57 B): if Ordinal_10612() != 0 { 0x7B3D0(); free(0x68a); Ordinal_10357(); }
// then Ordinal_10422(0).
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_7c580(int32_t (*o10612)(void), void (*sub_7b3d0)(void), void (*free_)(uint32_t, uint32_t),
                     void (*o10357)(void), void (*o10422)(uint32_t)) {
    if (o10612() != 0) { sub_7b3d0(); free_(0x68a, 0); o10357(); }
    o10422(0);
}

// 1.13c D2Client+0x7C5C0 (135 B): current unit lookup. [0x11C3ACA] != 0 -> chain at table 0x11BA808[id&0x7f]
// with id [0x11C3AC2], required type 1; else if [0x11C3A80] != 0 -> table 0x11BAA08[id&0x7f] with id
// [0x11C3AC6], required type 2. Chain next at +0xE4, id at +0xC, type at +0. Type mismatch -> fatal 0x5a.
extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t d2_client_7c5c0(uint32_t flag_c3aca, uint32_t flag_c3a80, uint32_t id_c3ac6, uint32_t id_c3ac2,
                         const uint32_t* tab_bba808, const uint32_t* tab_bbaa08, void (*fatal)(uint32_t code)) {
    uint32_t n, id, type;
    if (flag_c3aca == 0) {
        if (flag_c3a80 == 0) return 0;
        n = tab_bbaa08[id_c3ac6 & 0x7f]; id = id_c3ac6; type = 2;
        if (!n) return 0;
        while (W(n, 0xc) != id) { n = W(n, 0xe4); if (!n) return 0; }
    } else {
        id = id_c3ac2; type = 1;
        for (n = tab_bba808[id & 0x7f]; n && W(n, 0xc) != id; n = W(n, 0xe4)) {}
        if (!n) return 0;
    }
    if (W(n, 0) == type) return n;
    fatal(0x5a);
    return 0;
}

// 1.13c D2Client+0x7C650 (50 B): if [0x11C3A60] != 0 { 0x7B3D0(); free(0x68a); [0x11C3A60] = 0; }
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_7c650(uint32_t* p_c3a60, void (*sub_7b3d0)(void), void (*free_)(uint32_t, uint32_t)) {
    if (*p_c3a60) { sub_7b3d0(); free_(0x68a, 0); *p_c3a60 = 0; }
}

// 1.13c D2Client+0x7C730 (51 B): destroy window [0x11C3A70] (0xA3D20) and delete object [0x11C3A74]
// (virtual slot 0, arg 1).
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_7c730(uint32_t* win_c3a70, uint32_t* obj_c3a74, void (*sub_a3d20)(void),
                     void (*vdtor)(uint32_t obj, uint32_t flag)) {
    if (*win_c3a70) { sub_a3d20(); *win_c3a70 = 0; }
    if (*obj_c3a74) { vdtor(*obj_c3a74, 1); *obj_c3a74 = 0; }
}

// retired, see d2client_7c770.cpp: D2Client 0x7C770.

// retired, see d2client_7c8f0.cpp: D2Client 0x7C8F0.

// 1.13c D2Client+0x7CA40 (306 B, __fastcall(ECX=param_1, EDX=param_2), EAX = point {x,y}): if
// Ordinal_11067(param_1) < [0x11C3ACE] { Ordinal_10422(0,param_1); if param_2: [param_2+0xA4]=0 }. Build a
// box node, centre the point on it, probe the placement slot 0x7AF20 while [0x11C3AE4] < 0x10, place via
// 0x7BAD0, free. The RECT passed to 0x7AF20 is an uninitialised stack local in retail (zeroed here).
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_7ca40(uint32_t param_1, uint32_t param_2, int32_t* pt, uint32_t* count_c3ae4, uint32_t lim_c3ace,
                     uint32_t (*o11067)(uint32_t), void (*o10422)(uint32_t, uint32_t),
                     void (*lock)(void), uint32_t (*o10042)(uint32_t, uint32_t), uint32_t (*o10059)(uint32_t),
                     void (*sub_7bf60)(void), uint8_t (*sub_7af20)(int32_t* rect, uint32_t idx),
                     void (*sub_7bad0)(uint32_t node, int32_t* pt), void (*sub_7b3d0)(void),
                     void (*free_)(uint32_t, uint32_t)) {
    if (o11067(param_1) < lim_c3ace) {
        o10422(0, param_1);
        if (param_2) W(param_2, 0xa4) = 0;
    }
    lock();
    uint32_t node = o10042(0x659, 0);
    W(node, 0) = o10059(0xb4);
    sub_7bf60();
    uint16_t w = *(uint16_t*)(uintptr_t)(node + 0x10);
    int32_t y = pt[1];
    pt[1] = y - 0x32;
    pt[0] -= (uint32_t)(w >> 1);
    pt[1] = y - 0x32 + (int32_t)W(node, 4) * -0xf - 3;
    if (*count_c3ae4 < 0x10) {
        int32_t rect[4] = {0, 0, 0, 0};
        if (sub_7af20(rect, *count_c3ae4) == 0) { sub_7b3d0(); free_(0x68a, 0); return; }
    }
    (*count_c3ae4)++;
    sub_7bad0(node, pt);
    sub_7b3d0();
    free_(0x68a, 0);
    lock();
}
