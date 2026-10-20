// src/core/d2client/d2client_batch_4a3d0.cpp -- D2Client.dll 1.13c rows 0x4A3D0 and 0x4A4F0 from the
// cached decompiles. Alpha batch lane (OBJECTIVE v8.3): implementation + marker, no spec, no vector.
// Base 0x6FAB0000. Globals live in D2C4A3D0State (retail VA in each comment); every unrecovered callee
// is a seam on D2C4A3D0Env, unverified, retail argument order kept.
#include <cstdint>
#include <cstddef>
#include <emscripten.h>

struct D2C4A3D0State {
    int32_t  cur_unit;        // 6fbcbbfc
    int32_t  slot_count;      // 6fba3b9c
    uint8_t* slots;           // 6fba37a8, stride 0x16: +0 id(i32) +0x11 flag +0x12 flag2 +0x15 flag3
    uint8_t  f_9663, f_9721, f_9729; // 6fbc9663 / 6fbc9721 / 6fbc9729
    int32_t  v_966b;          // 6fbc966b
    int32_t  v_971d, v_9725;  // 6fbc971d / 6fbc9725
    int32_t  e_0ea8[6];       // 6fb90ea8..ebc, reset to -1
    int32_t  g_bc28, g_bc2c, g_bc38, g_bc3c; // 6fbcbc28/2c/38/3c
    int32_t  a_ada0;          // 6fbaada0
    int32_t  n_d5d0;          // 6fb8d5d0, must equal 0x26
    int32_t  g_46c4, g_46c8;  // 6fba46c4/c8
    int32_t  g_c3dc, g_c3e0, g_c3f8, g_c3fc, g_c2f8, g_c2f4; // 6fbcc3dc.. cleared
    int32_t  g_97ac, g_97b0;  // 6fbc97ac / 6fbc97b0
    int32_t  g_23e8;          // 6fba23e8
    int32_t  g_3a68;          // 6fbc3a68
    int32_t  g_973b;          // 6fbc973b
    uint8_t  tbl_ad80[0x26];  // 6fbaad80
    int32_t* tbl_d538[0x26];  // 6fb8d538 -> record, state dword at +0x20 (indexed as int32)
};

struct D2C4A3D0Env {
    void     (*fatal)(uint32_t strId);              // Ordinal_10265 + Ordinal_10024 + _exit(-1)
    void     (*sub_6910)();                         // 0x46910
    int32_t* (*sub_6150)();                         // 0x46150
    void     (*sub_7B20)();                         // 0x47B20
    void     (*ord_10227)();                        // Ordinal_10227
    void     (*sub_E2E0)();                         // 0xBE2E0
    void     (*sub_92F0)();                         // 0x492F0
    void     (*sub_8150)();                         // 0x48150
    int32_t  (*ord_10973)(int32_t, int32_t, int32_t);
    void     (*sub_E350)(int32_t, int32_t, int32_t* outFlag); // 0xBE350 (out = uStack_4)
    void     (*sub_5CC0)(void*, int32_t, int32_t);  // 0x45CC0
    void     (*sub_38BF0)(int32_t);                 // 0x88BF0 (extraout_ECX + ptr)
    void     (*sub_A210)();                         // 0x4A210
    void     (*sub_A060)(uint32_t);                 // 0x4A060
    void     (*sub_43E0)(void*);                    // 0x143E0
    void     (*sub_6A60)(int32_t);                  // 0x46A60
    void     (*sub_72790)(uint32_t, int32_t, int32_t); // 0xC2790
    int32_t  (*sub_55440)();                        // 0xA5440
    void     (*sub_8B20)();                         // 0x48B20
    int32_t  (*sub_4CCE0)(int32_t);                 // 0x9CCE0
    void     (*sub_2EB0)();                         // 0x22EB0
    int32_t  (*sub_38A70)(int32_t, int32_t, int32_t, int32_t); // 0x88A70
};

// 1.13c D2Client+0x4A3D0 -- `unit` arrives in EAX.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_4a3d0(D2C4A3D0State* g, const D2C4A3D0Env* e, int32_t unit) {
    if (unit != g->cur_unit) return;
    for (int32_t i = 0; i < g->slot_count; i++) {
        uint8_t* s = g->slots + i * 0x16;
        if (s[0x11] == 1) {
            s[0x11] = 0;
            if (g->cur_unit == 0) { e->fatal(0xfab); return; }
            e->sub_6910();
        }
        s[0x14] = 1;
    }
    if (!e->sub_6150()) return;
    g->f_9663 = 0;
    for (int i = 0; i < 6; i++) g->e_0ea8[i] = -1;
    g->g_bc28 = g->g_bc2c = g->g_bc38 = g->g_bc3c = 0;
    e->sub_7B20();
    int32_t old = g->a_ada0;
    e->ord_10227();
    if (g->n_d5d0 != 0x26) { e->fatal(0x139b); return; }
    g->a_ada0 = 0;
    if (old) { e->sub_E2E0(); e->sub_92F0(); }
    g->f_9721 = 0;
}

// 1.13c D2Client+0x4A4F0 --
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_4a4f0(D2C4A3D0State* g, const D2C4A3D0Env* e, int32_t* p) {
    g->g_46c8 = g->g_46c4 = 0x10;
    g->g_c3dc = g->g_c3e0 = g->g_c3f8 = g->g_c3fc = g->g_c2f8 = g->g_c2f4 = 0;
    if (!p) { e->sub_8150(); return; }
    g->f_9663 = 1;
    g->f_9729 = 0;
    g->v_966b = e->ord_10973(g->cur_unit, 0xe, 0);
    int32_t flag = 0;
    e->sub_E350(0, 0, &flag);
    if (flag == 0) {
        int32_t* c = e->sub_6150();
        if (p == c) return;
        if (e->sub_6150()) { e->sub_8150(); return; }
    } else {
        e->sub_6150();
    }
    g->f_9721 = 1;
    g->v_971d = p[3];
    g->v_9725 = p[1];
    int32_t id = p[1];
    p[0x31] &= ~2;
    e->sub_5CC0(nullptr /* &DAT_6fba2330 */, id, g->g_973b);
    if (g->g_97ac) {
        if (g->g_97b0) { e->sub_38BF0(g->g_97b0); g->g_97b0 = 0; }
        g->g_97ac = 0;
    }
    uint32_t u = (uint32_t)p[1];
    bool call = false;
    uint32_t arg = 0;
    if (u < 0xfd) {
        if (u == 0xfc || u == 0xc6) { call = true; arg = g->g_23e8 != -1; }
        else if (u == 0x96) {
            if (e->ord_10973(g->cur_unit, 0xc, 0) > 7) e->sub_A210();
            call = true; arg = g->g_23e8 != -1;
        }
    } else if (u == 0x16f || u == 0x203) { call = true; arg = g->g_23e8 != -1; }
    if (call) e->sub_A060(arg);

    int32_t prev = g->a_ada0;
    e->ord_10227();
    if (g->n_d5d0 != 0x26) { e->fatal(0x139b); return; }
    bool bail = g->g_3a68 != 0;
    for (uint32_t i = 0; !bail && (int32_t)i < g->n_d5d0; i++) {
        if (g->tbl_ad80[i] != 1) continue;
        switch (g->tbl_d538[i][8]) {   // dword at +0x20
        case 0: break;
        case 1: e->sub_72790(i, 1, 0); break;
        case 4: if (g->f_9721) e->sub_7B20(); break;
        default: bail = true;
        }
    }
    if (bail) {
        g->f_9663 = 0;
        p[0x31] |= 2;
        e->sub_43E0(nullptr /* stack buffer, content unrecovered */);
        g->f_9721 = 0;
        e->sub_6A60(p[3]);
        return;
    }
    if (g->cur_unit == 0 || (*(int32_t*)(intptr_t)g->cur_unit == 0 && e->sub_55440() == 0)) g->a_ada0 = 1;
    if (prev && g->a_ada0 == 0) { e->sub_E2E0(); e->sub_92F0(); }
    e->sub_8B20();
    if (flag != 0) return;
    int32_t kind = 0, k = 0;
    for (; k < g->slot_count; k++) {
        uint8_t* s = g->slots + k * 0x16;
        if (*(int32_t*)s != id) continue;
        if (s[0x12] == 1) { s[0x12] = 0; kind = 2; }
        if (s[0x15] != 1) return;
        s[0x15] = 0;
        break;
    }
    if (e->sub_4CCE0(kind)) {
        e->sub_2EB0();
        g->g_97b0 = e->sub_38A70(g->cur_unit, 0, 1, 0);
        if (kind == 2) {
            uint8_t buf[4] = { 0x4d, (uint8_t)id, (uint8_t)(id >> 8), 0 };
            e->sub_43E0(buf);
            g->g_97ac = 1;
            return;
        }
        g->g_97b0 = 0;
    }
}
