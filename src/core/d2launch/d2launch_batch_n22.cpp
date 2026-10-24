// src/core/d2launch/d2launch_batch_n22.cpp -- UNMAPPED 1.13c D2Launch.dll alpha-lane rows (batch n22).
// Win32 / ordinal / DAT_ callees are host seams (function pointers); retail 32-bit layouts.
#include <cstdint>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <ctime>

// Host seams
struct N22Host {
    int (*GetStringTypeW)(uint32_t, const uint16_t*, int, uint16_t*);
    int (*GetStringTypeA)(uint32_t lcid, uint32_t type, const char*, int, uint16_t*);
    int (*MultiByteToWideChar)(uint32_t cp, uint32_t fl, const char*, int, uint16_t*, int);
    int (*WideCharToMultiByte)(uint32_t cp, uint32_t fl, const uint16_t*, int, char*, int, const char*, int*);
    int (*LCMapStringW)(uint32_t, uint32_t, const uint16_t*, int, uint16_t*, int);
    int (*LCMapStringA)(uint32_t, uint32_t, const char*, int, char*, int);
    int (*GetCPInfoMaxChar)(uint32_t cp);                 // -1 on failure, else MaxCharSize
    uint32_t (*GetLastError)();
    uint32_t (*GetTickCount)();
    uint32_t (*cp_for_lcid)(uint32_t lcid);               // 0x93E6, 0xFFFFFFFF on failure
    int32_t  gst_mode;                                    // DAT_6fa65194
    int32_t  lcmap_mode;                                  // DAT_6fa651bc
    uint32_t def_lcid;                                    // DAT_6fa6517c
    uint32_t def_cp;                                      // DAT_6fa6518c
};

// 1.13c D2Launch+0x942D
char* n22_crtConvertCP(N22Host* h, uint32_t cpFrom, uint32_t cpTo, const char* src, uint32_t* pcb, char* out, int outCb) {
    uint32_t cb = *pcb;
    bool sb = false;
    char* res = nullptr;
    if (cpFrom == cpTo) return nullptr;
    if (h->GetCPInfoMaxChar(cpFrom) == 1 && h->GetCPInfoMaxChar(cpTo) == 1) sb = true;
    uint32_t wl;
    if (sb && cb == 0xFFFFFFFFu) wl = (uint32_t)strlen(src) + 1;
    else if (sb) wl = cb;
    else { wl = (uint32_t)h->MultiByteToWideChar(cpFrom, 1, src, (int)cb, nullptr, 0); if (!wl) return nullptr; }
    uint16_t* w = (uint16_t*)calloc(wl, 2);
    if (!w) return nullptr;
    if (h->MultiByteToWideChar(cpFrom, 1, src, (int)cb, w, (int)wl)) {
        if (!out) {
            if (sb || (wl = (uint32_t)h->WideCharToMultiByte(cpTo, 0, w, (int)wl, nullptr, 0, nullptr, nullptr))) {
                res = (char*)calloc(wl, 1);
                if (res) {
                    uint32_t n = (uint32_t)h->WideCharToMultiByte(cpTo, 0, w, (int)wl, res, (int)wl, nullptr, nullptr);
                    if (!n) { free(res); res = nullptr; }
                    else if (cb != 0xFFFFFFFFu) *pcb = n;
                }
            }
        } else if (h->WideCharToMultiByte(cpTo, 0, w, (int)wl, out, outCb, nullptr, nullptr)) res = out;
    }
    free(w);
    return res;
}

// 1.13c D2Launch+0x8C78
int n22_crtGetStringTypeA(N22Host* h, uint32_t type, const char* src, int cch, uint16_t* outType, uint32_t cp, int bError) {
    if (h->gst_mode == 0) {
        uint16_t t[2]; static const uint16_t e[1] = {0};
        if (!h->GetStringTypeW(1, e, 1, t)) { if (h->GetLastError() == 0x78) h->gst_mode = 2; }
        else h->gst_mode = 1;
    }
    if (h->gst_mode == 2 || h->gst_mode == 0) {
        uint32_t lcid = cp ? cp : h->def_lcid;
        uint32_t scp = h->cp_for_lcid(lcid);
        if (scp == 0xFFFFFFFFu) return 0;
        char* conv = nullptr;
        uint32_t n = (uint32_t)cch;
        if (scp != h->def_cp) {
            conv = n22_crtConvertCP(h, h->def_cp, scp, src, &n, nullptr, 0);
            if (!conv) return 0;
            src = conv; cch = (int)n;
        }
        int r = h->GetStringTypeA(lcid, type, src, cch, outType);
        free(conv);
        return r;
    }
    if (h->gst_mode == 1) {
        if (!cp) cp = h->def_cp;
        int wl = h->MultiByteToWideChar(cp, (bError != 0) * 8 + 1, src, cch, nullptr, 0);
        if (!wl) return 0;
        uint16_t* w = (uint16_t*)calloc((size_t)wl, 2);
        if (!w) return 0;
        int r = 0;
        if (h->MultiByteToWideChar(cp, 1, src, cch, w, wl)) r = h->GetStringTypeW(type, w, wl, outType);
        free(w);
        return r;
    }
    return 0;
}

// 1.13c D2Launch+0x8EC0
uint32_t n22_crtLCMapStringA(N22Host* h, uint32_t lcid, uint32_t flags, const char* src, int cch, char* dst, int dstCb, uint32_t cp, int bError) {
    if (h->lcmap_mode == 0) {
        static const uint16_t e[1] = {0};
        if (!h->LCMapStringW(0, 0x100, e, 1, nullptr, 0)) { if (h->GetLastError() == 0x78) h->lcmap_mode = 2; }
        else h->lcmap_mode = 1;
    }
    if (cch > 0) { int i = 0; while (i < cch && src[i]) i++; cch = (i < cch) ? i + 1 : i; }
    if (h->lcmap_mode != 2 && h->lcmap_mode != 0) {
        if (h->lcmap_mode != 1) return 0;
        if (!cp) cp = h->def_cp;
        int wl = h->MultiByteToWideChar(cp, (bError != 0) * 8 + 1, src, cch, nullptr, 0);
        if (!wl) return 0;
        uint16_t* w = (uint16_t*)calloc((size_t)wl, 2);
        if (!w) return 0;
        uint32_t r = 0;
        uint16_t* m = nullptr;
        if (h->MultiByteToWideChar(cp, 1, src, cch, w, wl)) {
            r = (uint32_t)h->LCMapStringW(lcid, flags, w, wl, nullptr, 0);
            if (r) {
                if (!(flags & 0x400)) {
                    m = (uint16_t*)calloc(r, 2);
                    if (m && h->LCMapStringW(lcid, flags, w, wl, m, (int)r))
                        r = (uint32_t)h->WideCharToMultiByte(cp, 0, m, (int)r, dst, dstCb, nullptr, nullptr);
                } else if (dstCb && (int)r <= dstCb)
                    h->LCMapStringW(lcid, flags, w, wl, (uint16_t*)dst, dstCb);
            }
        }
        free(m); free(w);
        return r;
    }
    if (!lcid) lcid = h->def_lcid;
    if (!cp) cp = h->def_cp;
    uint32_t scp = h->cp_for_lcid(lcid);
    if (scp == 0xFFFFFFFFu) return 0;
    if (scp == cp) return (uint32_t)h->LCMapStringA(lcid, flags, src, cch, dst, dstCb);
    uint32_t n = (uint32_t)cch;
    char* a = n22_crtConvertCP(h, cp, scp, src, &n, nullptr, 0);
    if (!a) return 0;
    uint32_t r = 0;
    char* conv = nullptr;
    int len = h->LCMapStringA(lcid, flags, a, (int)n, nullptr, 0);
    if (len) {
        char* tmp = (char*)calloc((size_t)len, 1);
        uint32_t ln = (uint32_t)len;
        if (tmp && (ln = (uint32_t)h->LCMapStringA(lcid, flags, a, (int)n, tmp, len))) {
            conv = n22_crtConvertCP(h, scp, cp, tmp, &ln, dst, dstCb);
            r = conv ? ln : 0;
        }
        free(tmp);
    }
    free(a);
    if (conv && dst != conv) free(conv);
    return r;
}

// 1.13c D2Launch+0x9296
int n22_setmode_lk(uint8_t** ioinfo, uint32_t fh, int mode, int* errnoOut) {
    uint8_t* fl = ioinfo[(int32_t)fh >> 5] + 4 + (fh & 0x1f) * 0x24;
    uint8_t old = *fl;
    if (mode == 0x8000) *fl &= 0x7f;
    else if (mode == 0x4000) *fl |= 0x80;
    else { *errnoOut = 0x16; return -1; }
    return ((old & 0x80) ? (int)0xffffc000 : 0) + 0x8000;
}

// 1.13c D2Launch+0x935E
long n22_atol(const char* s, bool (*isspace_)(uint8_t)) {
    while (isspace_((uint8_t)*s)) s++;
    uint32_t c = (uint8_t)*s++, first = c;
    if (c == '-' || c == '+') c = (uint8_t)*s++;
    int v = 0;
    while (c >= '0' && c <= '9') { v = (int)(c - '0') + v * 10; c = (uint8_t)*s++; }
    return first == '-' ? -v : v;
}

// UI-side seams (ordinals / FUN_ callees)
struct N22Ui {
    void (*Ord10089)(); void (*Ord10124)();               // 0x9A3C, 0x99FA
    void (*Ord10147)(); void (*Ord10106)(); void (*Ord10022)(int);
    int (*Ord502)(); void (*Ord10003_fn)(); int (*Ord10227)();
    void (*ae60)(); void (*a8c0)(); void (*b600)(); void (*bda0)(); void (*c3f0)(); void (*cc50)();
    void (*cb00)(const char*, int); void (*fn5090)(const void*, void (*)(), int); void (*fn7df0)();
    void (*ord10003_str)(void** out);
    uint32_t (*GetTickCount)();
    int32_t (*gate68)();                                  // (*(DAT_6fa656c4+0x68))()
    int32_t* dat_ec4; int32_t* dat_eb8; int32_t* dat_ebc; int32_t* dat_ee4; int32_t* dat_ad4;
    int32_t* list_ef4;                                    // DAT_6fa65ef4 chain head (nodes of 4 ints)
    void (*ordinal_10182)(int, int, int); int32_t* dat_ab0; int32_t* dat_ed0; int32_t* dat_db0;
    int32_t* dat_ed4; int32_t* dat_dbc; int32_t* dat_710; int32_t* dat_ec8;
    void (*ordinal_10131)();
};

// 1.13c D2Launch+0x9BD0
void n22_9BD0(N22Ui* u) { u->Ord10089(); u->Ord10124(); }

// Selector in EAX 0..6 (else default); each arm: Unicode::strcpy(dst, Ordinal_10003()) via the host seam.
// 1.13c D2Launch+0x9E40
void n22_9E40(N22Ui* u, uint32_t sel, void** out) { (void)sel; u->ord10003_str(out); }

// 1.13c D2Launch+0x9BF0
int n22_9BF0(N22Ui* u) {
    int32_t node = *u->dat_ec8;
    int i = 0;
    if (*u->dat_ed0 == 0) *u->dat_db0 = 0;
    int32_t d = *u->dat_db0 - *u->dat_ed4;
    if (d >= 0 && d < *u->dat_dbc) u->ordinal_10182(*u->dat_ab0, ((d & 1) ? 0x110 : 0) + 0x25, 0);
    u->ordinal_10131();
    for (; node && *u->dat_db0 != i; i++) node = *(int32_t*)(intptr_t)(node + 0x34c);
    u->Ord10147(); u->Ord10147();
    if (!node) { u->Ord10147(); u->Ord10147(); u->Ord10147(); u->ordinal_10131(); return 1; }
    u->Ord10147();
    if (*(uint8_t*)(intptr_t)(node + 0x324) & 0x20) u->Ord10147();
    u->Ord10147();
    if (*u->dat_ad4 == 1 && *u->dat_ee4) u->Ord10147();
    u->Ord10106(); u->Ord10022(0);
    return 1;
}

// 1.13c D2Launch+0xCD70
// ponytail: list node {key,str,expiry,next}; ceiling = host passes pointers as intptr.
void n22_CD70(N22Ui* u) {
    u->ae60();
    *u->dat_ec4 = 0;
    u->Ord10147();
    int key = u->Ord502();
    uint32_t now = u->GetTickCount();
    for (int32_t* n = u->list_ef4; n; n = (int32_t*)(intptr_t)n[3]) {
        if (n[0] == key && (int32_t)now <= n[2]) {
            if (n[1]) { u->cb00((const char*)(intptr_t)n[1], 0); return; }
            break;
        }
    }
    if (!u->gate68()) { u->fn5090(nullptr, u->fn7df0, 1); return; }
    *u->dat_eb8 = 1;
    *u->dat_ebc = 0;
}

// 1.13c D2Launch+0xD050
void n22_D050(N22Ui* u) {
    *u->dat_ee4 = 0;
    int m = *u->dat_ad4;
    if (m == 1) { u->Ord10106(); u->Ord10022(0); n22_CD70(u); u->a8c0(); u->b600(); return; }
    if (m == 0 || m == 3 || m == 2) {
        u->ae60();
        if (u->Ord10227() == 0) u->c3f0();
        u->cc50();
    }
    u->a8c0(); u->b600();
}

// 1.13c D2Launch+0xD140
int n22_D140(N22Ui* u) { n22_D050(u); u->bda0(); return 1; }

// Game-list row painter. Retail Unicode/sprintf work is delegated to the host (row_text) seam.
struct N22Rows {
    int32_t head; int32_t count; int32_t scroll; int32_t base710; int32_t mode; int32_t gateEB8; int32_t dedc;
    uint32_t (*Ord10009)(); void (*Ord10094)(int32_t); void (*Ord10106)();
    void (*Ord10140)(int32_t ctl, int y); void (*Ord10179)(int32_t ctl, int v); void (*Ord10194)(int32_t ctl, uint32_t f);
    void (*row_text)(int32_t entry, int rowIdx, int x);   // 3-col name/level/flags/expiry text (Ordinals 10003/10000/10205/10022)
    uint32_t invMask10, mask10, invMask14, mask14;        // gdwInvBitMasks/gdwBitMasks [4],[5]
};
// 1.13c D2Launch+0xAF00
void n22_AF00(N22Rows* r) {
    int32_t e = r->head, idx = 0, y = 0x25, x = r->base710 - 0xd;
    for (int i = 0; i < r->count; i++) {
        r->Ord10106();
        switch (r->Ord10009()) {
        case 6: case 9: case 10: r->Ord10094(-2); break;
        case 7: r->Ord10094(-7); break;
        default: break;
        }
    }
    if (r->dedc) r->Ord10106();
    if (r->mode == 1 && r->gateEB8) return;
    for (; e; e = *(int32_t*)(intptr_t)(e + 0x34c), idx++) {
        int32_t ctl = *(int32_t*)(intptr_t)(e + 0x338);
        if (idx < r->scroll || r->scroll + r->count <= idx) { r->Ord10194(ctl, 0); continue; }
        uint16_t fl = *(uint16_t*)(intptr_t)(e + 0x324);
        r->Ord10140(ctl, y + 0x1e);
        r->Ord10179(ctl, 0);
        uint32_t f = (fl & 8) ? (r->mask10 | 1) : (r->invMask10 & 1);
        f = (fl & 4) ? (f | r->mask14) : (f & r->invMask14);
        r->Ord10194(ctl, f);
        r->row_text(e, idx - r->scroll, x);
        if (y == 0x25) y = 0x135; else { y = 0x25; x += 0x5d; }
    }
}
