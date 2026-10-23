// 1.13c D2Client+0x78C50 (705 B): skill-panel slot pass. Env carries the globals and host callbacks;
// ordinals and unrecovered callees are callbacks. Decompile register aliasing (iVar14/iVar13 always 0,
// iVar8 re-read as 0 after pass 1, table index iVar10) is read as: list handle constant, index = loop i.
#include <cstdint>
#include <emscripten.h>

static inline uint32_t rd32(uint32_t a) { return *(const uint32_t*)(uintptr_t)a; }
static inline int16_t rd16(uint32_t a) { return *(const int16_t*)(uintptr_t)a; }

struct D2c78C50Env {
    uint32_t unit_bbfc, flag_bfb8, dt_sgpt;   // DAT_6fbcbbfc, DAT_6fbcbfb8, sgptDataTables
    int32_t y0_b9a4, y1_bc4c;                 // DAT_6fbcb9a4, DAT_6fb8bc4c
    uint8_t want_24f8;                        // DAT_6fb924f8
    int32_t *tab_3d38, *tab_3b38;             // 0x6FBC3D38 / 0x6FBC3B38
    void (*itoa_unicode)(uint32_t value);     // wsprintfA("%i") + Unicode::toUnicode(buf,9)
    int (*ord10587)(uint32_t, int, int);
    int (*ord10494)(uint32_t, int);
    void (*ord10150)(int, int);
    int (*ord11120)(uint32_t);
    int (*ord10044)(uint32_t, int);
    int (*ord11101)(uint32_t, int);
    int (*ord10708)(int);
    int (*ord10306)(uint32_t, int, int);
    int (*ord11021)(uint32_t, int);
    int (*ord10786)(uint32_t, int, int, int, uint32_t, int, int, int);
    int (*sub_17b0)(void);
    void (*sub_78820)(void);
};

// 1.13c D2Client+0x78C50
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_78c50(const D2c78C50Env* e) {
    const uint32_t u = e->unit_bbfc;
    if (!u) return;
    const uint32_t list = rd32(u + 4);
    e->itoa_unicode((uint32_t)e->ord10587(u, 5, 0));
    int on = ((e->ord10494(u, 0x36) != 0) || e->flag_bfb8 != 0) ? 1 : 0;
    const int32_t y = e->y0_b9a4 - 400 + e->y1_bc4c;
    e->ord10150(y, on);
    e->ord10587(u, 5, 0);
    int32_t lim = (int32_t)list;   // iVar11: passed to 10786, zeroed once a cooldown is covered
    for (int i = 0; i < e->ord11120(list); i++) {
        int id = e->ord10044(list, i);
        if (id < 0 || id >= (int32_t)rd32(e->dt_sgpt + 0xba0)) continue;
        uint32_t ent = rd32(e->dt_sgpt + 0xb98) + (uint32_t)id * 0x23c;
        if (!ent) continue;
        int k = rd16(ent + 0x194);
        if (k < 0 || k >= (int32_t)rd32(e->dt_sgpt + 0xb94)) continue;
        uint32_t r = (uint32_t)k * 0x120 + rd32(e->dt_sgpt + 0xb8c);
        if (!r || e->want_24f8 != *(const uint8_t*)(uintptr_t)(r + 2)) continue;
        if (e->ord11101(u, id) == 0) {
            e->tab_3d38[i] = -1; e->tab_3b38[i] = 5;
            int v = e->ord10708(-1);
            if (v) e->ord10306(u, v, 1);
        } else {
            int v = e->ord10708(-1), cd = 0;
            if (v) cd = e->ord10306(u, v, 0);
            if (e->sub_17b0() <= cd) lim = 0;
            if (e->ord11021(u, id) == 0) v = 0;
            int n = 1;
            if ((int32_t)rd32(ent + 0x170) != -1)
                n = e->ord10786(u, (int32_t)rd32(ent + 0x170), id, cd, list, i, lim, 1);
            if (0 < n || v == 0) { e->tab_3d38[i] = -1; e->tab_3b38[i] = 5; }
            else if (e->tab_3d38[i] != 1) { e->tab_3d38[i] = 0; e->tab_3b38[i] = 0; }
        }
        e->sub_78820();
    }
}
