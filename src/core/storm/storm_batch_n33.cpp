// src/core/storm/storm_batch_n33.cpp -- nine 1.13c Storm.dll rows transcribed from the cached
// decompiles (build-wasm/ghidra-storm/decomp/0x<RVA>.c): SNet provider bind (0x16E70), the SDlg
// registry lookup / region walk / dialog builder / clipped blit (0x1CD30 0x1D2E0 0x1DF90 0x1E830),
// the SFile handle position query (0x266C0), the bignum decimal conversions (0x2D4E0 0x2D6F0) and
// the keyed-record removal (0x35CD0). Stage A batch lane: implementation + marker, no spec, no vector.
//
// Retail's 32-bit pointers become native pointers (own structs, retail offsets in comments); every
// Win32 / loader / sibling-row call goes through g_n33 with a success-valued default, so the control
// flow, the arithmetic and the error values (g_n33Err, mirrored to SetLastError) are retail's.
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <strings.h>

namespace {

typedef uintptr_t H;  // HWND / HRGN / HFONT / HINSTANCE

struct N33Host {
    // kernel32 / user32 / gdi32 seams
    void (*setLastError)(uint32_t) = [](uint32_t) {};
    H (*getParent)(H) = [](H) -> H { return 0; };
    void (*getClassName)(H, char* b, int n) = [](H, char* b, int) { b[0] = 0; };
    intptr_t (*getWindowLong)(H, int) = [](H, int) -> intptr_t { return 0; };
    intptr_t (*setWindowLong)(H, int, intptr_t) = [](H, int, intptr_t) -> intptr_t { return 0; };
    H (*getProp)(H, const char*) = [](H, const char*) -> H { return 0; };
    void (*setProp)(H, const char*, H) = [](H, const char*, H) {};
    bool (*isWindowVisible)(H) = [](H) { return false; };
    H (*getTopWindow)(H) = [](H) -> H { return 0; };
    H (*getWindow)(H, unsigned) = [](H, unsigned) -> H { return 0; };
    void (*getWindowRect)(H, int r[4]) = [](H, int r[4]) { r[0] = r[1] = r[2] = r[3] = 0; };
    void (*screenToClient)(H, int pt[2]) = [](H, int[2]) {};
    bool (*rectInRegion)(H, const int r[4]) = [](H, const int[4]) { return false; };
    H (*createRectRgn)(int, int, int, int) = [](int, int, int, int) -> H { return 1; };
    void (*combineRgn)(H, H, H, int) = [](H, H, H, int) {};
    void (*deleteObject)(H) = [](H) {};
    // dialog builder seams
    int (*mulDiv)(int a, int b, int c) = [](int a, int b, int c) { return (int)((int64_t)a * b / c); };
    H (*createFont)(int h, int weight, unsigned italic, unsigned underline, unsigned strike, unsigned charset,
                    unsigned pitchFamily, const char* face) =
        [](int, int, unsigned, unsigned, unsigned, unsigned, unsigned, const char*) -> H { return 0; };
    bool (*isTrueTypeFont)(H hwnd, H font) = [](H, H) { return true; };       // TMPF_TRUETYPE of the font
    void (*fontUnits)(H font, int* bx, int* by) = [](H, int*, int*) {};       // Storm+0x1CE50
    void (*selectFont)(H hwnd, H font) = [](H, H) {};
    void (*adjustWindowRectEx)(int r[4], uint32_t style, uint32_t ex) = [](int*, uint32_t, uint32_t) {};
    int (*getSystemMetrics)(int i) = [](int i) { return i ? 480 : 640; };
    bool (*isIconic)(H) = [](H) { return false; };
    void (*showWindow)(H, int) = [](H, int) {};
    void (*setActiveWindow)(H) = [](H) {};
    void (*setFocus)(H) = [](H) {};
    bool (*pumpMessage)() = []() { return false; };                           // Storm+0x1D600
    H (*createWindowEx)(uint32_t ex, const char* cls, const char* title, uint32_t style, int x, int y, int w,
                        int h, H parent, H menu, H inst, const void* param) =
        [](uint32_t, const char*, const char*, uint32_t, int, int, int, int, H, H, H, const void*) -> H {
        static H n = 0x1000;
        return ++n;
    };
    intptr_t (*sendMessage)(H, unsigned, uintptr_t, intptr_t) = [](H, unsigned, uintptr_t, intptr_t) -> intptr_t { return 1; };
    // 0x1E830 seams
    void (*getCursorPos)(int pt[2]) = [](int pt[2]) { pt[0] = pt[1] = 0; };
    int (*showCursor)(bool) = [](bool) { return -1; };
    void (*gdiFlush)() = []() {};
    // region rects, 4 ints each (GetRegionData); returns count, fills up to cap
    unsigned (*regionRects)(H rgn, int* rects, unsigned cap) = [](H, int*, unsigned) -> unsigned { return 0; };
    void (*blitClip)(uintptr_t bits, const int* rect, const int* z, uintptr_t pitch, uintptr_t a, const int* dst,
                     uintptr_t b, uintptr_t c, uint32_t d, uint32_t e) =
        [](uintptr_t, const int*, const int*, uintptr_t, uintptr_t, const int*, uintptr_t, uintptr_t, uint32_t, uint32_t) {};  // Storm+0x285C0
    void (*blitPlain)(uintptr_t bits, const int* rect, uintptr_t pitch, uintptr_t a, const int* edi, uintptr_t b,
                      int x, int y, uint32_t d, uint32_t e) =
        [](uintptr_t, const int*, uintptr_t, uintptr_t, const int*, uintptr_t, int, int, uint32_t, uint32_t) {};  // Storm+0x28470
    void (*lockSurface)(uintptr_t surf, uintptr_t* bits, uintptr_t* pitch) = [](uintptr_t, uintptr_t* b, uintptr_t* p) { *b = 1; *p = 1; };  // 0x192A0
    void (*unlockSurface)(uintptr_t surf, uintptr_t bits) = [](uintptr_t, uintptr_t) {};                                           // 0x19040
    // SNet provider seams
    void (*h1040)(int) = [](int) {};
    void (*h69a0)() = []() {};
    void (*h1F130)(uint32_t) = [](uint32_t) {};
    void* (*pageAlloc)(uint32_t) = [](uint32_t n) { return calloc(1, n); };   // VirtualAlloc + VirtualLock
    bool (*snpBind)(const char* dll, uint32_t arg, const uint32_t** caps) =  // LoadLibraryA + SnpBind
        [](const char*, uint32_t, const uint32_t** c) { *c = nullptr; return false; };
    int (*snpInit)(const uint32_t* caps, uintptr_t a, uintptr_t b, uintptr_t c, uintptr_t d, uintptr_t e) =  // caps[7]
        [](const uint32_t*, uintptr_t, uintptr_t, uintptr_t, uintptr_t, uintptr_t) { return 1; };
    // SFile handle seam
    void (*refreshPos)(void* node) = [](void*) {};                            // Storm+0x25DC0
} g_n33;

uint32_t g_n33Err;  // 0x6FC45388
void setErr(uint32_t e) { g_n33Err = e; g_n33.setLastError(e); }

// ---- SDlg globals -------------------------------------------------------------------------------
struct SDlgReg { SDlgReg* next; H hwnd; H parent; uint32_t mask, style; char name[0xc4]; };  // retail +4/+8/+0xc/+0x10/+0x14/+0x3c
SDlgReg* g_sdlgList;       // 0x6FC45A08
H g_sdlgMain;              // 0x6FC453D8
struct SDlgFont { int32_t size, weight, f8, pitch; char face[32]; }* g_sdlgFontOv;  // 0x6FC453A8
int g_sdlgScreenOv;        // 0x6FC453E4
int g_sdlgW, g_sdlgH;      // 0x6FC41A20 / 0x6FC41A24
intptr_t g_sdlgItemProc;   // 0x6FC0DC40 item window procedure

uint16_t u16(const uint8_t* p) { uint16_t v; memcpy(&v, p, 2); return v; }
int16_t s16(const uint8_t* p) { return (int16_t)u16(p); }
uint32_t u32(const uint8_t* p) { uint32_t v; memcpy(&v, p, 4); return v; }
unsigned wlen(const uint8_t* p) { unsigned n = 0; while (u16(p + 2 * n)) n++; return n; }  // Storm+0x6868
void narrow(char* d, const uint8_t* w, unsigned n) { unsigned i = 0; for (; i < n && u16(w + 2 * i); i++) d[i] = (char)u16(w + 2 * i); d[i] = 0; }  // Storm+0x6A28

// ---- bignum helpers (retail vector {cap,size,data,grow,off}) ----------------------------------
struct BigVec { uint32_t cap, size; uint32_t* data; uint32_t grow, off; };
void bigReserve(BigVec* v, uint32_t n) { v->data = (uint32_t*)realloc(v->data, n * 4); v->cap = n; }   // Storm+0x3C80
void bigSize(BigVec* v, uint32_t n) {                                                                   // the 0x2D6F0 / 0x36D0 grow-then-set
    if (v->size < n && v->cap < n) {
        uint32_t g = v->grow;
        if (!g) { if (n < 0x40) { g = 1; for (uint32_t t = n; t; t &= t - 1) g = t; } else v->grow = g = 0x40; }  // Storm+0x3C40
        bigReserve(v, n % g ? n + g - n % g : n);
    }
    v->size = n;
}
void bigTrim(BigVec* v) { while (v->size != v->off && v->data[v->size - 1] == 0) v->size--; }           // Storm+0x36D0
uint32_t bigDivSmall(BigVec* v, uint32_t d) {                                                           // Storm+0x2CD30 in place
    uint64_t r = 0;
    for (uint32_t i = v->size; i > v->off;) { --i; uint64_t a = (r << 32) | v->data[i]; v->data[i] = (uint32_t)(a / d); r = a % d; }
    bigTrim(v);
    return (uint32_t)r;
}
void bigMulSmall(BigVec* v, uint32_t m) {                                                               // Storm+0x2CBD0 in place
    uint64_t c = 0;
    for (uint32_t i = v->off; i < v->size; i++) { uint64_t p = (uint64_t)v->data[i] * m + c; v->data[i] = (uint32_t)p; c = p >> 32; }
    if (c) { uint32_t n = v->size; bigSize(v, n + 1); v->data[n] = (uint32_t)c; }
}
void bigAddSmall(BigVec* v, uint32_t a) {                                                               // Storm+0x2D1A0 in place
    uint64_t c = a;
    for (uint32_t i = v->off; c; i++) {
        if (i >= v->size) { bigSize(v, i + 1); v->data[i] = 0; }
        c += v->data[i]; v->data[i] = (uint32_t)c; c >>= 32;
    }
}

struct StrBuilder { uint32_t cap, size; char* data; uint32_t grow; };  // 0x2D4E0 param_1
void sbPush(StrBuilder* s, char c) {
    uint32_t n = s->size + 1;
    if (s->cap < n) {
        if (!s->grow) { if (n < 0x100) { /* retail computes a step and discards it */ } else s->grow = 0x100; }
        s->cap = n;                                                                                      // Storm+0x3D40
        s->data = (char*)realloc(s->data, n);
    }
    s->data[s->size++] = c;
}

// ---- keyed-record table (0x35CD0) -----------------------------------------------------------------
struct HItem { uint32_t key, pad, val; HItem* next; };   // +0 +8 +0xC
struct HSub { HItem** buckets; uint32_t n, count; };      // +0 +4 +8
struct HEnt { uint32_t k1, k2, pad; HSub* sub; HEnt* next; };  // +0 +4 +0xC +0x10
HEnt** g_keyTab;           // 0x6FC45370
uint32_t g_keyTabN;        // 0x6FC45374
uint32_t g_keyDirty;       // 0x6FC42F58

// ---- SNet provider list (0x16E70) -----------------------------------------------------------------
struct SnpProvider { SnpProvider* next; char name[0x104]; uint32_t bindArg, id, flags, latency; };  // +4 +8 +0x10C +0x110 +0x218 +0x22C
SnpProvider* g_snpList;    // 0x6FC45A50
SnpProvider* g_snpCur;     // 0x6FC45840
uint32_t g_snpCaps[23];    // 0x6FC45834 (0x5C bytes)
bool g_snpCapsOk;
void* g_snpPage;           // 0x6FC45848
uint32_t g_snpT[5];        // 0x6FC41B38.. timeouts

// ---- SFile handle list (0x266C0) -----------------------------------------------------------------
struct FileRec { FileRec* next; uint32_t id, state, size, pos; };  // +4 +8 +0x18 +0x24 +0x2C
FileRec* g_fileList;       // 0x6FC459CC

}  // namespace

// 1.13c Storm+0x16E70
// EAX = provider id; args 1..5 are forwarded to the provider's init slot (caps[7]). 1 on success.
extern "C" uint32_t Storm_N33_16E70(uint32_t id, uintptr_t p1, uintptr_t p2, uintptr_t p3, uintptr_t p4, uintptr_t p5) {
    g_n33.h1040(0);
    g_n33.h69a0();
    for (g_snpCur = g_snpList; g_snpCur; g_snpCur = g_snpCur->next) {
        if (g_snpCur->id != id) continue;
        uint32_t u = g_snpCur->latency;
        g_snpT[0] = u * 5 > 0xf9 ? u * 5 : 0xfa;
        g_snpT[1] = u * 12 > 4999 ? u * 12 : 5000;
        g_snpT[2] = u * 4 > 999 ? u * 4 : 1000;
        g_snpT[3] = (u >> 1) > 0x18 ? u >> 1 : 0x19;
        g_snpT[4] = u > 0x31 ? u : 0x32;
        if (g_snpCur->flags & 1) {
            g_snpPage = g_n33.pageAlloc(0x2000);
            if (!g_snpPage) { g_n33.h1040(0); setErr(8); return 0; }
        }
        const uint32_t* caps = nullptr;
        if (g_n33.snpBind(g_snpCur->name, g_snpCur->bindArg, &caps) && caps && caps[0] > 0x4f) {
            memset(g_snpCaps, 0, sizeof g_snpCaps);
            uint32_t n = caps[0] > 0x5b ? 0x5c : caps[0];
            memcpy(g_snpCaps, caps, n);
            g_snpCapsOk = true;
            int ok = g_n33.snpInit(g_snpCaps, p1, p2, p3, p4, p5);
            uint32_t e = g_n33Err;
            if (ok) return 1;
            g_n33.h1040(0);
            g_n33.h1F130(e);
            return 0;
        }
        break;
    }
    g_snpCur = nullptr;
    g_n33.h1040(0);
    setErr(0x4b4);
    return 0;
}

// 1.13c Storm+0x1CD30
// EAX = hwnd, param = registry mask. Returns the registry node or null.
extern "C" void* Storm_N33_1CD30(H hwnd, uint32_t mask) {
    for (SDlgReg* n = g_sdlgList; n; n = n->next)
        if (n->hwnd == hwnd && (n->mask & mask)) return n;
    H parent = g_n33.getParent(hwnd);
    char cls[0x100]; cls[0] = 0;
    g_n33.getClassName(hwnd, cls, 0x100);
    const char* c = cls;
    if (strncasecmp(cls, "SDlg", 4) == 0) c = cls + 4;
    uintptr_t st = (uintptr_t)g_n33.getWindowLong(hwnd, -16);
    if (g_n33.getProp(hwnd, "SDlg_OrigStyle")) st = (uintptr_t)g_n33.getProp(hwnd, "SDlg_OrigStyle");
    uint32_t w = (uint32_t)st;
    for (SDlgReg* n = g_sdlgList; n; n = n->next) {
        if (n->hwnd == 0 && (n->parent == 0 || n->parent == parent) && (n->mask & mask) &&
            (~n->style & w & 0xf) == 0 && ((n->style & 0x10000) || (n->style & w) == n->style) &&
            strcasecmp(n->name, c) == 0)
            return n;
    }
    return nullptr;
}

// 1.13c Storm+0x1D2E0
// Subtracts every visible, non-transparent descendant rect of `wnd` from `rgn` (client coords of `base`).
extern "C" void Storm_N33_1D2E0(H base, H rgn, H wnd) {
    if (!base || !rgn || !wnd) { setErr(0x57); return; }
    if (!g_n33.isWindowVisible(wnd)) return;
    uint32_t ex = (uint32_t)g_n33.getWindowLong(wnd, -20);
    if (wnd != base && !(ex & 0x20)) {
        int r[4];
        g_n33.getWindowRect(wnd, r);
        g_n33.screenToClient(base, r);
        g_n33.screenToClient(base, r + 2);
        if (g_n33.rectInRegion(rgn, r)) {
            H s = g_n33.createRectRgn(r[0], r[1], r[2], r[3]);
            g_n33.combineRgn(rgn, rgn, s, 4);  // RGN_DIFF
            g_n33.deleteObject(s);
        }
    }
    for (H c = g_n33.getTopWindow(wnd); c; c = g_n33.getWindow(c, 2)) Storm_N33_1D2E0(base, rgn, c);
}

// 1.13c Storm+0x1DF90
// SDlgCreateDialogIndirect worker: DLGTEMPLATE(EX) -> "SDlgDialog" window plus child controls. Returns the dialog hwnd or 0.
extern "C" H Storm_N33_1DF90(H hinst, const uint8_t* t, H parent, void* param4, intptr_t param5) {
    H mainWnd = g_sdlgMain;
    if (!parent) parent = mainWnd;
    bool ex = (int32_t)u32(t) == -0xffff;
    uint32_t style, exstyle, size = 0, weight = 0, italic = 0;
    unsigned cdit;
    int x, y, cx, cy;
    const uint8_t *title, *face = nullptr, *items;
    const uint8_t* m;  // menu
    if (ex) {
        exstyle = u32(t + 8); style = u32(t + 0xc); cdit = u16(t + 0x10);
        x = s16(t + 0x12); y = s16(t + 0x14); cx = s16(t + 0x16); cy = s16(t + 0x18);
        m = t + 0x1a;
    } else {
        style = u32(t); exstyle = u32(t + 4); cdit = u16(t + 8);
        x = s16(t + 10); y = s16(t + 12); cx = s16(t + 14); cy = s16(t + 16);
        m = t + 0x12;
    }
    const uint8_t* cl = (u16(m) == 0 || u16(m) == 0xffff) ? m + 2 : m + (wlen(m) + 1) * 2;
    title = (u16(cl) == 0 || u16(cl) == 0xffff) ? cl + 2 : cl + (wlen(cl) + 1) * 2;
    if (style & 0x40) {
        const uint8_t* f = title + (wlen(title) + 1) * 2;
        size = u16(f);
        if (ex) { weight = u16(f + 2); italic = f[4] & 1; face = f + 6; }
        else face = f + 2;
        items = face + (wlen(face) + 1) * 2;
    } else items = title + (wlen(title) + 1) * 2;

    char ttl[256], fnt[256], cls[256], txt[256];
    ttl[0] = fnt[0] = 0;
    if (u16(title) && u16(title) != 0xffff) narrow(ttl, title, 0xff);
    if (face && u16(face) && u16(face) != 0xffff) narrow(fnt, face, 0xff);
    H font = 0;
    if (fnt[0]) {
        const SDlgFont* ov = g_sdlgFontOv;
        int sz = ov ? ov->size : (int16_t)size, wt = ov ? ov->weight : (int16_t)weight;
        unsigned it = ov ? 0 : (uint16_t)italic, pitch = ov ? (unsigned)ov->pitch : 0;
        font = g_n33.createFont(-g_n33.mulDiv(sz, 0x60, 0x48), wt, it, 0, 0, 1, pitch, ov ? ov->face : fnt);
        if (font && !g_n33.isTrueTypeFont(parent, font)) {
            g_n33.deleteObject(font);
            font = g_n33.createFont(-g_n33.mulDiv(sz, 0x60, 0x48), wt, ov ? 0 : it, 0, 0, 1, 0x20, "Arial");
        }
    }
    int bx = 8, by = 0x10;
    if (font) g_n33.fontUnits(font, &bx, &by);
    uint32_t ws = (((style & 0x80) | 0x800000) >> 7) | exstyle;
    int l = x * bx / 4, t0 = y * by / 8;
    int rc[4] = {l, t0, cx * bx / 4 - 1 + l, cy * by / 8 - 1 + t0};
    g_n33.adjustWindowRectEx(rc, style & 0xefff0000, ws);
    int sw = g_sdlgScreenOv ? g_sdlgW : g_n33.getSystemMetrics(0), sh = g_sdlgScreenOv ? g_sdlgH : g_n33.getSystemMetrics(1);
    int w = rc[2] - rc[0] + 1, h = rc[3] - rc[1] + 1;
    int px = l, py = t0;
    if (style & 0x800) { px = (sw - w) >> 1; py = (sh - h) >> 1; }
    int mx = sw - w, my = sh - h;
    int fx = (px < mx ? px : mx) < 0 ? 0 : (px < mx ? px : mx);
    int fy = (py < my ? py : my) < 0 ? 0 : (py < my ? py : my);
    if (mainWnd && g_n33.isIconic(mainWnd)) {
        g_n33.showWindow(mainWnd, 3);
        while (g_n33.pumpMessage()) {}
        g_n33.setActiveWindow(parent);
        g_n33.setFocus(parent);
    }
    H dlg = g_n33.createWindowEx(ws, "SDlgDialog", ttl, style & 0xefffffff, fx, fy, w, h, parent, 0, hinst, param4);
    if (!dlg) return 0;
    g_n33.selectFont(dlg, font);
    g_n33.setProp(dlg, "SDlg_EndDialog", 0);
    g_n33.setProp(dlg, "SDlg_EndResult", 0);
    H focus = dlg;
    const uint8_t* p = items;
    for (unsigned i = 0; i < cdit; i++) {
        unsigned pad = (unsigned)((p - t) & 3);
        if (pad) p += 4 - pad;
        if (ex) p += 4;
        const uint8_t* ic = p + (ex ? 0x14 : 0x12);
        const uint8_t* it;
        if (u16(ic) == 0xffff) it = ic + 4; else it = ic + (wlen(ic) + 1) * 2;
        const uint8_t* cd;  // creation data
        if (u16(it) == 0xffff) cd = it + 6; else cd = it + (wlen(it) + 2) * 2;
        cls[0] = txt[0] = 0;
        if (u16(ic) == 0xffff) {
            const char* n = nullptr;
            switch (u16(ic + 2)) {
                case 0x80: n = "Button"; break;
                case 0x81: n = "Edit"; break;  // DAT_6fc37520
                case 0x82: n = "SDlgStatic"; break;
                case 0x83: n = "Listbox"; break;
                case 0x84: n = "Scrollbar"; break;
                case 0x85: n = "Combobox"; break;
            }
            if (n) { strncpy(cls, n, 0xff); cls[0xff] = 0; }
        } else narrow(cls, ic, 0xff);
        if (u16(it) != 0xffff) narrow(txt, it, 0xff);
        uint32_t istyle = ex ? u32(p + 4) : u32(p), iex = ex ? u32(p) : u32(p + 4);
        int ix = s16(p + 8), iy = s16(p + 10), icx = s16(p + 12), icy = s16(p + 14);
        H c = g_n33.createWindowEx(iex, cls, txt, istyle, ix * bx / 4, iy * by / 8, icx * bx / 4, icy * by / 8, dlg,
                                   (H)u16(p + 16), hinst, cd);
        H orig = (H)g_n33.getWindowLong(c, -4);
        g_n33.setProp(c, "SDlg_WndProc", orig);
        g_n33.setWindowLong(c, -4, g_sdlgItemProc);
        if (c && font) g_n33.sendMessage(c, 0x30, font, 0);
        g_n33.showWindow(c, 5);
        if (c && focus == dlg && !(u32(p) & 0x8000000) && strcasecmp(cls, "Static") && strcasecmp(cls, "SDlgStatic")) focus = c;
        p = cd + u16(cd - 2);
    }
    if (font) g_n33.sendMessage(dlg, 0x30, font, 0);
    if (!g_n33.sendMessage(dlg, 0x110, focus, param5)) focus = 0;
    for (H c = g_n33.getTopWindow(dlg); c; c = g_n33.getWindow(c, 2)) {
        if (!Storm_N33_1CD30(c, 0xffff)) continue;
        intptr_t s = g_n33.getWindowLong(c, -16);
        g_n33.setProp(c, "SDlg_OrigStyle", (H)s);
        cls[0] = 0;
        g_n33.getClassName(c, cls, 0x100);
        intptr_t ns;
        if (!strcasecmp(cls, "Button") && (((uint32_t)s & 0xf) == 0 || ((uint32_t)s & 0xf) == 1)) ns = (intptr_t)((uint32_t)s | 0xb);
        else if (!strcasecmp(cls, "ComboBox") || !strcasecmp(cls, "ListBox")) ns = (intptr_t)((uint32_t)s | 0x10);
        else continue;
        if (ns != s) g_n33.setWindowLong(c, -16, ns);
    }
    if (!g_n33.getProp(dlg, "SDlg_EndDialog")) {
        g_n33.showWindow(dlg, 1);
        if (focus) g_n33.setFocus(focus);
    }
    return dlg;
}

// 1.13c Storm+0x1E830
// Region-clipped SDLG blit. `edi` is retail's implicit EDI (4-int source rect). Returns 1; *hidden += cursor hides.
extern "C" uint32_t Storm_N33_1E830(uintptr_t bits, const int* clip, uintptr_t pitch, uintptr_t a, uintptr_t b, uintptr_t c,
                                    int dx, int dy, uint32_t d, uint32_t e, const int* org, H rgn, uint32_t flags,
                                    uintptr_t surf, int* hidden, const int* edi) {
    static int buf[4 * 4096];
    unsigned n = g_n33.regionRects(rgn, buf, 4096);
    if (n == 0) return 1;
    for (unsigned i = 0; i < n; i++) {
        int* r = buf + 4 * i;
        int v;
        v = org[0] + r[0]; r[0] = v < clip[0] ? clip[0] : v;
        v = org[1] + r[1]; r[1] = v < clip[1] ? clip[1] : v;
        v = org[0] + r[2]; r[2] = clip[2] < v ? clip[2] : v;
        v = org[1] + r[3]; r[3] = clip[3] < v ? clip[3] : v;
    }
    unsigned hid = 0;
    if (flags & 0x1000) {
        int pt[2];
        g_n33.getCursorPos(pt);
        for (unsigned i = 0; i < n; i++) {
            const int* r = buf + 4 * i;
            if (pt[0] + 0x20 < r[0] || pt[1] + 0x20 < r[1] || r[2] < pt[0] - 0x20 || r[3] < pt[1] - 0x20) continue;
            do { hid++; } while (g_n33.showCursor(false) >= 0);
            break;
        }
        g_n33.gdiFlush();
    }
    uint32_t lock = flags & 2;
    if (lock) g_n33.lockSurface(surf, &bits, &pitch);
    if (bits && pitch) {
        for (unsigned i = 0; i < n; i++) {
            const int* r = buf + 4 * i;
            if (!(flags & 1)) {
                int dst[4] = {edi[0] + (r[0] - clip[0]) + dx, edi[1] + (r[1] - clip[1]) + dy, edi[2], edi[3]};
                g_n33.blitClip(bits, r, nullptr, pitch, a, dst, b, c, d, e);
            } else {
                g_n33.blitPlain(bits, r, pitch, a, edi, c, (r[0] - org[0]) + dx, (r[1] - org[1]) + dy, d, e);
            }
        }
    }
    if (lock) g_n33.unlockSurface(surf, bits);
    unsigned left = hid;
    if (flags & 0x2000) { for (; left; left--) g_n33.showCursor(true); hid = 0; }
    if (hidden) *hidden += (int)hid;
    return 1;
}

// Storm 1.13c +0x266C0 (self-contained twin; the source marker lives in storm_batch_26e20.cpp)
// Handle position query: out1 = play position, out2 = size; returns 1 if the handle record exists and is not state 4.
extern "C" uint32_t Storm_N33_266C0(uint32_t id, uint32_t* out1, uint32_t* out2) {
    uint32_t ret = 0, pos = 0, size = 0;
    bool miss = true;
    for (FileRec* n = g_fileList; n; n = n->next) {
        if (n->id != id) continue;
        size = n->size;
        if (n->state != 4) {
            ret = 1;
            if (n->state != 0 && n->state != 1) { g_n33.refreshPos(n); pos = n->pos; }
            miss = false;
        }
        break;
    }
    if (miss) setErr(0x85100072);
    if (out1) *out1 = pos;
    if (out2) *out2 = size;
    return ret;
}

// 1.13c Storm+0x2D4E0
// EAX = bignum to print, out = string builder, stack = scratch bignum stack (depth counter in `depth`).
extern "C" void Storm_N33_2D4E0(const BigVec* src, StrBuilder* out, BigVec* stack, uint32_t* depth) {
    BigVec* w = &stack[(*depth)++];
    if (w != src) {
        w->size = 0;
        bigSize(w, src->size);
        memcpy(w->data, src->data, src->size * 4);
    }
    w->grow = src->grow;
    w->off = src->off;
    out->size = 0;
    do {
        char d = (char)(bigDivSmall(w, 10) + '0');
        sbPush(out, d);
        bigTrim(w);
    } while (w->size - w->off > 1 || (w->off < w->size && w->data[w->off] != 0));
    for (uint32_t i = 0; i < out->size / 2; i++) { char t = out->data[i]; out->data[i] = out->data[out->size - 1 - i]; out->data[out->size - 1 - i] = t; }
    sbPush(out, 0);
    (*depth)--;
}

// 1.13c Storm+0x2D6F0
// EAX = decimal string, ECX = destination bignum (cleared to its offset, then v = v*10 + digit per char).
extern "C" void Storm_N33_2D6F0(const char* s, BigVec* v) {
    bigSize(v, v->off);
    v->size = v->off;
    for (; *s; s++) {
        bigMulSmall(v, 10);
        bigAddSmall(v, (uint32_t)(*s - '0'));
    }
}

// 1.13c Storm+0x35CD0
// Removes the (k3, k4) records from the (k1, k2) table; 1 if anything was removed.
extern "C" uint32_t Storm_N33_35CD0(uint32_t k1, uint32_t k2, uint32_t k3, uint32_t k4) {
    uint32_t ret = 0;
    if (!g_keyTab || !g_keyTabN) return 0;
    HEnt* e = g_keyTab[(k1 ^ k2) & (g_keyTabN - 1)];
    if (!e) return 0;
    while (e->k1 != k1 || e->k2 != k2) { e = e->next; if (!e) return 0; }
    HSub* sub = e->sub;
    if (sub->buckets && sub->n) {
        HItem** pp = &sub->buckets[(sub->n - 1) & k3];
        while (*pp) {
            HItem* it = *pp;
            if (it->key == k3 && (k4 == 0 || it->val == k4)) {
                *pp = it->next;
                free(it);  // Storm+0x1050 SMemFree
                g_keyDirty = 1;
                ret = 1;
                sub->count--;
            } else pp = &it->next;
        }
    }
    return ret;
}
