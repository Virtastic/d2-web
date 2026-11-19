// src/core/d2client/d2client_batch_60af0.cpp -- 12 UNMAPPED 1.13c D2Client.dll rows (alpha lane),
// from build-wasm/ghidra-d2client/decomp/0x<RVA>.c. Base 0x6FAB0000. Globals are parameters; unrecovered
// callees and ordinals are host callbacks after the retail arguments.
// HIDDEN-REGISTER rows (decompile shows an unaff_/in_ register argument) stay NAMED, and each
// such argument is a GUESS: 60AF0 (EDI=unit), 61490 (EAX=unit, EBX=out byte*), 61780 (stack arg 2),
// 62580 (EBX/EBP/EDI/stack), 62710 (EAX=key), 62E60 (EAX=packet).
#include <cstdint>
#include <emscripten.h>

static inline uint32_t rd32(uint32_t a) { return *(const uint32_t*)(uintptr_t)a; }
static inline int16_t rd16(uint32_t a) { return *(const int16_t*)(uintptr_t)a; }
static inline void wr32(uint32_t a, uint32_t v) { *(uint32_t*)(uintptr_t)a = v; }
static inline void wr16(uint32_t a, int16_t v) { *(int16_t*)(uintptr_t)a = v; }

// 1.13c D2Client+0x61320 (274 B): recursive AVL insert of node `n` into the tree at *root.
// Key order: [n+8] (s16), [n+6] (s16), then group table [0x1155B8 + s16[n+4]*4] (-1 or equal = tie), then [n+4].
// Balance in s16 [node+10]; children [+0xc] left, [+0x10] right. Returns 1 when the subtree height grew.
// rot_l = 0x60750-style (helper at 0x60810 for the left case), rot_r the right one: GUESS which is which
// from the callee order (0x10810 on left-heavy, 0x10750 on right-heavy).
extern "C" EMSCRIPTEN_KEEPALIVE
int32_t d2_client_61320(uint32_t n, uint32_t* root, const int32_t* grp_table_1155b8,
                        void (*rot_left_heavy_60810)(uint32_t*), void (*rot_right_heavy_60750)(uint32_t*)) {
    uint32_t r = *root;
    int32_t cmp;
    if (r == 0) {
        wr32(n + 0xc, 0); wr32(n + 0x10, 0); wr16(n + 10, 0);
        *root = n;
        return 1;
    }
    cmp = (int32_t)rd16(n + 8) - (int32_t)rd16(r + 8);
    if (cmp == 0) {
        cmp = (int32_t)rd16(n + 6) - (int32_t)rd16(r + 6);
        if (cmp == 0) {
            int32_t g = grp_table_1155b8[rd16(n + 4)];
            if (g == -1 || g == grp_table_1155b8[rd16(r + 4)]) { *root = r; return 0; }
            cmp = (int32_t)rd16(n + 4) - (int32_t)rd16(r + 4);
        }
    }
    uint32_t loc = r;
    if (cmp < 0) {
        uint32_t child = rd32(r + 0xc);
        int32_t grew = d2_client_61320(n, &child, grp_table_1155b8, rot_left_heavy_60810, rot_right_heavy_60750);
        wr32(r + 0xc, child);
        if (!grew) { *root = r; return 0; }
        int16_t b = rd16(r + 10);
        if (b == -1) { rot_left_heavy_60810(&loc); *root = loc; return 0; }
        if (b == 0) { wr16(r + 10, -1); *root = r; return 1; }
        wr16(r + 10, 0); *root = r; return 0;   // b == 1
    }
    if (cmp < 1) { *root = r; return 0; }
    uint32_t child = rd32(r + 0x10);
    int32_t grew = d2_client_61320(n, &child, grp_table_1155b8, rot_left_heavy_60810, rot_right_heavy_60750);
    wr32(r + 0x10, child);
    if (!grew) { *root = r; return 0; }
    int16_t b = rd16(r + 10);
    if (b == 0) { wr16(r + 10, 1); *root = r; return 1; }
    if (b == 1) { rot_right_heavy_60750(&loc); *root = loc; return 0; }
    wr16(r + 10, 0); *root = r; return 0;       // b == -1
}

// 1.13c D2Client+0x61440 (72 B): flush the current list: 0x61140, 0x5F300, 0x5F180, free each node of the
// chain at [0x11CC1C0] via next=[node+0x18] (0xBF86: ecx=node, line 0xc1a, 0), then clear [0x11CC1C0] and [0x11CC1C4].
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_61440(uint32_t* head_c1c0, uint32_t* cur_c1c4, void (*sub_61140)(void),
                     void (*sub_5f300)(void), void (*sub_5f180)(void), void (*free_bf86)(uint32_t node, int line, int s1)) {
    sub_61140(); sub_5f300(); sub_5f180();
    uint32_t p = *head_c1c0;
    while (p) { uint32_t node = p; p = rd32(p + 0x18); free_bf86(node, 0xc1a, 0); }
    *head_c1c0 = 0; *cur_c1c4 = 0;
}

// 1.13c D2Client+0x62580 (218 B, ret 0xC): stack (room, force, ctx). Retail: edi = cc4c(ecx=room); base = cc34(room, &cnt);
// for each 0x30-byte record whose flags (+0x14) lack 8 and have 0x20000, or [11C200], or force: 61880(esi=record, ctx+8).
// Then cb0e(room, &cnt) (esi reloaded from the room argument), second loop passes ctx+0xC; then 61d00(room, ctx+0x10).
struct D2c62580Env {
    uint32_t (*cc4c)(uint32_t room);
    uint32_t (*cc34)(uint32_t room, uint32_t* cnt);
    uint32_t (*cb0e)(uint32_t a, uint32_t* cnt);
    void (*f61880)(uint32_t rec, uint32_t edi, uint32_t arg);
    void (*f61d00)(uint32_t esi, uint32_t edi, uint32_t arg);
};
extern "C" EMSCRIPTEN_KEEPALIVE
void d2c_62580(uint32_t room, uint32_t force, uint32_t ctx, uint32_t c200, const D2c62580Env* e) {
    uint32_t cnt = 0;
    uint32_t edi = e->cc4c(room);
    uint32_t base = e->cc34(room, &cnt);
    for (uint32_t i = 0; i < cnt; i++) {
        uint32_t f = rd32(base + 0x14 + i * 0x30);
        if (!(f & 8) && ((f & 0x20000) || c200 || force)) e->f61880(base + i * 0x30, edi, ctx + 8);
    }
    base = e->cb0e(room, &cnt);
    for (uint32_t i = 0; i < cnt; i++) {
        uint32_t f = rd32(base + 0x14 + i * 0x30);
        if (!(f & 8) && ((f & 0x20000) || c200 || force)) e->f61880(base + i * 0x30, edi, ctx + 0xc);
    }
    e->f61d00(room, edi, ctx + 0x10);
}

// 1.13c D2Client+0x62710 (65 B) NAMED: key in EAX (GUESS). Find the list node with *node == key (next [+0x18])
// from [0x11CC1C0], else make one (0x5F240); if it is not the current ([0x11CC1C4]), swap: 0x61140, 0x5F300,
// set current, 0x61E70.
extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t d2_client_62710(uint32_t key, uint32_t head_c1c0, uint32_t* cur_c1c4, uint32_t (*sub_5f240)(uint32_t),
                         void (*sub_61140)(void), void (*sub_5f300)(void), void (*sub_61e70)(void)) {
    uint32_t p = head_c1c0;
    for (; p; p = rd32(p + 0x18)) if (rd32(p) == key) break;
    if (!p) p = sub_5f240(key);   // EAX still holds the key at the call (retail never reloads it)
    if (p != *cur_c1c4) { sub_61140(); sub_5f300(); *cur_c1c4 = p; sub_61e70(); }
    return p;
}

// 1.13c D2Client+0x62CC0 (39 B): Ordinal_477(0x20000, -1, "<str @0x1D5214>", buf@0x11CC0A8, 0x104, 0,0,0) ; return 1.
extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t d2_client_62cc0(void (*ord477)(uint32_t, int32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t),
                         uint32_t str_85214, uint32_t buf_c0a8) {
    ord477(0x20000, -1, str_85214, buf_c0a8, 0x104, 0, 0, 0);
    return 1;
}

// 1.13c D2Client+0x62CF0 (214 B): LNG manager load. Platform (LoadLibraryEx/registry/MessageBox) is
// callback-supplied; the control flow is retail's. Returns 0 on failure, 1 on success.
extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t d2_client_62cf0(char* path_c0a8, uint32_t* lng_c1ac, uint32_t* accel_c0a4,
                         uint32_t (*load_lib)(const char*), int (*reg_read_422)(char*), void (*reg_write_425)(const char*),
                         void (*msgbox)(void), uint32_t (*load_accel)(uint32_t, uint32_t),
                         void (*ord464)(uint32_t, uint32_t, uint32_t),
                         void (*df00)(int32_t (*)(uint32_t, char*, uint32_t)), int32_t (*cb_62c30)(uint32_t, char*, uint32_t)) {
    if (path_c0a8[0]) { *lng_c1ac = load_lib(path_c0a8); reg_write_425(path_c0a8); }
    if (!*lng_c1ac) {
        if (reg_read_422(path_c0a8)) *lng_c1ac = load_lib(path_c0a8);
        if (!*lng_c1ac && !(*lng_c1ac = load_lib("D2.LNG"))) { msgbox(); return 0; }
    }
    *accel_c0a4 = load_accel(*lng_c1ac, 0x64);
    ord464(0xfff, *lng_c1ac, 0);
    df00(cb_62c30);   // 0xDF00 with the 0x62C30 message source, passed by the host as its native address
    return 1;
}

// 1.13c D2Client+0x62E60 (110 B) NAMED: packet in EAX (GUESS; opcode byte then u16 at +1, u16 at +3).
// Dispatch on u16 [pkt+1]. EAX as retail leaves it: the callee's result on the call/tail-jump cases, 1 after case 0x17,
// the u16 [pkt+3] on case 0x24; an unhandled opcode 1..0x24 leaves the byte map's default index 6, any other opcode op-1.
extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t d2_client_62e60(uint32_t pkt, uint32_t (*c740)(uint32_t), uint32_t (*f5e50)(uint32_t), uint32_t (*f65f0)(void),
                     uint32_t (*b2acc0)(uint32_t), uint32_t (*c4660)(void), uint32_t* f8c9c, uint32_t* c404, uint32_t* c3a4,
                     uint8_t* d339f, uint32_t* d33a5) {
    const uint32_t op = (uint16_t)rd16(pkt + 1);
    switch (op) {
    case 1: return c740(pkt);
    case 2: return f5e50((uint16_t)rd16(pkt + 3));
    case 3: case 6: return f65f0();
    case 4: case 0xd: return b2acc0(pkt);
    case 0x17: c4660(); *f8c9c = 0; *c404 = 1; *c3a4 = 1; return 1;
    case 0x24: *d339f = 1; *d33a5 = (uint16_t)rd16(pkt + 3); return *d33a5;
    }
    return op - 1 <= 0x23 ? 6u : op - 1;
}
