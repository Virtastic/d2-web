// src/core/d2client/d2client_batch_68c30.cpp -- 12 UNMAPPED 1.13c D2Client.dll rows (alpha lane), from
// build-wasm/ghidra-d2client/decomp/0x<RVA>.c. Base 0x6FAB0000. Globals are parameters/pointers; ordinals and
// unrecovered callees are host callbacks after the retail arguments. GUESSED hidden-register args: 8AC40
// (EAX=flag, EDI=index), 8AD30 (EAX=count), 8B600/8B650 (EAX=x, EDX=y), 8B900 (EAX=id), 8B0C0 (ESI=obj).
// Rows stay NAMED-grade: no spec, no vector.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

typedef void (*Ord10041)(int32_t*, int32_t, int32_t, int32_t, int32_t, int32_t);

// 1.13c D2Client+0x68C30 (73 B)
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_68c30(uint32_t* tbl_c4cd8_0x203dw, uint32_t* tick_c044, uint32_t has_c4cdc,
                     void (*sub68810)(uint32_t*), void (*ord10043)(int32_t, int32_t)) {
    sub68810(tbl_c4cd8_0x203dw);
    *tick_c044 -= tbl_c4cd8_0x203dw[2];  // DAT_6fbc4ce0
    if (has_c4cdc) ord10043(0xe50, 0);
    std::memset(tbl_c4cd8_0x203dw, 0, 0x203 * 4);
}

static inline uint32_t step(uint32_t* s) {
    uint64_t v = (uint64_t)s[0] * 0x6ac690c5u + s[1];
    s[0] = (uint32_t)v; s[1] = (uint32_t)(v >> 32);
    return (uint32_t)v;
}
// 1.13c D2Client+0x8AC40 (230 B): seed = {lo,hi}; seed = lo*0x6ac690c5 + hi each draw.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_8ac40(int32_t flag_eax, int32_t idx_edi, uint32_t* seed, uint32_t base_a9e14, uint32_t range_a9e18,
                     uint32_t* out_beaf0, uint32_t* out_bb5a0, int32_t* out_beef0, const uint32_t* tab_beae0_8,
                     uint32_t (*sub1330)(uint32_t*), uint32_t* out_bb440) {
    uint32_t a = flag_eax == 0 ? sub1330(seed) : ((step(seed) & 7) - 1) + base_a9e14;
    out_beaf0[idx_edi] = a;
    uint32_t b;
    if ((int32_t)range_a9e18 < 1) b = 0;
    else if ((range_a9e18 & (range_a9e18 - 1)) == 0) b = (range_a9e18 - 1) & step(seed);
    else b = step(seed) % range_a9e18;
    out_bb5a0[idx_edi] = b;
    out_beef0[idx_edi] = -1 - (int32_t)(step(seed) % 5);
    out_bb440[idx_edi] = tab_beae0_8[step(seed) & 7];
}

// 1.13c D2Client+0x8AD30 (195 B): list nodes are {obj*, next*}; obj+8 = handle, obj+0x1c = level.
struct Node { uint32_t* obj; Node* next; };
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_8ad30(int32_t n_eax, int32_t* h_beae8, Node* head_cbf30,
                     int32_t (*sub88a70)(int, int, int, int), void (*sub87ed0)(int32_t, int32_t, int32_t),
                     void (*sub88bf0)(int32_t, int32_t)) {
    int32_t t = n_eax * 0xff / 0x14;
    int32_t tgt = t < 1 ? 0 : (t < 0xff ? t : 0xff);
    int32_t h = *h_beae8;
    int32_t obj = n_eax * 0xff;
    if (tgt != 0 && *h_beae8 == 0) {
        *h_beae8 = sub88a70(0, 0, 0, 0);
        if (*h_beae8 == 0) return;
        sub87ed0(0, *h_beae8, 0);  // ECX/EDX hidden; guessed
        h = *h_beae8;
    }
    if (h == 0) return;
    int32_t cur = 0;
    for (Node* p = head_cbf30; p; p = p->next)
        if ((int32_t)p->obj[2] == h) { obj = (int32_t)(intptr_t)p->obj; cur = (int32_t)p->obj[7]; goto found; }
found:
    if (tgt == cur) return;
    int32_t nx = cur < tgt ? (cur + 6 <= tgt ? cur + 6 : tgt) : (tgt <= cur - 6 ? cur - 6 : tgt);
    if (nx == 0) { sub88bf0(obj, h); *h_beae8 = 0; return; }
    sub87ed0(obj, h, nx);
}

// 1.13c D2Client+0x8AFD0 (229 B)
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_8afd0(int32_t* p1, int32_t* p2, uint32_t now, uint32_t t0_b9cc, uint32_t d0_b9d0, uint32_t d1_b9d4,
                     uint32_t d2_b9d8, uint32_t* amp_cbefc, int32_t* o0_cbf00, int32_t* o1_b9dc,
                     uint32_t (*sub18d0)(uint32_t*), uint32_t* rng, void (*sub8ad30)()) {
    uint32_t el = now - t0_b9cc, v;
    if (d2_b9d8 + d1_b9d4 + d0_b9d0 < el) { sub8ad30(); *amp_cbefc = 0; *o0_cbf00 = 0; *o1_b9dc = 0; return; }
    if (el < d0_b9d0) {
        if (d0_b9d0 == 0) { sub8ad30(); return; }
        v = *amp_cbefc * el / d0_b9d0;
    } else {
        v = *amp_cbefc;
        if (d1_b9d4 + d0_b9d0 <= el) {
            if (d2_b9d8 == 0) { sub8ad30(); return; }
            v = (uint32_t)((uint64_t)(uint32_t)((int32_t)(d2_b9d8 - el + d1_b9d4 + d0_b9d0) * (int32_t)*amp_cbefc) / d2_b9d8);
        }
    }
    if (v != 0) {
        *o0_cbf00 = (int32_t)sub18d0(rng);
        *o1_b9dc = (int32_t)sub18d0(rng);
        *p1 += *o0_cbf00; *p2 += *o1_b9dc;
    }
    sub8ad30();
}

// 1.13c D2Client+0x8B0C0 (76 B): obj[3]/obj[4] nonzero -> call sub, clear bit 0 of obj[0].
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_8b0c0(int32_t arg, uint32_t* obj_esi, int32_t state_cc414, void (*sub2a9f0)(int32_t),
                     void (*fatal)()) {
    if (!obj_esi) { fatal(); return; }  // Ordinal_10265(0x1dd); Ordinal_10024; _exit(-1)
    if (obj_esi[3] || obj_esi[4]) {
        if (state_cc414 != 3) sub2a9f0(arg);
        *obj_esi &= 0xfffffffe;
    }
}

// 1.13c D2Client+0x8B2F0 (505 B): four button cells; g[] = DAT_6fb91118/1c/30/34/58/5c/60/64 pairs and cbd offsets.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_8b2f0(int32_t p1, const int32_t* pos /* x,y pairs: 91118,9111c 91130,91134 91160,91164 91158,9115c */,
                     const int32_t* off /* cbdbc,cbdc8 cbd94,cbda0 cbde4,cbdf0 cbdd0,cbddc */,
                     uint32_t h_beb4, uint32_t h_bec0, uint32_t h_bec8, uint32_t uStack_c,
                     int32_t (*ord11139)(int32_t, int32_t), int32_t (*ord10051)(uint32_t, int32_t),
                     Ord10041 ord10041) {
    int32_t a[18] = {0};
    int32_t u = *(int32_t*)(intptr_t)(p1 + 0x60);
    auto draw = [&](int k, uint32_t h, int i) {
        a[0] = k; a[13] = (int32_t)h;
        ord10041(a, pos[2 * i] + off[2 * i], pos[2 * i + 1] + off[2 * i + 1], -1, 5, 0);
    };
    if (u == 0 || ord11139(u, 3) == 0) draw(0, h_beb4, 0);
    if (u == 0 || ord11139(u, 1) == 0) draw(1, h_bec0, 1);
    int32_t i4 = 0, i5 = 0; bool b = false;
    if (u != 0) { i4 = ord11139(u, 4); i5 = ord11139(u, 5); b = ord10051(uStack_c, u) == 2; }
    if (i5 == 0 && (i4 == 0 || !b)) draw(0, h_bec8, 2);
    if (i4 != 0) return;
    if (i5 == 0) draw(0, h_bec8, 3);
    else if (!b) draw(0, h_bec8, 3);
}

// 1.13c D2Client+0x8B600 (71 B)
extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t d2_client_8b600(int32_t x, int32_t y, int32_t px_b9a0, int32_t py_b9a4, int32_t off_bc4c) {
    return px_b9a0 + 0x90 < x && x < px_b9a0 + 0xb8 && py_b9a4 - 0xdf + off_bc4c < y && y < py_b9a4 - 0xb7 + off_bc4c;
}

// 1.13c D2Client+0x8B650 (65 B)
extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t d2_client_8b650(int32_t x, int32_t y, int32_t px_b9a0, int32_t py_b9a4, int32_t off_bc4c) {
    return px_b9a0 + 0x113 < x && x < px_b9a0 + 0x13b && py_b9a4 - 100 + off_bc4c < y && y < py_b9a4 - 0x3c + off_bc4c;
}

// 1.13c D2Client+0x8B900 (81 B): g = {246,25a,26e,282} ids -> out {23c,250,264,278} flags.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_8b900(uint32_t id_eax, const uint16_t* ids4, uint32_t* flags4) {
    for (int i = 0; i < 4; i++) flags4[i] = ids4[i] == id_eax;
}

// 1.13c D2Client+0x8BB20 (51 B)
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_8bb20(uint32_t* state_cbc34, void (*fatal)()) {
    if (*state_cbc34 != 5) { fatal(); return; }  // Ordinal_10265(0x1e90); Ordinal_10024; _exit(-1)
    *state_cbc34 = 6;
}

static void rel(uint32_t* h, void (*o65)(uint32_t), void (*o43)(int32_t, int32_t)) {
    uint32_t v = *h;
    if (v) { o65(v); o43(0x7a, 0); *h = 0; }
}

// 1.13c D2Client+0x8BB90 (84 B): g[0..4] = cbc34, cbc48, cbe7c, b2a4, b430 cleared; then release b35c, clear b2a0.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_8bb90(uint32_t* g5, uint32_t* h_b35c, uint32_t* z_b2a0, void (*o65)(uint32_t),
                     void (*o43)(int32_t, int32_t)) {
    for (int i = 0; i < 5; i++) g5[i] = 0;
    rel(h_b35c, o65, o43);
    *z_b2a0 = 0;
}

// 1.13c D2Client+0x8BF10 (564 B): h[] order = cbc64,cbeb4,cbeb8,cbebc,cbec0,cbec4,cbec8,cbecc,cbc7c,cbed0,cbed4,cbed8.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_8bf10(uint32_t* h12, int32_t c9854, int32_t (*ord10227)(), void (*o65)(uint32_t),
                     void (*o43)(int32_t, int32_t)) {
    for (int i = 0; i < 9; i++) rel(&h12[i], o65, o43);
    if (ord10227() && c9854 && h12[9]) rel(&h12[9], o65, o43);
    rel(&h12[10], o65, o43);
    rel(&h12[11], o65, o43);
    for (int i = 1; i < 9; i++) h12[i] = 0;
}
