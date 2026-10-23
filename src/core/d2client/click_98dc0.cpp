// D2Client+0x98DC0: left-click mouse event dispatcher (caller 0x9BE70); env-seam style.
// Names are the retail addresses (DAT_6fbXXXXX -> gXXXXX, FUN_6fbXXXXX -> fXXXXX).
#include <cstdint>
#include <cstring>
#if __has_include(<emscripten.h>)
#include <emscripten.h>
#else
#define EMSCRIPTEN_KEEPALIVE
#endif
struct D2c98dc0Env {
    // globals (int32 cells)
    int32_t *gcc3a4, *gbbbfc_ptr /*DAT_6fbcbbfc unit*/, *g8bc48, *gcc474, *gcbc34, *gcbe7c, *gcbedc,
        *g8d5d0, *gaad84, *gaad88, *gaadd0, *gaadbc, *gaadc0, *gaae10, *gaae14, *gaadd8, *gcc418,
        *gcc414, *ga9e14, *gbbb3e4 /*4 consecutive: l,r,t,b*/, *grects /*DAT_6fbcbca0, 11*5 ints*/,
        *gcb864, *gc9854, *gcbc28 /*+0,+4 then gcbc38 +0,+4 via gcbc38*/, *gcbc38,
        *gbb28c /*l,r,t,b*/, *gbb39c /*l,r,t,b*/, *gbb2d4 /*l,r,t,b*/, *gbb3cc /*l,r,t,b*/;
    // functions
    void (*f408c0)(); int32_t (*f3d3d0)(); void (*f38a70)(int, int, int, int);
    int32_t (*f3bd90)(int, int); int32_t (*f6320)(); int32_t (*f3c580)(int);
    void (*ord10227)(); int32_t (*ord10265)(int); void (*ord10024)(int32_t str, int32_t v);
    void (*fatal)();  // _exit(-1)
    void (*f6f720)(); int32_t (*ord10048)(); void (*faecc60)(int, int); void (*f6e2e0)(); void (*f3bbf0)();
    void (*ord10006)(); void (*f3a4d0)(int, int, int); void (*ord511)(uint8_t* ev);
    void* (*f46aa0)(int32_t unit, uint32_t a); int32_t (*ord11017)(uint32_t); int32_t (*ord11139)(uint32_t, int);
    int32_t (*f3c2b0)(int32_t); int32_t (*f31cb0)(); int32_t (*f423f0)(); void (*fac4910)(int, int);
    void (*f31ce0)(); void (*f31e40)(); void (*f6eb90)(); void (*fac4940)();
    int32_t (*f3c150)(int, int); void (*f40f60)(int); void (*f095c0)(); void (*f3f880)(int);
    void* (*rectFn)(int idx);  // DAT_6fb90ec4[idx](idx)
    int32_t (*uget)(int32_t ptr, int off);  // 32-bit retail struct dword read
    const uint8_t* str359f;    // &DAT_6fb8359f (passed as handle by caller)
};
extern "C" {
// 1.13c D2Client+0x98DC0
EMSCRIPTEN_KEEPALIVE void d2c_98dc0(const D2c98dc0Env* e, uint8_t* ev) {
    auto W = [&](int o, int32_t v) { memcpy(ev + o, &v, 4); };
    auto Z = [&] { W(0x18, 1); W(0x1c, 0); };
    auto clr = [&] { e->gcbc28[0] = 0; e->gcbc28[1] = 0; e->gcbc38[0] = 0; e->gcbc38[1] = 0; };
    int32_t unit = *e->gbbbfc_ptr;
    if (*e->gcc3a4 || !unit) { W(0x18, 0); return; }
    int32_t st = e->uget(unit, 0x10);
    if (st == 0x11) { W(0x18, 0); return; }
    uint16_t sy, sx; memcpy(&sx, ev + 0xc, 2); memcpy(&sy, ev + 0xe, 2);
    int32_t x = sx, y = sy;
    if (*e->g8bc48 < x) { Z(); e->ord511(ev); }
    e->f408c0();
    uint32_t u60 = e->uget(unit, 0x60);
    bool hit = false;
    if (*e->gcc474 == 0) {
        int32_t s = *e->gcbc34;
        if (e->f3d3d0() && (s == 0 || s == 0xb || s == 0xc || s == 0xf)) { *e->gcbe7c = 1; e->f38a70(0, 0, 0, 0); }
        if (e->f3bd90(x, y) && (s == 0 || s == 0x13)) { *e->gcbedc = 1; Z(); e->f38a70(0, 0, 0, 0); e->ord511(ev); return; }
    }
    if (!e->f6320() && e->f3c580(y)) {
        int32_t prev = *e->gaad84;
        if (e->f3bd90(x, y)) {
            *e->gcbedc = 1; e->ord10227();
            if (*e->g8d5d0 != 0x26) { e->ord10024((int32_t)(intptr_t)e->str359f, e->ord10265(0x139b)); e->fatal(); return; }
            *e->gaad84 = 0;
            if (prev) { e->f6f720(); e->faecc60(e->ord10048(), 0); e->f6e2e0(); }
            e->f3bbf0();
            if (!*e->gaad88 && !*e->gaadd0 && !*e->gaadbc && !*e->gaadc0 && !*e->gaae10 && !*e->gaae14 && !*e->gaadd8) {
                *e->gcc418 = 0; *e->gcc414 = 0; e->ord10006(); e->f3a4d0(*e->ga9e14, 0, 0); Z();
            } else {
                *e->gcc418 = *e->g8bc48 / 4; *e->gcc414 = 2; e->ord10006();
                int32_t q = *e->ga9e14 / 4; e->f3a4d0(*e->ga9e14 + q, 0, q); Z();
            }
            e->f38a70(0, 0, 0, 0); e->ord511(ev); return;
        }
        Z();
        const int32_t* r = e->gbbb3e4;  // l,r,t,b
        void* res = nullptr;
        if (x < r[0] || r[1] < x || y < r[2] || r[3] < y) {
            bool fin = false;
            for (int i = 0; i < 11; i++) {
                const int32_t* c = e->grects + i * 5;
                if (!(c[0] <= x && x <= c[1] && c[2] <= y && y <= c[3])) continue;
                int32_t o = e->ord11017(u60);
                if (*e->gcb864 != 6 && *e->gcb864 != 8) {
                    int32_t o2 = e->ord11139(u60, i);
                    if (e->f3c2b0(o)) {
                        if (!e->f31cb0() && e->f423f0()) {
                            e->fac4910(o ? e->uget(o, 0xc) : -1, o2 ? e->uget(o2, 0xc) : -1);
                            e->f31ce0(); e->f31e40(); e->f38a70(0, 0, 0, 0); hit = true;
                        }
                        fin = hit; break;
                    }
                }
                res = e->rectFn(i); hit = true; fin = true; break;
            }
            if (!fin && *e->gc9854 && e->f3c150(11, x)) { W(0x18, 1); e->f40f60(11); e->ord511(ev); return; }
        } else { res = e->f46aa0(unit, u60); hit = true; }
        Z();
        if (!res) { if (*e->gcb864 == 6) e->f6eb90(); else if (*e->gcb864 == 8) e->fac4940(); }
        e->ord511(ev); clr();
        if (hit) return;
        e->f3f880(y); return;
    }
    {
        int32_t s = *e->gcbc34; const int32_t* b = nullptr;
        if (s == 0xb) {
            b = e->gbb28c;
            if (b[0] <= x && x <= b[1] && b[2] <= y && y <= b[3]) {
                void* r = e->f46aa0(unit, u60); Z(); clr(); e->ord511(ev);
                if (r) e->f095c0();
                return;
            }
        } else {
            if (s == 0xc) b = *e->gc9854 ? e->gbb2d4 : e->gbb39c;
            else if (s == 0xe) b = e->gbb3cc;
            if (b && b[0] <= x && x <= b[1] && b[2] <= y && y <= b[3]) {
                e->f46aa0(unit, u60); Z(); clr(); e->ord511(ev); return;
            }
        }
    }
    W(0x18, 0);
    e->f3f880(y);
}
}
