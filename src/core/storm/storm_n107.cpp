// src/core/storm/storm_n107.cpp -- Storm 1.13c rows 0x16E70 0x1CD30 0x1DF90 0x1E830 0x21CB0 0x21E40
// (0x35CD0 already lives in storm_batch_19040.cpp; 0x09BA1 has no cached decompile and is skipped).
// Transcribed from build-wasm/ghidra-storm/decomp/0x<RVA>.c. Retail structs hold 32-bit addresses, so
// fields are uint32_t cast through uintptr_t (storm_n95.cpp convention). Win32 has no wasm analogue:
// every API call is a function-pointer seam in a per-function Env, same call order as retail.
// Register arguments Ghidra shows as in_EAX/unaff_EBX/extraout_EDX are explicit parameters.
#include <cstdint>
#include <cstring>
#include <strings.h>

namespace {
typedef uint32_t u32;
inline u32* P(u32 a) { return (u32*)(uintptr_t)a; }
inline u32 A(const void* p) { return (u32)(uintptr_t)p; }
inline u32* W(void* b, u32 o) { return (u32*)((uint8_t*)b + o); }

// ---- intrusive list used by the Huffman tree (0x4810 owns the type; links are {next, prev}) ----
// n[1] < 0 : prev is the head slot itself (~n[1]); else prev slot = n + (n[1] - next[1]).
void llUnlink(u32* n) {
    u32 nx = n[0];
    if (nx == 0) return;
    int32_t pv = (int32_t)n[1];
    u32* slot = pv < 0 ? P(~n[1]) : P(A(n) + (n[1] - P(nx)[1]));
    *slot = nx;
    P(n[0])[1] = n[1];
    n[0] = 0; n[1] = 0;
}
// link n at the tail of the list whose head slot is st+head, tail slot st+tail, base slot st+base
void llAppend(uint8_t* st, u32* n, u32 head, u32 tail, u32 base) {
    u32* h = W(st, head);
    n[0] = A(h);
    n[1] = *W(st, tail);
    u32 t = *W(st, tail);
    u32 b = *W(st, base);
    u32* slot;
    if ((int32_t)t < 0) slot = P(~t);
    else {
        if ((int32_t)b < 0) b = A(h) - P(*h)[1];
        slot = P(t + b);
    }
    *slot = A(n);
    *W(st, tail) = A(n);
}
// take a node: free-list head (st+0x3058) if usable, else the next of the 0x18-byte pool at st+8
u32* poolTake(uint8_t* st) {
    u32 f = *W(st, 0x3058);
    if ((int32_t)f < 1) {
        u32* n = (u32*)(st + 8 + *W(st, 0x3068) * 0x18);
        (*W(st, 0x3068))++;
        return n;
    }
    return P(f);
}
// insert n before `at` (or at the list head slot when at is null): *n=*at; n[1]=(*at)[1]; (*at)[1]=n; *at=n
void llInsert(uint8_t* st, u32* n, u32* at) {
    if (at == nullptr) at = W(st, 0x3060);
    u32 v = *at;
    n[0] = v;
    n[1] = P(v)[1];
    P(v)[1] = A(n);
    *at = A(n);
}
}  // namespace

// 1.13c STORM+0x21CB0 -- __thiscall: add a leaf node for symbol param_1 under the tail
extern "C" void Storm_N107_21CB0(void* self, int32_t param_1, void (*sub4810)(void* self)) {
    uint8_t* st = (uint8_t*)self;
    u32 t = *W(st, 0x3064);
    u32 parent = ((int32_t)t < 1 ? 0u : 0xffffffffu) ^ 0xffffffffu;  // ((t<1)-1)&t
    parent &= t;
    u32* n1 = poolTake(st);
    llUnlink(n1);
    llAppend(st, n1, 0x3060, 0x3064, 0x305c);
    *W(st, 0x3064) = A(n1);
    n1[4] = 0; n1[5] = 0;
    n1[2] = P(parent)[2];
    n1[3] = P(parent)[3];
    n1[4] = parent;
    *W(st, 0x306c + n1[2] * 4) = A(n1);

    u32* n2 = poolTake(st);
    llUnlink(n2);
    llAppend(st, n2, 0x3060, 0x3064, 0x305c);
    *W(st, 0x3064) = A(n2);
    n2[5] = 0; n2[3] = 0;
    n2[4] = parent;
    n2[2] = (u32)param_1;
    *W(st, 0x306c + (u32)param_1 * 4) = A(n2);
    P(parent)[5] = A(n2);
    sub4810(self);
}

// 1.13c STORM+0x21E40 -- build the Huffman tree from a frequency table
// st = EBX (decoder state), tableSet = byte (param_1), freq = DAT_6fc35cf8 (0x102 bytes per set).
extern "C" void Storm_N107_21E40(uint8_t* st, uint8_t tableSet, const uint8_t* freq) {
    for (;;) {  // move the old nodes onto the list at 0x3054
        u32 n = *W(st, 0x3064);
        if ((int32_t)n < 1) break;
        u32* p = P(n);
        llUnlink(p);
        llAppend(st, p, 0x3054, 0x3058, 0x3050);
    }
    u32* slot = W(st, 0x306c);
    std::memset(slot, 0, 0x102 * 4);
    u32 last = 0;
    int32_t i = 0;
    do {
        u32 w = last;
        uint8_t f = freq[i + (u32)tableSet * 0x102];
        if (f != 0) {
            u32* n = poolTake(st);
            u32* h = n ? n : W(st, 0x3060);
            llUnlink(h);
            llInsert(st, h, nullptr);          // insert at head, (*head)=h
            n[4] = 0; n[5] = 0;
            *slot = A(n);
            n[2] = (u32)i;
            n[3] = f;
            w = f;
            if (f < last) {                    // keep ascending: find first node with weight >= f
                u32 q = *W(st, 0x3064);
                u32* at;
                if ((int32_t)q < 1) at = nullptr;
                else {
                    at = P(q);
                    for (;;) {
                        if (f <= at[3]) break;
                        if ((int32_t)at[1] < 1) { at = nullptr; break; }
                        at = P(at[1]);
                    }
                }
                llUnlink(n);
                llInsert(st, n, at);
                w = last;
            }
        }
        last = w;
        i++; slot++;
    } while (i < 0x100);
    for (; i < 0x102; i++, slot++) {           // the two escape symbols, weight 1, at the tail
        u32* n = poolTake(st);
        u32* h = n ? n : W(st, 0x3060);
        llUnlink(h);
        llAppend(st, h, 0x3060, 0x3064, 0x305c);
        *W(st, 0x3064) = A(n);
        n[4] = 0; n[5] = 0;
        *slot = A(n);
        n[2] = (u32)i;
        n[3] = 1;
    }
    u32 a = *W(st, 0x3064);
    if ((int32_t)a > 0) {                      // merge the two lightest until one root remains
        for (;;) {
            u32 b = P(a)[1];
            if ((int32_t)b < 1) break;
            u32* n = poolTake(st);
            u32* h = n ? n : W(st, 0x3060);
            llUnlink(h);
            llInsert(st, h, nullptr);
            n[5] = 0; n[4] = 0;
            n[3] = P(b)[3] + P(a)[3];
            n[5] = a;
            P(a)[4] = A(n);
            P(b)[4] = A(n);
            u32 wgt = n[3];
            if (wgt < last) {
                u32 q = P(b)[1];
                u32* at;
                if ((int32_t)q < 1) at = nullptr;
                else {
                    at = P(q);
                    for (;;) {
                        if (wgt <= at[3]) break;
                        if ((int32_t)at[1] < 1) { at = nullptr; break; }
                        at = P(at[1]);
                    }
                }
                llUnlink(n);
                llInsert(st, n, at);
                wgt = last;
            }
            last = wgt;
            a = P(b)[1];
            if ((int32_t)a < 1) break;
        }
    }
    *W(st, 4) = 1;
}

// 1.13c STORM+0x1CD30 -- find the SDlg skin record for a window
// Record: +4 next, +8 hwnd, +0xc parent, +0x10 kind mask, +0x14 style, +0x3c class name. head = DAT_6fc45a08.
struct StormN107SkinEnv {
    uint32_t head;
    uint32_t (*getParent)(uint32_t hwnd);
    void (*getClassName)(uint32_t hwnd, char* buf, int n);
    int32_t (*getWindowLong)(uint32_t hwnd, int idx);
    uint32_t (*getProp)(uint32_t hwnd, const char* name);
};
extern "C" uint32_t Storm_N107_1CD30(const StormN107SkinEnv* e, uint32_t hwnd, uint32_t mask) {
    for (u32 r = e->head; (int32_t)r > 0 && r != 0; r = P(r)[1])
        if (P(r)[2] == hwnd && (P(r)[4] & mask) != 0) return r;
    uint32_t parent = e->getParent(hwnd);
    char buf[0x100] = {};
    e->getClassName(hwnd, buf, 0x100);
    const char* cls = buf;
    if (strncasecmp(buf, "SDlg", 4) == 0) cls = buf + 4;
    uint32_t style = (uint32_t)e->getWindowLong(hwnd, -0x10);
    if (e->getProp(hwnd, "SDlg_OrigStyle")) style = e->getProp(hwnd, "SDlg_OrigStyle");
    for (u32 r = e->head; (int32_t)r > 0 && r != 0; r = P(r)[1]) {
        if (P(r)[2] != 0) continue;
        if (!(P(r)[3] == 0 || P(r)[3] == parent)) continue;
        if (!(P(r)[4] & mask)) continue;
        u32 s = P(r)[5];
        if ((~s & style & 0xf) != 0) continue;
        if (!((s & 0x10000) != 0 || (s & style) == s)) continue;
        if (strcasecmp((const char*)(uintptr_t)(r + 0x3c), cls) == 0) return r;
    }
    return 0;
}

// 1.13c STORM+0x16E70 -- bind provider `id` and run its vtable entry +0x1c
// Provider record (list head DAT_6fc45a50): +4 next, +8 dll name, +0x10c bind arg, +0x110 id, +0x218 flags,
// +0x22c base. Tunables DAT_6fc41b38..48 are written to tun[0..4]. extraout_EDX is taken to be the freshly
// filled table (DAT_6fc45834), whose +0x1c slot is the provider's init callback (assumption: Ghidra shows EDX
// as an unnamed leftover; the table copy is the only object that has a callback slot).
struct StormN107BindEnv {
    void (*lock)(int);                                   // 0x1040
    void (*scanProviders)();                             // 0x69A0
    uint32_t listHead;                                   // DAT_6fc45a50
    uint32_t tun[5];                                     // DAT_6fc41b38..48
    uint32_t cur;                                        // DAT_6fc45840
    void* page; uint32_t table;                          // DAT_6fc45848, DAT_6fc45834 (5c bytes)
    void* (*valloc)(uint32_t n); void (*vlock)(void*, uint32_t n);
    void* (*loadLib)(const char* name);                  // LoadLibraryA
    void* (*getProc)(void* lib, const char* name);       // GetProcAddress
    void* (*alloc)(uint32_t n);                          // 0x1C580 SMemAlloc(0x5c, "SNET.CPP", 0x2ce)
    void (*setLastError)(uint32_t);
    void (*raise)(uint32_t err);                         // 0x1F130
    uint32_t lastError;                                  // DAT_6fc45388
    void* lib;                                           // DAT_6fc45838
};
extern "C" uint32_t Storm_N107_16E70(StormN107BindEnv* e, int32_t id, uint32_t a1, uint32_t a2, uint32_t a3,
                                     uint32_t a4, uint32_t a5) {
    e->lock(0);
    e->scanProviders();
    e->cur = e->listHead;
    if ((int32_t)e->listHead >= 1) {
        do {
            uint8_t* p = (uint8_t*)(uintptr_t)e->cur;
            if (*(int32_t*)(p + 0x110) == id) {
                u32 b = *(u32*)(p + 0x22c);
                e->tun[0] = b * 5 > 0xf9 ? b * 5 : 0xfa;
                e->tun[1] = b * 0xc > 4999 ? b * 0xc : 5000;
                e->tun[2] = b * 4 > 999 ? b * 4 : 1000;
                e->tun[3] = (b >> 1) > 0x18 ? (b >> 1) : 0x19;
                e->tun[4] = b > 0x31 ? b : 0x32;
                if (*(p + 0x218) & 1) {
                    e->page = e->valloc(0x2000);
                    if (e->page == nullptr) {
                        e->lock(0);
                        e->lastError = 8;
                        e->setLastError(8);
                        return 0;
                    }
                    e->vlock(e->page, 0x2000);
                }
                e->lib = e->loadLib((const char*)(p + 8));
                void* bind = e->lib ? e->getProc(e->lib, "SnpBind") : nullptr;
                if (bind) {
                    u32* info = nullptr;
                    ((void (*)(uint32_t, u32**))bind)(*(u32*)(p + 0x10c), &info);
                    if (info && *info > 0x4f) {
                        if (e->table == 0) e->table = A(e->alloc(0x5c));
                        std::memset(P(e->table), 0, 0x5c);
                        u32 n = *info > 0x5b ? 0x5c : *info;
                        std::memcpy(P(e->table), info, n);
                        typedef int (*Init)(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);
                        if (((Init)(uintptr_t)P(e->table)[7])(a1, a2, a3, a4, a5) != 0) return 1;
                        uint32_t err = e->lastError;
                        e->lock(0);
                        e->raise(err);
                        return 0;
                    }
                }
                break;
            }
            e->cur = *(u32*)(p + 4);
            if ((int32_t)e->cur < 1) { e->cur = 0; break; }
        } while (e->cur != 0);
    } else e->cur = 0;
    e->lock(0);
    e->lastError = 0x4b4;
    e->setLastError(0x4b4);
    return 0;
}

// 1.13c STORM+0x1E830 -- blit a surface through every rect of a GDI region
struct StormN107BlitEnv {
    uint32_t (*getRegionData)(uintptr_t rgn, uint32_t n, void* buf);   // GetRegionData
    void* (*alloc)(uint32_t n);                                        // 0x1C580 (SDLG.CPP:0x2fd)
    void (*free)(void*);                                               // 0x1B960 (SDLG.CPP:0x306/0x35d)
    void (*getCursorPos)(int32_t* xy);
    int32_t (*showCursor)(int show);
    void (*gdiFlush)();
    void (*lockSurf)(int32_t arg14, void*, int32_t* p1, int32_t* p3, int);       // 0x192A0
    void (*unlockSurf)(int32_t arg14, int32_t p1);                               // 0x19040
    void (*blitA)(int32_t, const void* rect, int*, int32_t, int32_t, int32_t* dst, intptr_t, int32_t, uint32_t, uint32_t);   // 0x385C0
    void (*blitB)(int32_t, const void* rect, int32_t, int32_t, const int32_t* edi, int32_t, int32_t, int32_t, uint32_t, uint32_t); // 0x38470
};
extern "C" uint32_t Storm_N107_1E830(const StormN107BlitEnv* e, int32_t p1, const int32_t* clip, int32_t p3, int32_t p4,
                                     intptr_t p5, int32_t p6, int32_t p7, int32_t p8, uint32_t p9, uint32_t p10,
                                     const int32_t* off, uintptr_t rgn, uint32_t flags, int32_t p14, int32_t* p15,
                                     const int32_t* edi) {
    uint32_t size = e->getRegionData(rgn, 0, nullptr);
    uint8_t* d = (uint8_t*)e->alloc(size);
    e->getRegionData(rgn, size, d);
    uint32_t count = *(uint32_t*)(d + 8);
    if (count == 0 || d == (uint8_t*)(uintptr_t)0xffffffe0) { e->free(d); return 1; }
    int32_t* r = (int32_t*)(d + 0x20);   // RECT[count]
    for (uint32_t i = 0; i < count; i++, r += 4) {
        r[0] = off[0] + r[0] < clip[0] ? clip[0] : off[0] + r[0];
        r[1] = off[1] + r[1] < clip[1] ? clip[1] : off[1] + r[1];
        r[2] = clip[2] < off[0] + r[2] ? clip[2] : off[0] + r[2];
        r[3] = clip[3] < off[1] + r[3] ? clip[3] : off[1] + r[3];
    }
    uint32_t hidden = 0;
    if (flags & 0x1000) {
        int32_t pt[2];
        e->getCursorPos(pt);
        r = (int32_t*)(d + 0x20);
        for (uint32_t i = 0; i < count; i++, r += 4) {
            if (pt[0] + 0x20 < r[0] || pt[1] + 0x20 < r[1] || r[2] < pt[0] - 0x20 || r[3] < pt[1] - 0x20) continue;
            do { hidden++; } while (e->showCursor(0) >= 0);
            break;
        }
        e->gdiFlush();
    }
    uint32_t lock = flags & 2;
    if (lock) e->lockSurf(p14, nullptr, &p1, &p3, 0);
    if (p1 != 0 && p3 != 0 && count != 0) {
        r = (int32_t*)(d + 0x20);
        for (uint32_t i = 0; i < count; i++, r += 4) {
            if ((flags & 1) == 0) {
                int32_t dst[4] = {edi[0] + (r[0] - clip[0]) + p7, edi[1] + (r[1] - clip[1]) + p8, edi[2], edi[3]};
                e->blitA(p1, r, nullptr, p3, p4, dst, p5, p6, p9, p10);
            } else {
                e->blitB(p1, r, p3, p4, edi, p6, (r[0] - off[0]) + p7, (r[1] - off[1]) + p8, p9, p10);
            }
        }
    }
    if (lock) e->unlockSurf(p14, p1);
    uint32_t n = hidden;
    if (flags & 0x2000) for (; n != 0; n--) { e->showCursor(1); hidden = 0; }
    if (p15) *p15 += hidden;
    e->free(d);
    return 1;
}

// 1.13c STORM+0x1DF90 -- create a modeless SDlg dialog from a DLGTEMPLATE(EX)
struct StormN107DlgHdr {   // what 0x1D0B0 (EX) / 0x1D1D0 (plain) parse out of the template header
    uint32_t style, exstyle; uint16_t count; int16_t x, y, cx, cy;
    int16_t pointSize, weight, italic; const uint16_t* title; const uint16_t* face; const uint8_t* items;
};
struct StormN107DlgEnv {
    void (*parse)(const void* tmpl, bool ex, StormN107DlgHdr* out);                 // 0x1D0B0 / 0x1D1D0
    void (*wideToNarrow)(char* dst, const uint16_t* src, int n);                    // 0x6A28
    int (*wcslen)(const uint16_t*);                                                 // 0x6868
    void (*strcpyN)(char* dst, const char* src, int n);                             // 0x3B080
    void (*fontMetrics)(uintptr_t font, int32_t* bx, int32_t* by);                  // 0x1CE50
    bool (*pump)();                                                                 // 0x1D600
    uint32_t (*skin)(uintptr_t hwnd, uint32_t mask);                                // 0x1CD30
    uintptr_t mainWnd; int32_t* globalFont; int32_t haveRes; int32_t resW, resH;    // DAT_6fc453d8/a8/e4/41a20/24
    uintptr_t subclassProc;                                                         // 0x6FC0DC40
    uintptr_t (*createFont)(int h, int w, int esc, int ori, int weight, int italic, int ul, int so, int cs, int op,
                            int cp, int q, int pitch, const char* face);
    int (*mulDiv)(int, int, int);
    uintptr_t (*getDC)(uintptr_t); uintptr_t (*createCompatDC)(uintptr_t); uintptr_t (*selectObject)(uintptr_t, uintptr_t);
    bool (*isTrueType)(uintptr_t dc); void (*deleteDC)(uintptr_t); void (*releaseDC)(uintptr_t, uintptr_t);
    void (*deleteObject)(uintptr_t);
    void (*adjustRect)(int32_t* rect4, uint32_t style, uint32_t exstyle);
    int (*sysMetric)(int); bool (*isIconic)(uintptr_t); void (*showWindow)(uintptr_t, int);
    void (*setActive)(uintptr_t); void (*setFocus)(uintptr_t);
    uintptr_t (*createWindowEx)(uint32_t ex, const char* cls, const char* title, uint32_t st, int x, int y, int w, int h,
                                uintptr_t parent, uintptr_t menu, uintptr_t inst, const void* param);
    void (*setProp)(uintptr_t, const char*, uintptr_t); uintptr_t (*getProp)(uintptr_t, const char*);
    int32_t (*getWindowLong)(uintptr_t, int); void (*setWindowLong)(uintptr_t, int, intptr_t);
    intptr_t (*sendMessage)(uintptr_t, uint32_t, uintptr_t, intptr_t);
    uintptr_t (*getTopWindow)(uintptr_t); uintptr_t (*getWindow)(uintptr_t, int);
    void (*getClassName)(uintptr_t, char*, int);
};
extern "C" uintptr_t Storm_N107_1DF90(const StormN107DlgEnv* e, uintptr_t hinst, const int32_t* tmpl, uintptr_t parent,
                                      const void* initParam, intptr_t lparam) {
    if (parent == 0) parent = e->mainWnd;
    bool ex = (int32_t)tmpl[0] == -0xffff;
    StormN107DlgHdr h{};
    e->parse(tmpl, ex, &h);
    char title[256] = {}, face[256] = {};
    if (h.title && *h.title != 0 && *h.title != 0xffff) e->wideToNarrow(title, h.title, 0xff);
    if (h.face && *h.face != 0 && *h.face != 0xffff) e->wideToNarrow(face, h.face, 0xff);
    uintptr_t font = 0;
    if (face[0]) {
        const int32_t* gf = e->globalFont;
        int sz = gf ? gf[0] : h.pointSize, wt = gf ? gf[1] : h.weight, it = gf ? 0 : h.italic, pitch = gf ? gf[3] : 0;
        const char* fn = gf ? (const char*)(gf + 4) : face;
        font = e->createFont(-e->mulDiv(sz, 0x60, 0x48), 0, 0, 0, wt, it, 0, 0, 1, 0, 0, 0, pitch, fn);
        if (font) {
            uintptr_t dc = e->getDC(parent), mdc = e->createCompatDC(dc);
            uintptr_t old = e->selectObject(mdc, font);
            bool tt = e->isTrueType(mdc);
            e->selectObject(mdc, old); e->deleteDC(mdc); e->releaseDC(parent, dc);
            if (!tt) {
                e->deleteObject(font);
                font = e->createFont(-e->mulDiv(sz, 0x60, 0x48), 0, 0, 0, wt, gf ? 0 : h.italic, 0, 0, 1, 0, 0, 0, 0x20, "Arial");
            }
        }
    }
    int32_t bx = 8, by = 16;
    if (font) e->fontMetrics(font, &bx, &by);
    uint32_t exs = (((h.style & 0x80) | 0x800000) >> 7) | h.exstyle;
    int32_t left = h.x * bx / 4, top = h.y * by / 8;
    int32_t rc[4] = {left, top, h.cx * bx / 4 - 1 + left, h.cy * by / 8 - 1 + top};
    e->adjustRect(rc, h.style & 0xefff0000, exs);
    int sw = e->haveRes == 0 ? e->sysMetric(0) : e->resW, sh = e->haveRes == 0 ? e->sysMetric(1) : e->resH;
    int w = rc[2] - rc[0] + 1, ht = rc[3] - rc[1] + 1;
    int x = left, y = top;
    if (h.style & 0x800) { x = (sw - w) >> 1; y = (sh - ht) >> 1; }
    int mx = sw - w, my = sh - ht;
    x = (x < mx ? x : mx) < 0 ? 0 : (x < mx ? x : mx);
    y = (y < my ? y : my) < 0 ? 0 : (y < my ? y : my);
    if (e->mainWnd && e->isIconic(e->mainWnd)) {
        e->showWindow(e->mainWnd, 3);
        while (e->pump()) {}
        e->setActive(parent); e->setFocus(parent);
    }
    uintptr_t dlg = e->createWindowEx(exs, "SDlgDialog", title, h.style & 0xefffffff, x, y, w, ht, parent, 0, hinst, initParam);
    if (!dlg) return 0;
    uintptr_t dc = e->getDC(dlg);
    e->selectObject(dc, font);
    e->releaseDC(dlg, dc);
    e->setProp(dlg, "SDlg_EndDialog", 0);
    e->setProp(dlg, "SDlg_EndResult", 0);
    uintptr_t focus = dlg;
    const uint8_t* it = h.items;
    for (uint32_t n = h.count; n != 0; n--) {
        uint32_t mis = (uint32_t)((uintptr_t)it - (uintptr_t)tmpl) & 3;
        if (mis) it += 4 - mis;
        if (ex) it += 4;
        const uint16_t* cls = (const uint16_t*)(it + (ex ? 0x14 : 0x12));
        const uint16_t* ttl = *cls == 0xffff ? cls + 2 : cls + e->wcslen(cls) + 1;
        const uint16_t* extra = *ttl == 0xffff ? ttl + 3 : ttl + e->wcslen(ttl) + 2;
        char cn[256] = {}, tt[256] = {};
        if (*cls == 0xffff) {
            static const char* const k[] = {"Button", "Edit", "SDlgStatic", "Listbox", "Scrollbar", "Combobox"};
            // 0x81 names DAT_6fc37520 in retail; the standard "Edit" class is the only 0x81 atom.
            if (cls[1] >= 0x80 && cls[1] <= 0x85) e->strcpyN(cn, k[cls[1] - 0x80], 0x100);
        } else e->wideToNarrow(cn, cls, 0xff);
        if (*ttl != 0xffff) e->wideToNarrow(tt, ttl, 0xff);
        const uint32_t* w32 = (const uint32_t*)it;
        uint32_t st = ex ? w32[1] : w32[0], xs = ex ? w32[0] : w32[1];
        int16_t ix = *(const int16_t*)(it + 8), iy = *(const int16_t*)(it + 10);
        int16_t iw = *(const int16_t*)(it + 12), ih = *(const int16_t*)(it + 14);
        uint16_t id = *(const uint16_t*)(it + 16);
        uintptr_t c = e->createWindowEx(xs, cn, tt, st, ix * bx / 4, iy * by / 8, iw * bx / 4, ih * by / 8, dlg, id, hinst, extra);
        e->setProp(c, "SDlg_WndProc", (uintptr_t)e->getWindowLong(c, -4));
        e->setWindowLong(c, -4, (intptr_t)e->subclassProc);
        if (c && font) e->sendMessage(c, 0x30, font, 0);
        e->showWindow(c, 5);
        if (c && focus == dlg && !(st & 0x8000000) && strcasecmp(cn, "Static") != 0 && strcasecmp(cn, "SDlgStatic") != 0)
            focus = c;
        it = (const uint8_t*)extra + extra[-1];
    }
    if (font) e->sendMessage(dlg, 0x30, font, 0);
    if (e->sendMessage(dlg, 0x110, focus, lparam) == 0) focus = 0;
    for (uintptr_t c = e->getTopWindow(dlg); c; c = e->getWindow(c, 2)) {
        if (!e->skin(c, 0xffff)) continue;
        uintptr_t os = (uintptr_t)e->getWindowLong(c, -0x10);
        e->setProp(c, "SDlg_OrigStyle", os);
        char cn[256] = {};
        e->getClassName(c, cn, 0x100);
        uintptr_t ns;
        if (strcasecmp(cn, "Button") == 0 && ((os & 0xf) == 0 || (os & 0xf) == 1)) ns = os | 0xb;
        else if (strcasecmp(cn, "ComboBox") == 0 || strcasecmp(cn, "ListBox") == 0) ns = os | 0x10;
        else continue;
        if (ns != os) e->setWindowLong(c, -0x10, (intptr_t)ns);
    }
    if (e->getProp(dlg, "SDlg_EndDialog") == 0) {
        e->showWindow(dlg, 1);
        if (focus) e->setFocus(focus);
    }
    return dlg;
}
