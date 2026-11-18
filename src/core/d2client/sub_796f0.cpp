// 1.13c D2Client+0x796F0 (1411 B): skill-panel mouse-down handler. Env carries globals (pointers where
// written) and host callbacks; ordinals and unrecovered callees are callbacks. Decompile register aliasing
// is read as in 78C50: list handle constant, slot index = loop i, mouse x = ev+0xc, y = ev+0xe.
#include <cstdint>
#include <emscripten.h>

static inline uint32_t rd32(uint32_t a) { return *(const uint32_t*)(uintptr_t)a; }

struct D2c796F0Env {
    uint32_t unit_bbfc;                         // DAT_6fbcbbfc
    int32_t w_bc48, x0_b9a0, y0_b9a4, h_bc4c;   // DAT_6fb8bc48, DAT_6fbcb9a0, DAT_6fbcb9a4, DAT_6fb8bc4c
    uint32_t flag_bfc8, flag_5f0;               // DAT_6fbcbfc8, DAT_6fb925f0
    uint8_t* want_24f8;                         // DAT_6fb924f8 (rw)
    int32_t *tab_3d38, *tab_3b38;               // 0x6FBC3D38 / 0x6FBC3B38
    int32_t *g_c3dc, *g_c3e0, *g_c3f8, *g_c3fc; // DAT_6fbcc3dc/3e0/3f8/3fc (rw)
    int32_t *g_bfb4;                            // DAT_6fbcbfb4 (rw)
    int32_t *g_aad90;                           // DAT_6fbaad90 (rw)
    const int32_t *busy7;                       // bbaad88,adbc..: 7 flags (aad88,aadd0,aadbc,aadc0,aae10,aae14,aadd8)
    int32_t *g_c414, *g_c418;                   // DAT_6fbcc414/418 (rw)
    int32_t w_a9e14;                            // DAT_6fba9e14
    int32_t cls_d5d0;                           // DAT_6fb8d5d0 (must be 0x26)
    uint32_t bitmask_0x10;                      // gdwBitMasks[0x10]
    int (*sub_16360)(void);
    int (*ord10587)(uint32_t, int, int);
    int (*ord11120)(uint32_t);
    int (*ord10044)(uint32_t, int);
    uint32_t (*sub_15b0)(int id);               // -> skill rec (short id@0, byte flags@4)
    uint32_t (*sub_17f0)(uint32_t rec);         // -> row (byte@2)
    int (*slot_x_77ca0)(int i);
    int (*slot_y_77c50)(int i);
    void (*sub_14910)(int id, int arg);
    void (*ord10227)(void);
    void (*bad_class)(void);                    // Ordinal_10265(0x139b)/10024/_exit(-1); no return
    void (*on_clear)(void);                     // 10048, FUN_6faecc60(.., &DAT_6fb80604), BE2E0, 77DA0
    void (*ord10006)(void);
    void (*sub_8a4d0)(int, int, int);
    int (*in_rect_779f0)(int, int, int);
    void (*sub_38a70)(void);
    void (*sub_16360b)(void);                   // FUN_6fac6360 latch
    void (*ord511)(uint32_t ev);
};

static void done(const D2c796F0Env* e, uint32_t ev) {
    *(uint32_t*)(uintptr_t)(ev + 0x18) = 1;
    *(uint32_t*)(uintptr_t)(ev + 0x1c) = 0;
    e->ord511(ev);
}

// 1.13c D2Client+0x796F0
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_796f0(uint32_t ev, const D2c796F0Env* e) {
    const int32_t x = *(const uint16_t*)(uintptr_t)(ev + 0xc), y = *(const uint16_t*)(uintptr_t)(ev + 0xe);
    const uint32_t u = e->unit_bbfc;
    if (e->sub_16360() != 0 || !u || y > e->h_bc4c - 0x30) return;
    const uint32_t list = rd32(u + 4);
    const int32_t L = e->w_bc48 - e->x0_b9a0, base = e->y0_b9a4 + e->h_bc4c;
    if (!e->flag_bfc8) {
        *e->g_c3dc = *e->g_c3e0 = *e->g_c3f8 = *e->g_c3fc = 0;
        done(e, ev);
        return;
    }
    auto hit = [&](int i, uint32_t* rec) {
        int id = e->ord10044(list, i);
        *rec = e->sub_15b0(id);
        if (!*rec) return false;
        uint32_t row = e->sub_17f0(*rec);
        if (!row) return false;
        (void)row;
        return true;
    };
    auto inside = [&](int i, uint32_t rec) {
        uint32_t row = e->sub_17f0(rec);
        int sx = e->slot_x_77ca0(i), sy = e->slot_y_77c50(i);
        return *e->want_24f8 == *(const uint8_t*)(uintptr_t)(row + 2) && sx - 0x30 < x && x < sx &&
               sy < y && y < sy + 0x30 && e->tab_3d38[i] == 0;
    };
    if (L - 0x140 < x && x < L - 0x58 && e->ord10587(u, 5, 0) < 1) {
        for (int i = 0; i < e->ord11120(list); i++) {
            uint32_t rec;
            if (!hit(i, &rec)) continue;
            if (inside(i, rec) && !(e->bitmask_0x10 & *(const uint8_t*)(uintptr_t)(rec + 4))) {
                e->sub_14910(*(const int16_t*)(uintptr_t)rec, -1);
                int32_t old = *e->g_aad90;
                e->ord10227();
                if (e->cls_d5d0 != 0x26) e->bad_class();
                *e->g_aad90 = 0;
                if (old) e->on_clear();
                const int32_t* b = e->busy7;
                if (!b[0] && !b[1] && !b[2] && !b[3] && !b[4] && !b[5] && !b[6]) {
                    *e->g_c418 = *e->g_c414 = 0;
                    e->ord10006();
                    e->sub_8a4d0(e->w_a9e14, 0, 0);
                } else {
                    *e->g_c418 = e->w_bc48 / 4;
                    *e->g_c414 = 2;
                    e->ord10006();
                    int32_t q = e->w_a9e14 / 4;
                    e->sub_8a4d0(e->w_a9e14 + q, 0, q);
                }
            } else {
                done(e, ev);
            }
        }
        done(e, ev);
    }
    if (L - 0x58 <= x) {
        if (x <= L) {
            uint8_t prev = *e->want_24f8;
            if (base - 0x174 < y && y < base - 0x109) *e->want_24f8 = 3;
            if (base - 0x108 < y && y < base - 0x9d) *e->want_24f8 = 2;
            if (base - 0x9c < y && y < e->h_bc4c - 0x31) *e->want_24f8 = 1;
            if (*e->want_24f8 != prev) e->sub_38a70();
        }
        done(e, ev);
    }
    if ((e->flag_5f0 == 1 || e->flag_5f0 == 2 || e->flag_5f0 == 3) &&
        e->in_rect_779f0(e->h_bc4c, base - 0x3f, 0x22)) {
        e->sub_38a70();
        *e->g_bfb4 = 1;
    }
    if (L - 0x140 < x && x < L - 0x58) {
        if (e->ord10587(u, 5, 0) < 1) { done(e, ev); return; }
        for (int i = 0; i < e->ord11120(list); i++) {
            uint32_t rec;
            if (!hit(i, &rec) || !inside(i, rec)) continue;
            e->tab_3d38[i] = 1;
            e->tab_3b38[i] = 3;
            e->sub_16360b();
            *(uint32_t*)(uintptr_t)(ev + 0x18) = 1;
            *(uint32_t*)(uintptr_t)(ev + 0x1c) = 0;
            e->sub_38a70();
            e->ord511(ev);
            return;
        }
        done(e, ev);
        return;
    }
    *(uint32_t*)(uintptr_t)(ev + 0x18) = 0;
}
