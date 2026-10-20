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
void d2_client_7b9c0(uint32_t* head_c3ab0, int32_t* count_c3ab4,
                     uint32_t (*alloc_7b120)(void), void (*free_7b330)(void)) {
    uint32_t node = alloc_7b120();
    if ((uint32_t)*count_c3ab4 > 0x7f && *head_c3ab0 != 0) {
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

// 1.13c D2Client+0x7BEB0 (133 B): list {head; u16 count} -- free every node (6 string frees + node free),
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

// 1.13c D2Client+0x7C380 (437 B): draw the 4 scroll-panel cells (frames 0..3, scroin) and the buy/sell
// button (frame 10, buysellbtn); loaders (0x6FADB420) fill the two cached cell pointers on first use.
struct Draw7C380 {
    void (*rect)(int32_t, int32_t, int32_t, int32_t, int32_t, int32_t);   // Ordinal_10014
    void (*cel)(uint32_t cellfile, int32_t frame, int32_t x, int32_t y, int32_t color, int32_t mode, int32_t);  // Ordinal_10041
    uint32_t (*load)(const char* path);                                   // wsprintfA + FUN_6fadb420(0)
};
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_7c380(const Draw7C380* c, uint32_t* scroin_cbfb0, uint32_t* buysell_cb9c4,
                     int32_t x_cb9a0, int32_t y_cb9a4, int32_t width_8bc48, int32_t height_8bc4c) {
    c->rect(x_cb9a0, y_cb9a4 - 0xe0 + height_8bc4c, width_8bc48 / 2, height_8bc4c - 0x30, 0, 5);
    if (!*scroin_cbfb0) *scroin_cbfb0 = c->load("DATA\\GLOBAL\\UI\\MENU\\scroin");
    int32_t ya = y_cb9a4 - 0x6d + height_8bc4c, yb = y_cb9a4 - 0x31 + height_8bc4c;
    c->cel(*scroin_cbfb0, 0, x_cb9a0, ya, -1, 5, 0);
    c->cel(*scroin_cbfb0, 1, x_cb9a0 + 0x100, ya, -1, 5, 0);
    c->cel(*scroin_cbfb0, 2, x_cb9a0, yb, -1, 5, 0);
    c->cel(*scroin_cbfb0, 3, x_cb9a0 + 0x100, yb, -1, 5, 0);
    if (!*buysell_cb9c4) *buysell_cb9c4 = c->load("DATA\\GLOBAL\\ui\\panel\\buysellbtn");
    c->cel(*buysell_cb9c4, 10, x_cb9a0 + 0x115, y_cb9a4 - 0x3d + height_8bc4c, -1, 5, 0);
}

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

// 1.13c D2Client+0x7C770 (382 B): build the two-label window (SEH frame omitted). win[0x17]=1 after create,
// win[0x14] must be < 0x80000000 (else fatal 0x4f8), then win[0x11] = -1.
struct Win7C770 {
    uint32_t (*create)(int32_t, int32_t, uint32_t cb, int32_t, int32_t, int32_t, int32_t, int32_t w, int32_t h); // 0xA3E40
    uint32_t (*str)(void);                                                          // Ordinal_10003
    void (*label)(uint32_t win, uint32_t s, int32_t y, int32_t, int32_t, uint32_t cb, int32_t);   // 0xA36F0 area
    uint32_t (*alloc)(void);                                                        // Ordinal_401(0xc,..)
    uint32_t (*ctor)(uint32_t obj, int32_t, int32_t, int32_t w, int32_t h, int32_t); // 0x67F20
    void (*sub_a3400)(void); void (*sub_7aea0)(void);
    void (*fatal)(uint32_t code);
};
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_7c770(const Win7C770* c, uint32_t* win_c3a70, uint32_t* obj_c3a74, int32_t w_8bc48, int32_t h_8bc4c) {
    const uint32_t cb = 0x6fb2ac90;
    if (*win_c3a70) return;
    *win_c3a70 = c->create(5, 0x1e, cb, 0, 0, 0, 0, w_8bc48 - 0x14, h_8bc4c - 0x82);
    if (!*win_c3a70) { c->fatal(0x529); return; }
    W(*win_c3a70, 0x5c) = 1;
    c->label(*win_c3a70, c->str(), 0x15, 0, 1, 0, 0);
    c->label(*win_c3a70, c->str(), h_8bc4c - 0xa5, 0, 1, cb, 1);
    uint32_t o = c->alloc();
    *obj_c3a74 = o ? c->ctor(o, 5, 0x41, w_8bc48 - 0x14, h_8bc4c - 200, 2) : 0;
    if (!*obj_c3a74) return;
    c->sub_a3400();
    c->sub_7aea0();
    if (!*win_c3a70) { c->fatal(0x4f2); return; }
    if (W(*win_c3a70, 0x50) < 0x80000000u) { W(*win_c3a70, 0x44) = 0xffffffffu; return; }
    c->fatal(0x4f8);
}

// 1.13c D2Client+0x7C8F0 (244 B): replace the message box [0x11C3A60]: free the old one, take string
// Ordinal_10003, node = Ordinal_10042(0x659) with buffer Ordinal_10059(0xb4) + 0x7BF60, then on success
// node[0x16]=1, [0x11C3A64]=tick+(len+0x19)*200, [0x11C3A88]=[0x11C3A84]=1,
// [0x11C3A94]=100-(u16@+0x12>>2), [0x11C3A90]=0x140-(u16@+0x10>>1).
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_7c8f0(uint32_t* p_c3a60, uint32_t* t_c3a64, uint32_t* f_c3a88, uint32_t* f_c3a84,
                     uint32_t* y_c3a94, uint32_t* x_c3a90,
                     void (*sub_7b3d0)(void), void (*free_)(uint32_t, uint32_t), uint32_t (*o10003)(void),
                     void (*lock)(void), uint32_t (*o10042)(uint32_t, uint32_t), uint32_t (*o10059)(uint32_t),
                     void (*sub_7bf60)(void), uint32_t (*ulen)(uint32_t), uint32_t (*now)(void)) {
    if (*p_c3a60) { sub_7b3d0(); free_(0x68a, 0); *p_c3a60 = 0; }
    uint32_t s = o10003();
    lock();
    uint32_t node = o10042(0x659, 0);
    W(node, 0) = o10059(0xb4);
    sub_7bf60();
    *p_c3a60 = node;
    lock();
    if (*p_c3a60) {
        W(*p_c3a60, 0x16) = 1;
        uint32_t n = ulen(s);
        *t_c3a64 = (n + 0x19) * 200 + now();
        *f_c3a88 = 1; *f_c3a84 = 1;
        *y_c3a94 = 100 - (uint32_t)(*(uint16_t*)(uintptr_t)(*p_c3a60 + 0x12) >> 2);
        *x_c3a90 = 0x140 - (uint32_t)(*(uint16_t*)(uintptr_t)(*p_c3a60 + 0x10) >> 1);
    }
}

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
