// 1.13c D2Client+0x78FC0 (1791 B): per-tick UI-bar reconcile; from the cached decompile.
// INTERIM: the DAT_6fbcbfb4==0 branch (decompile has clobbered register vars) is a host seam `scan`.
#include <cstdint>
#include <emscripten.h>

struct D2Client78fc0Env {
    int32_t *pPlayer;            // DAT 0x6FBCBBFC (int*)
    int32_t flagBfc8, flagBfb4, flagBfb8, mode25f0, cnt8d5d0, w4, w9e14, x9a4, bc4c;
    int32_t *a3dc, *a3e0, *a3f8, *a3fc, *a414, *a418;   // 0x6FBCC3DC/3E0/3F8/3FC/414/418
    uint32_t *pAad90;            // 0x6FBAAD90
    int32_t aad80[0x26];         // 0x6FBAAD80 (stride 4)
    const int32_t *kind;         // *(PTR_DAT_6FB8D538[i]+0x10), resolved by host
    int32_t flagsAny;            // OR of DAT 6FBAAD88/AD0/ADBC/ADC0/AE10/AE14/ADD8
    int32_t bc9721, bc3a68;
    void (*f6300)(void*); int (*f6320)();
    int (*inPanel)(int32_t x, int32_t y, int32_t w);     // 0x779F0
    int (*f55440)(); void (*f72790)(int, int, int); void (*f7b20)();
    void (*f6e2e0)(); void (*f7da0)(); void (*ccc0)(); void (*cc60)();
    void (*f3a4d0)(int32_t, int32_t, int32_t); void (*ord10006)(); void (*ord10227)();
    void (*ord511)(void*); void (*fatal)();
    int (*scan)(void* env);      // first-branch body; returns nonzero if it already finished
};

extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_78fc0(uint8_t* p, D2Client78fc0Env* e) {
    uint32_t* w = (uint32_t*)p;
    uint16_t cur = *(uint16_t*)(p + 0xc);
    (void)cur;
    auto done = [&] { w[6] = 1; w[7] = 0; e->ord511(p); };
    if (!e->pPlayer) return;
    e->f6300(p);
    if (e->f6320()) return;
    if (!e->flagBfc8) { *e->a3dc = *e->a3e0 = *e->a3f8 = *e->a3fc = 0; done(); return; }
    if (!e->flagBfb4) { e->scan(e); return; }
    e->flagBfb4 = 0;
    if (e->mode25f0 == 1 || e->mode25f0 == 2) {
        if (!e->inPanel(0, e->x9a4 - 0x3f + e->bc4c, 0x22)) { done(); return; }
        e->ord10227();
        if (e->cnt8d5d0 != 0x26) e->fatal();
        uint32_t old = *e->pAad90, v = old;
        if (!old) {
            if (e->bc3a68) { done(); return; }
            for (int i = 0; i < e->cnt8d5d0; i++) {
                if (e->aad80[i] != 1) continue;
                int k = e->kind[i];
                if (k == 0) continue;
                if (k == 1) { e->f72790(i, 1, 0); v = *e->pAad90; }
                else if (k == 4) { if (e->bc9721) { e->f7b20(); v = *e->pAad90; } }
                else { done(); return; }
            }
        }
        if (!e->pPlayer || (*e->pPlayer == 0 && e->f55440() == 0)) v = *e->pAad90 = (v == 0);
        if (!old) { if (v) e->ccc0(); }
        else if (!v) { e->cc60(); e->f6e2e0(); e->f7da0(); }
        int q = e->w4 / 4, r = e->w9e14 / 4;
        if (!e->flagsAny) {
            if (!v) { *e->a418 = 0; *e->a414 = 0; e->ord10006(); e->f3a4d0(e->w9e14, 0, 0); }
            else { *e->a418 = -q; *e->a414 = 1; e->ord10006(); e->f3a4d0(e->w9e14 - r, 0, -r); }
        } else if (v) {
            *e->a418 = 0; *e->a414 = 3; e->ord10006(); e->f3a4d0(e->w9e14, 0, 0);
            *e->a3dc = *e->a3e0 = *e->a3f8 = *e->a3fc = 0;
        } else { *e->a418 = q; *e->a414 = 2; e->ord10006(); e->f3a4d0(e->w9e14 + r, 0, r); }
    } else if (e->mode25f0 == 3 && e->inPanel(0, e->x9a4 - 0x3f + e->bc4c, 0x22)) e->f72790(4, 2, 0);
    done();
}
