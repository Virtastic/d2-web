// src/core/d2win/d2win_47b7.cpp -- D2Win.dll CRT __crtLCMapStringA (MSVC 7.1). Win32 calls are seams.
#include <cstdint>
#include <cstdlib>
#include <emscripten.h>

struct D2Win47b7Env {
    int32_t (*lcmap_w)(uint32_t lcid, uint32_t flags, const uint16_t* src, int32_t n, uint16_t* dst, int32_t m);
    int32_t (*lcmap_a)(uint32_t lcid, uint32_t flags, const char* src, int32_t n, char* dst, int32_t m);
    int32_t (*mb2wc)(uint32_t cp, uint32_t flags, const char* src, int32_t n, uint16_t* dst, int32_t m);
    int32_t (*wc2mb)(uint32_t cp, uint32_t flags, const uint16_t* src, int32_t n, char* dst, int32_t m, const char* defch, int32_t* used);
    uint32_t (*get_last_error)();
    uint32_t (*codepage_of_lcid)(uint32_t lcid);                                   // 0x6E3F, -1 on failure
    char* (*cp_convert)(uint32_t from, uint32_t to, const char* s, int32_t* n, char* dst, int32_t m); // 0x6E86
    int32_t* wapi;       // DAT_6f901314: 0 unknown, 1 W, 2 A
    uint32_t def_lcid;   // DAT_6f901350
    uint32_t def_cp;     // DAT_6f901360
};

// 1.13c D2Win+0x047B7
extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t d2win_crt_lcmapstringa_47b7(const D2Win47b7Env* e, uint32_t lcid, uint32_t flags, const char* src,
                                     int32_t cch, char* dst, int32_t dcch, uint32_t cp, int32_t bErr) {
    if (*e->wapi == 0) {
        static const uint16_t empty[1] = {0};
        if (e->lcmap_w(0, 0x100, empty, 1, nullptr, 0) == 0) { if (e->get_last_error() == 0x78) *e->wapi = 2; }
        else *e->wapi = 1;
    }
    if (cch > 0) {                       // strncnt
        int32_t n = 0;
        while (n < cch && src[n]) n++;
        cch = (n < cch) ? n + 1 : cch;
    }
    if (*e->wapi == 1) {
        if (cp == 0) cp = e->def_cp;
        int32_t wn = e->mb2wc(cp, bErr ? 9 : 1, src, cch, nullptr, 0);
        if (!wn) return 0;
        uint16_t* ws = (uint16_t*)malloc(wn * 2);
        if (!ws) return 0;
        uint32_t r = 0;
        if (e->mb2wc(cp, 1, src, cch, ws, wn)) {
            r = (uint32_t)e->lcmap_w(lcid, flags, ws, wn, nullptr, 0);
            if (r) {
                if (flags & 0x400) {
                    if (dcch && (int32_t)r <= dcch) e->lcmap_w(lcid, flags, ws, wn, (uint16_t*)dst, dcch);
                } else {
                    uint16_t* ms = (uint16_t*)malloc(r * 2);
                    if (!ms) { free(ws); return 0; }
                    if (e->lcmap_w(lcid, flags, ws, wn, ms, (int32_t)r))
                        r = (uint32_t)e->wc2mb(cp, 0, ms, (int32_t)r, dcch ? dst : nullptr, dcch ? dcch : 0, nullptr, nullptr);
                    free(ms);
                }
            }
        }
        free(ws);
        return r;
    }
    if (*e->wapi != 2) return 0;
    if (lcid == 0) lcid = e->def_lcid;
    if (cp == 0) cp = e->def_cp;
    uint32_t lcp = e->codepage_of_lcid(lcid);
    if (lcp == 0xffffffffu) return 0;
    if (lcp == cp) return (uint32_t)e->lcmap_a(lcid, flags, src, cch, dst, dcch);
    int32_t n = cch;
    char* s2 = e->cp_convert(cp, lcp, src, &n, nullptr, 0);
    if (!s2) return 0;
    uint32_t r = 0;
    int32_t m = e->lcmap_a(lcid, flags, s2, n, nullptr, 0);
    if (m) {
        char* t = (char*)calloc(m, 1);
        if (t) {
            m = e->lcmap_a(lcid, flags, s2, n, t, m);
            if (m) {
                char* o = e->cp_convert(lcp, cp, t, &m, dst, dcch);
                r = o ? (uint32_t)m : 0;
                if (o && o != dst) free(o);
            }
            free(t);
        }
    }
    free(s2);
    return r;
}
