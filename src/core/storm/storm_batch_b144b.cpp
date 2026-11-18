// src/core/storm/storm_batch_b144b.cpp -- twelve 1.13c Storm.dll records transcribed from the cached
// decompiles (build-wasm/ghidra-storm/decomp/0x<RVA>.c). Stage A batch lane: implementation + marker
// only, no spec, no vector. Syntax-checked only, never run.
//
// Retail is 32-bit: node/record words stay uint32_t and are read at retail byte offsets. Retail stores
// *pointers* in those words; N() widens them back with uintptr_t, which is only sound while the
// object lives below 4 GiB (ponytail ceiling: wasm64 heap, same as storm_sub_1bdd0.cpp). Module state
// below is private to this TU (retail addresses in the comments), not shared with the other batches.
// Win32 / DirectSound calls go through W(): deterministic no-op (returns 0) until a back end installs
// g_win32. Callee rows not in this batch stay seams with retail argument order.
#include <cstdint>
#include <cstring>
#include <Windows.h>

// ---- seams: callees owned by other rows -------------------------------------------------------
extern "C" void*   Storm_Sub1C580(uint32_t size, const char* tag, int32_t line, uint32_t flags);   // SMemAlloc
extern "C" int32_t Storm_Sub1B960(void* p, const char* file, int32_t line);                        // SMemFree
extern "C" void Storm_Sub4040(uint32_t o);                                               // request-record dtor (C linkage: storm_4040.cpp)
void Storm_Sub4D60(uint8_t* node);                                                      // list unlink (storm_batch_b47.cpp)
inline void Storm_Sub4D60(void* p) { Storm_Sub4D60((uint8_t*)p); }
uint32_t Storm_Sub4250(void* listHead);                                                 // list next
void    Storm_Sub2FB30(const char* type, const char* name);
void    Storm_Sub1F590(uint32_t err, uint32_t* name, int32_t off, void*, uint32_t, uint32_t);
uint32_t Storm_Sub25BA0(void);
void    Storm_Sub17FC0(void);
void    Storm_Sub38220(int32_t* p);                                                     // 0x38220
void    Storm_Sub3BB50(void);                                                           // 0x3BB50
void    Storm_Sub22C50(void* buf, uint32_t n, uint8_t fill);
uint32_t* Storm_Sub27A70(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t,
                         int32_t, uint32_t, uint32_t, uint32_t, int32_t, uint32_t, uint32_t);
uint32_t Storm_Sub28E80(uint32_t a, uint32_t* b, uint32_t c);
void    Storm_Sub26D70(int32_t* io, uint32_t* src, uint32_t n, uint32_t* key);
int32_t Storm_Sub241D0(uint32_t hwnd, uint32_t msg, uint32_t wp, intptr_t lp, int32_t* handled, intptr_t* res);
// SDLG helpers (0x1CAF0, 0x1CD30, 0x1EFC0, 0x1FB50, 0x1D2E0, 0x1E830, 0x382C0)
void    Storm_Sub1CAF0(void);
void*   Storm_Sub1CD30(uint32_t kind);
void    Storm_Sub1FB50(void* hwnd, void* paintstruct);
void    Storm_Sub1EFC0(void* hwnd, uint32_t kind, void* rgn, int32_t, int32_t, uint32_t* r, int32_t);
void    Storm_Sub1D2E0(void* root, void* rgn, void* child);
void    Storm_Sub1E830(void* dst, int32_t* rc, int32_t pitch, int32_t src, int32_t* srcpt, int32_t, int32_t,
                       int32_t, int32_t, uint32_t rop, int32_t* org, void* rgn, uint32_t flags, int32_t,
                       int32_t* extra);
void    Storm_Sub382C0(int32_t a, uint32_t buf, int32_t b, int32_t c, int32_t d, int32_t e, int32_t f,
                       int32_t g, uint32_t rop);
extern uint32_t* g_stormHeapLists_1bdd0[0x100];                                          // 0x6FC43150
struct StormPageEnv {                                                                    // as storm_1b1c0.cpp
  bool trackingOff; void (*setLastError)(uint32_t);
  void (*errDisplay)(uint32_t, const char*, int32_t); void (*debugOut)(const char*);
  void (*blockFree)(uint8_t*, uint8_t*); void (*virtualFree)(void*);
};
extern "C" uint32_t* Storm_Sub1B1C0(StormPageEnv& e, uint32_t* head, bool errHook);

namespace {

intptr_t (*g_win32)(const char* api, const intptr_t* a, int n) = nullptr;
// retail IAT slot of each import a probe records (Storm .rdata); 0 = not recorded
uint32_t ImpSlot(const char* api) {
    static const struct { const char* n; uint32_t s; } t[] = { { "GetTopWindow", 0x432a0 }, { "GetWindow", 0x43358 } };
    for (const auto& e : t) if (!strcmp(api, e.n)) return e.s;
    return 0;
}
}  // namespace
extern "C" uint32_t (*g_storm_imp_hook)(uint32_t slot, uint32_t w0, uint32_t w1, uint32_t w2);   // storm_batch_16260.cpp
namespace {
template <class... A> intptr_t W(const char* api, A... a) {
    intptr_t v[sizeof...(A) + 3] = {(intptr_t)a..., 0};
    if (g_storm_imp_hook)
        if (uint32_t s = ImpSlot(api)) return (intptr_t)g_storm_imp_hook(s, (uint32_t)v[0], (uint32_t)v[1], (uint32_t)v[2]);
    return g_win32 ? g_win32(api, v, (int)sizeof...(A)) : 0;
}
inline uint8_t* N(uint32_t a) { return (uint8_t*)(uintptr_t)a; }
inline uint32_t& U(const void* p, uint32_t off) { return *(uint32_t*)((uint8_t*)p + off); }
inline uint32_t Lo(const void* p) { return (uint32_t)(uintptr_t)p; }  // truncation: see header
inline uint32_t Pos(uint32_t x) { return (int32_t)x < 1 ? 0 : x; }   // retail "(x<1)-1 & x"

// ---- module state (retail address) -------------------------------------------------------------
uint32_t g_45168 = 1;        // 0x6FC45168 heap set live (initial value not verified)
uint32_t g_44d60[0x100];     // 0x6FC44D60
CRITICAL_SECTION g_heapCs[0x100]; // 0x6FC43558
uint32_t g_45a08;            // 0x6FC45A08 __AUBITMAPREC__ list
uint32_t g_45960, g_4537c, g_45380, g_415f8 = 0xffffffffu; // 0x6FC45960 handler list / 4537C / 45380 / 415F8
CRITICAL_SECTION g_42fe0;    // 0x6FC42FE0
uint32_t g_45390, g_45394, g_458ac; // 0x6FC45390/94/458AC
uint32_t g_458940[8], g_458a00[8];  // 0x6FC45894 / 0x6FC458A0 (FUN_38220 targets)
CRITICAL_SECTION g_45988;    // 0x6FC45988
uint32_t g_43114, g_43128, g_43140, g_41a08, g_43124, g_41a00; // 0x6FC43114/28/40, 41A08, 43124, 41A00 (.data values not verified)
uint32_t g_4311c, g_459cc, g_45978, g_43104, g_43108, g_4312c, g_459a8; // 0x6FC4311C/459CC/45978/43104/43108/4312C/459A8
uint32_t g_453c0, g_453ac, g_453b4, g_453b0, g_453bc, g_453e4, g_45388; // SDLG state 0x6FC453C0/AC/B4/B0/BC/E4, 45388
int32_t g_41a0c, g_41a10, g_41a14, g_41a18, g_41a20, g_41a24;         // 0x6FC41A0C.. (.data values not verified)

const int32_t kAdj[32] = {-1,0,-1,4,-1,2,-1,6,-1,1,-1,5,-1,3,-1,7,-1,1,-1,5,-1,3,-1,7,-1,2,-1,4,-1,6,-1,8};
const uint32_t kStep[89] = {  // 0x6FC36690
 7,8,9,10,11,12,13,14,16,17,19,21,23,25,28,31,34,37,41,45,50,55,60,66,73,80,88,97,107,118,130,143,
 157,173,190,209,230,253,279,307,337,371,408,449,494,544,598,658,724,796,876,963,1060,1166,1282,1411,
 1552,1707,1878,2066,2272,2499,2749,3024,3327,3660,4026,4428,4871,5358,5894,6484,7132,7845,8630,9493,
 10442,11487,12635,13899,15289,16818,18500,20350,22385,24623,27086,29794,32767};

}  // namespace

extern "C" uint32_t* Storm_B144B_BitmapHead() { return &g_45a08; }  // FN-probe accessor for the __AUBITMAPREC__ list head

// 1.13c Storm+0x1bd00: tear down all 0x100 heap lists (frees empty pages, reports leaks via 1B1C0).
// Slots are retail's 32-bit words; the seams are the critical sections, 2FB30, 1B1C0 and VirtualFree.
struct StormTeardownEnv {
    uint32_t* gate;                     // 0x6FC45168
    uint32_t* dirty;                    // 0x6FC44D60
    uint32_t* lists;                    // 0x6FC43150
    void (*enter)(int id); void (*leave)(int id); void (*destroy)(int id);
    void (*leak)(const char* type, const char* name);   // 2FB30
    uint32_t* (*pageTeardown)(uint32_t* slot);          // 1B1C0
    void (*release)(void* page);                        // VirtualFree(page, 0, MEM_RELEASE)
};
extern "C" uint32_t Storm_Sub1BD00Env(const StormTeardownEnv& e) {
    if (*e.gate == 0) return 1;
    *e.gate = 0;
    for (int i = 0; i < 0x100; i++) {
        e.enter(i);
        e.dirty[i] = 0;
        uint32_t* pp = &e.lists[i];
        while (*pp) {
            uint8_t* pg = N(*pp);
            if (U(pg, 0x14) == 0) {
                if (U(pg, 0x10) != 0 && 0x7fffffffu < U(pg, 4)) e.leak("HSHEAP", nullptr);
                *pp = U(pg, 0);     // next page (32-bit link, see header)
                e.release(pg);
            } else {
                pp = e.pageTeardown(pp);
            }
        }
        e.leave(i);
        e.destroy(i);
    }
    return 1;
}

uint32_t Storm_Sub1BD00(void) {
    static uint32_t lists[0x100];
    for (int i = 0; i < 0x100; i++) lists[i] = Lo(g_stormHeapLists_1bdd0[i]);
    StormPageEnv pageEnv{};
    static StormPageEnv* gp; gp = &pageEnv;
    StormTeardownEnv e{ &g_45168, g_44d60, lists,
        [](int i) { EnterCriticalSection(&g_heapCs[i]); },
        [](int i) { LeaveCriticalSection(&g_heapCs[i]); },
        [](int i) { DeleteCriticalSection(&g_heapCs[i]); },
        [](const char* t, const char* n) { Storm_Sub2FB30(t, n); },
        [](uint32_t* slot) { return Storm_Sub1B1C0(*gp, slot, true); },
        [](void* pg) { free(pg); } };   // 1bdd0 allocates with calloc
    uint32_t r = Storm_Sub1BD00Env(e);
    for (int i = 0; i < 0x100; i++) g_stormHeapLists_1bdd0[i] = (uint32_t*)N(lists[i]);
    return r;
}

// 1.13c Storm+0x1d6b0: free every __AUBITMAPREC__ bound to hwnd (or its owner), then recurse children.
void Storm_Sub1D6B0(void* hwnd) {
    uint32_t n = Pos(g_45a08);
    while ((int32_t)n > 0) {
        uint8_t* r = N(n);
        if (N(U(r, 8)) == hwnd || N(U(r, 12)) == hwnd) {
            uint32_t next = U(r, 4);
            Storm_Sub4D60(r);
            Storm_Sub1B960(r, "__AUBITMAPREC__", -2);
            n = Pos(next);
        } else {
            n = U(r, 4);
        }
    }
    for (void* c = (void*)W("GetTopWindow", hwnd); c; c = (void*)W("GetWindow", c, 2)) Storm_Sub1D6B0(c);
}

// 1.13c Storm+0x20170: SDlg owner-draw of a button/caption (dlg: +0x10 flags, +0x14 hwnd, +0x18 hdc).
// Win32 GDI is entirely behind W(); the geometry and branch structure follow the decompile.
uint32_t Storm_Sub20170(uint8_t* dlg) {
    void* hwnd = N(U(dlg, 0x14));  // 32-bit handle widened
    int32_t rcClient[4] = {0, 0, 0, 0};
    W("GetClientRect", hwnd, rcClient);
    void* hdc = (void*)W("GetDC", hwnd);
    uint32_t pressed = U(dlg, 0x10) & 1;
    if (W("SendMessageA", hwnd, 0xf2, 0, 0) & 1) pressed = 1;
    uint32_t radio = (U(dlg, 0x10) & 6) != 0;
    uintptr_t style = (uintptr_t)W("GetWindowLongA", hwnd, -0x10);
    if (W("GetPropA", hwnd, "SDlg_OrigStyle")) style = (uintptr_t)W("GetPropA", hwnd, "SDlg_OrigStyle");
    uint32_t type = (uint32_t)style & 0xf;
    int32_t isButton = (type == 0 || type == 1 || type == 0xb);
    uint32_t kind;
    if (pressed == 0) kind = radio ? 0x100 : ((U(dlg, 0x10) & 0x10) ? 0x20 : 0x10);
    else              kind = radio ? 0x400 : ((U(dlg, 0x10) & 0x10) ? 0x80 : 0x40);
    int32_t rc[4] = {0, 0, 0, isButton ? 0 : -1};
    uint8_t* skin = (uint8_t*)Storm_Sub1CD30(kind);
    if (skin) {
        Storm_Sub1EFC0(hwnd, kind, nullptr, 0, 0, (uint32_t*)rc, isButton ? 3 : 0);
    } else {
        int32_t bad = 0;
        if (!hwnd) bad = 1;
        else if (W("IsWindow", hwnd) && !(W("GetWindowLongA", hwnd, -0x14) & 0x20)) {
            uint8_t* s = (uint8_t*)Storm_Sub1CD30(1);
            if (s) {
                int32_t cr[4];
                W("GetClientRect", hwnd, cr);
                W("ClientToScreen", hwnd, cr); W("ClientToScreen", hwnd, cr + 2);
                cr[0] += Pos(rc[0]);
                if (!isButton) {
                    int32_t m = (int32_t)(U(s, 0x24) - U(s, 0x1c)) + cr[0];
                    if (m < cr[2]) cr[2] = m;
                }
                // The region/blit pipeline (CreateRectRgn, CombineRgn, 1E830/382C0 pair on the SDLG.CPP
                // scratch buffer, DrawEdge) is the identical W()/seam call sequence of the decompile;
                // reduced here to the two seam calls that carry the pixels.
                Storm_Sub1E830(nullptr, cr, 0, (int32_t)U(s, 0x18), (int32_t*)(s + 0x2c),
                               (int32_t)U(s, 0x2c), 1 - (int32_t)pressed, 1 - (int32_t)pressed, 0, 0xcc0020,
                               cr, nullptr, g_453ac == 0 ? (g_453bc == 0 ? 0x3003 : 0x1003) : 3, 0, nullptr);
            } else bad = 1;
        } else bad = 1;
        if (bad) { g_45388 = 0x57; W("SetLastError", 0x57); }
        if (isButton) W("DrawEdge", hdc, rcClient, pressed ? 10 : 1, 0xf);
    }
    int32_t len = (int32_t)W("GetWindowTextLengthA", hwnd);
    char* txt = (char*)Storm_Sub1C580((uint32_t)len + 1, "..\\3rdParty\\STORM\\SOURCE\\SDLG.CPP", 0x24c, 0);
    W("GetWindowTextA", hwnd, txt, len + 1);
    W("SelectObject", hdc, W("GetCurrentObject", U(dlg, 0x18), 6));
    W("SetTextAlign", hdc, 0); W("SetBkMode", hdc, 1);
    int32_t tr[4] = {rcClient[0] + 3, rcClient[1] + 3, rcClient[2] - 1, rcClient[3] - 1};
    if (!isButton) {
        if (skin) { tr[1] = rcClient[1] + 2; tr[0] = rcClient[0] + 9 + (int32_t)(U(skin, 0x24) - U(skin, 0x1c)); }
    } else {
        uint32_t st = (uint32_t)W("GetWindowLongA", hwnd, -0x10);
        int32_t ext[2] = {0x10, 0x10};
        W("GetTextExtentPoint32A", hdc, "~,_Oy", 5, ext);
        if (!(st & 0x800)) { if ((st & 0xc00) || !(st & 0x400)) tr[1] += 1 + ((tr[3] - ext[1]) - tr[1]) / 2; }
        else tr[1] += -6 + (tr[3] - ext[1]);
        if (pressed) { tr[0]++; tr[1]++; tr[2]++; tr[3]++; }
    }
    W("SetTextColor", hdc, 0);
    W("DrawTextA", hdc, txt, len, tr, (uint32_t)isButton);
    for (int i = 0; i < 4; i++) tr[i]--;
    W("SetTextColor", hdc, radio ? 0xff808081u + 0xffffff : 0xffffffu);
    W("DrawTextA", hdc, txt, len, tr, (uint32_t)isButton);
    Storm_Sub1B960(txt, "..\\3rdParty\\STORM\\SOURCE\\SDLG.CPP", 0x27b);
    if (isButton && (U(dlg, 0x10) & 0x10) && Storm_Sub1CD30(0xa0) == nullptr) {
        int32_t fr[4] = {rcClient[0] + 4 + (int32_t)pressed, rcClient[1] + 4 + (int32_t)pressed,
                         rcClient[2] - 3 + (int32_t)pressed, rcClient[3] - 3 + (int32_t)pressed};
        W("DrawFocusRect", hdc, fr);
    }
    W("ReleaseDC", hwnd, hdc);
    return 1;
}

// 1.13c Storm+0x20b70: SDlg dialog procedure (subclass of the dialog class).
intptr_t Storm_Sub20B70(void* hwnd, uint32_t msg, void* wp, int32_t* lp) {
    switch (msg) {
    case 1: W("SetCursor", W("LoadCursorA", 0, 0x7f00)); break;
    case 2: W("RemovePropA", hwnd, "SDlg_Font"); Storm_Sub1D6B0(hwnd); break;
    case 0xf: {
        uint8_t ps[0x40] = {};
        Storm_Sub1FB50(hwnd, ps);
        Storm_Sub1CAF0();
        if (W("GetPropA", hwnd, "SDlg_Modal")) {
            void* parent = (void*)W("GetParent", hwnd);
            if (W("IsWindowEnabled", parent)) {
                W("EnableWindow", (void*)W("GetParent", hwnd), 0);
                if ((void*)W("GetActiveWindow") == (void*)W("GetParent", hwnd)) W("SetActiveWindow", hwnd);
            }
        }
        return 0;
    }
    case 0x14:
        if ((W("GetWindowLongA", hwnd, -0x14) & 0x20) || Storm_Sub1CD30(0xffff)) return 0;
        break;
    case 0x18:
        if (wp == nullptr && (lp == nullptr || lp == (int32_t*)1) && W("GetPropA", hwnd, "SDlg_Modal"))
            W("EnableWindow", (void*)W("GetParent", hwnd), 1);
        break;
    case 0x2b:
        if (*lp == 4) return (intptr_t)Storm_Sub20170((uint8_t*)lp);
        break;
    case 0x30: W("SetPropA", hwnd, "SDlg_Font", wp); break;
    case 0x31: return W("GetPropA", hwnd, "SDlg_Font");
    case 0x82: {
        void* dc = (void*)W("GetDC", hwnd);
        W("SelectObject", dc, W("GetStockObject", 0xd));
        W("ReleaseDC", hwnd, dc);
        W("DeleteObject", W("SendMessageA", hwnd, 0x31, 0, 0));
        break;
    }
    }
    intptr_t r;
    if (g_453c0 == 0) r = W("DefDlgProcA", hwnd, msg, wp, lp);
    else { g_453c0 = 0; r = W("CallWindowProcA", W("GetWindowLongA", hwnd, 4), hwnd, msg, wp, lp); }
    if (msg == 0x81) W("SetWindowLongA", hwnd, 4, *lp);
    return r;
}

// 1.13c Storm+0x21560: Storm ADPCM decoder. `in` has [0]=?, [1]=shift, then ch int16 seeds, then nibble-coded
// bytes up to in+inSize. Returns bytes written to `out` (stops early when outCap is exhausted).
int32_t Storm_Sub21560(uint8_t* out, uint32_t outCap, const uint8_t* in, uint32_t inSize, uint32_t ch) {
    int32_t idx[2] = {0x2c, 0x2c};
    int32_t pred[2];
    uint8_t* o = out;
    const uint8_t* p = in + 2;
    uint32_t c = 0;
    for (; c < ch; c++) {
        pred[c] = (int16_t)(p[0] | (p[1] << 8));
        p += 2;
        if (outCap < 2) return (int32_t)(o - out);
        *(int16_t*)o = (int16_t)pred[c];
        outCap -= 2; o += 2;
    }
    c = ch - 1;
    const uint8_t* end = in + inSize;
    for (;;) {
        if (p >= end) return (int32_t)(o - out);
        if (ch == 2) c = (c == 0);
        uint8_t b = *p++;
        if ((int8_t)b < 0) {
            uint32_t k = b & 0x7f;
            if (k != 0) {
                if (k == 1) { idx[c] += 8; if (idx[c] > 0x58) idx[c] = 0x58; }
                else if (k == 2) { idx[c] -= 8; if (idx[c] < 0) idx[c] = 0; }
                else continue;
                if (ch == 2) c = (c == 0);
                continue;
            }
            if (idx[c] != 0) idx[c]--;
            if (outCap < 2) return (int32_t)(o - out);
            *(int16_t*)o = (int16_t)pred[c]; o += 2; outCap -= 2;
            continue;
        }
        uint32_t step = kStep[idx[c]];
        uint32_t d = step >> (in[1] & 0x1f);
        if (b & 1) d += step;
        if (b & 2) d += step >> 1;
        if (b & 4) d += step >> 2;
        if (b & 8) d += step >> 3;
        if (b & 0x10) d += step >> 4;
        if (b & 0x20) d += step >> 5;
        int32_t v;
        if (!(b & 0x40)) { v = pred[c] + (int32_t)d; if (v > 0x7ffe) v = 0x7fff; }
        else { v = pred[c] - (int32_t)d; if (v < -0x7fff) v = -0x8000; }
        pred[c] = v;
        if (outCap < 2) return (int32_t)(o - out);
        *(int16_t*)o = (int16_t)v; outCap -= 2; o += 2;
        idx[c] += kAdj[b & 0x1f];
        if (idx[c] < 0) idx[c] = 0; else if (idx[c] > 0x58) idx[c] = 0x58;
    }
}

// 1.13c Storm+0x25c70: SFile/SBlt module attach (reason 1) / detach (reason 0).
uint32_t Storm_Sub25C70(uint32_t hinst, int32_t reason, int32_t reserved) {
    if (reason == 1) {
        g_45390 = hinst;
        g_45394 = (uint32_t)W("SetUnhandledExceptionFilter", 0x6fc15b70);
        if (g_458ac == 0) {
            g_458ac = Lo(Storm_Sub1C580(0xc00, "..\\3rdParty\\STORM\\SOURCE\\SBLT.CPP", 0x7e, 8));
            Storm_Sub38220((int32_t*)g_458940);
            Storm_Sub38220((int32_t*)g_458a00);
        }
        Storm_Sub3BB50();
        if (reserved == 0) { Storm_Sub17FC0(); return 1; }
    } else if (reason == 0) {
        W("SetUnhandledExceptionFilter", g_45394);
        Storm_Sub25BA0();
    }
    return 1;
}

// 1.13c Storm+0x26930: refill one streaming DirectSound buffer for request `req` (the audio worker's
// per-chunk copy). Retail's lock pointer lives at req+0x20; here a local, low 32 bits mirrored there.
// The new write position adds the locked byte count (retail reloads it from the lock's out slot); `esi` is unused.
void Storm_Sub26930(uint8_t* req, uint32_t esi) {
    (void)esi;
    EnterCriticalSection(&g_45988);
    uint8_t* ds = N(U(req, 0x28));
    uint8_t* rec = N(U(req, 0x30));
    uint8_t* locked = nullptr;
    uint32_t got = 0;
    // IDirectSoundBuffer::Lock (vtable +0x2c)
    intptr_t hr = W("IDirectSoundBuffer::Lock", ds, U(req, 0x2c), g_43114, &locked, &got);
    if (hr >= 0 && got == g_43114) {
        U(req, 0x20) = Lo(locked);
        uint32_t n = got < U(req, 0x3c) ? got : U(req, 0x3c);
        memcpy(locked, N(g_43128), n);
        if (n < got) memset(locked + n, *(uint8_t*)(rec + 0x48), got - n);
        W("IDirectSoundBuffer::Unlock", ds, locked, got, 0, 0);  // vtable +0x4c
        U(rec, 0x30) = (U(req, 0x10) - U(rec, 0x20)) + got;
        if (U(rec, 0x24) < U(rec, 0x30)) U(rec, 0x1c) = U(rec, 0x30) - U(rec, 0x24);
        if (U(rec, 0x18) == 1) {
            uint32_t v = U(rec, 0x24) < 0x10001 ? U(rec, 0x24) : 0x10000;
            if (U(rec, 0x40) <= v) v = U(rec, 0x40);
            if (v <= (U(req, 0x34) - U(rec, 0x20)) + U(req, 0x10)) {
                W("IDirectSoundBuffer::Play", N(U(rec, 0x3c)), 0, 0, 1);  // vtable +0x30
                U(rec, 0x18) = 2;
            }
        }
    }
    LeaveCriticalSection(&g_45988);
}

// 1.13c Storm+0x27250: pick the next request to service from the list at g_45978. Returns non-zero when
// a "due" class (<1000ms and ready / queued / short) was chosen; *out is the chosen node or null.
// The pure core: head = the word at the list head (g_45978), useArchive = g_41a08, now = GetTickCount().
extern "C" uint32_t Storm_Pick27250(uint32_t head, uint32_t useArchive, uint32_t now, int32_t curArchive, uint32_t curPos, uint32_t* out) {
    uint32_t best = 0, fDue = 0, fQueued = 0, fShort = 0;
    int32_t bestDelta = 0x7fffffff;
    uint32_t score = 0xffffffffu;
    uint32_t n = Pos(head);
    while ((int32_t)n > 0) {
        uint8_t* r = N(n);
        uint32_t t;
        bool skip = false;
        if (U(r, 0x24) != 0) {
            t = U(N(U(N(U(r, 0)), 0)), 4);   // retail: mov edx,[ecx]; mov edx,[edx]; mov esi,[edx+4]
            if ((int32_t)t > 0 && U(r, 0x24) == U(N(Pos(t)), 0x24)) skip = true;
        }
        if (!skip) {
            int32_t delta = (int32_t)(U(r, 0x18) - now);
            uint32_t sc = score;
            bool take = false;
            if (delta < 1000 && U(r, 0x30) != 0 && (fDue == 0 || delta < bestDelta)) { fDue = 1; take = true; }
            else if (fDue == 0) {
                int32_t pri = (int32_t)U(r, 0x1c);
                if (pri > 0 && (fQueued == 0 || delta < bestDelta)) { fQueued = 1; take = true; }
                else if (fQueued == 0) {
                    if (delta < 1000 && delta < bestDelta) { fShort = 1; take = true; }
                    else if (fShort == 0 && (best == 0 || (int32_t)U(N(best), 0x1c) <= pri)) {
                        sc = 0;
                        if (useArchive != 0) {
                            uint8_t* ar = N(U(N(U(r, 0x0c)), 0x110));
                            uint32_t d = 0;
                            if (ar && U(ar, 0x120) < curPos) d = curPos - U(ar, 0x120);
                            if ((int32_t)Lo(ar) == curArchive) {
                                sc = U(r, 0x14);
                                sc = sc < d ? sc + 0x8000000u : sc - d;
                            } else sc = 0xfffffffeu;
                        }
                        if (best == 0 || (int32_t)U(N(best), 0x1c) < pri || sc < score ||
                            (sc == score && delta < bestDelta)) take = true;
                    }
                }
            }
            if (take) { best = n; bestDelta = delta; score = sc; }   // tiers 1-3 jump past the score store (0x273be)
        }
        n = U(r, 4);
        if ((int32_t)n < 1) break;
    }
    *out = best;
    return fShort | fQueued | fDue;
}
uint32_t Storm_Sub27250(int32_t curArchive, uint32_t curPos, uint32_t* out) {
    return Storm_Pick27250(g_45978, g_41a08, GetTickCount(), curArchive, curPos, out);
}

// 1.13c Storm+0x27b80: the stream service step. The port is storm_27b80.cpp; this binds it to the module words, the 0x25DC0
// position probe (through the buffer's GetCurrentPosition) and the DirectSound calls, which stay W() seams.
struct Storm27B80Env {
    uint32_t g4311c, g459cc, g43114, g43140;
    uint32_t (*cursor)(uint32_t rec);
    void (*fill)(uint32_t p, uint32_t n, uint32_t byte);
    int32_t (*lock)(uint32_t buf, uint32_t off, uint32_t len, uint32_t* p, uint32_t* n);
    void (*unlock)(uint32_t buf, uint32_t p, uint32_t n);
    uint32_t (*stop)(uint32_t buf);
    void (*queue)(uint32_t owner, const uint32_t* a);
};
extern "C" uint32_t Storm13c_27B80(const Storm27B80Env* e);
static uint32_t Storm27B80Cursor(uint32_t rec) {   // 0x25DC0
    uint8_t* o = N(rec);
    uint32_t a = 0, b = 0;
    if ((int32_t)W("IDirectSoundBuffer::GetCurrentPosition", N(U(o, 0x3c)), &a, &b) < 0) return 0xffffffffu;
    int32_t d = (int32_t)(U(o, 0x30) % U(o, 0x40) - a);
    if (d < 1) d += (int32_t)U(o, 0x40);
    uint32_t v = U(o, 0x30) - (uint32_t)d;
    if (U(o, 0x24) <= v) v = U(o, 0x24);
    U(o, 0x2c) = v;
    return a;
}
static int32_t Storm27B80Lock(uint32_t buf, uint32_t off, uint32_t len, uint32_t* p, uint32_t* n) {
    uint8_t* q = nullptr;
    int32_t r = (int32_t)W("IDirectSoundBuffer::Lock", N(buf), off, len, &q, n);
    *p = Lo(q);
    return r;
}
uint32_t Storm_Sub27B80(void) {
    Storm27B80Env e{ g_4311c, g_459cc, g_43114, g_43140, Storm27B80Cursor,
        [](uint32_t p, uint32_t n, uint32_t byte) { Storm_Sub22C50(N(p), n, (uint8_t)byte); },
        Storm27B80Lock,
        [](uint32_t buf, uint32_t p, uint32_t n) { W("IDirectSoundBuffer::Unlock", N(buf), N(p), n, 0, 0); },
        [](uint32_t buf) { return (uint32_t)W("IDirectSoundBuffer::Stop", N(buf)); },
        [](uint32_t owner, const uint32_t* a) {
            Storm_Sub27A70(a[0], a[1], a[2], a[3], a[4], a[5], a[6], (int32_t)a[7], a[8], a[9], a[10], (int32_t)a[11], a[12], owner);
        } };
    return Storm13c_27B80(&e);
}

// 1.13c Storm+0x29200: audio worker thread body. Loops until g_43104 is set; each pass wakes waiters, advances
// streams (27B80), picks a request (27250), unlinks it, runs its decode/copy and completion callback.
uint32_t Storm_Sub29200(void) {
    int32_t curArchive = 0;
    uint32_t curPos = 0;
    uint32_t last = GetTickCount();
    while (g_43104 == 0) {
        uint32_t now = GetTickCount();
        uint32_t slice = g_43124 ? (g_41a00 * 1000u) / g_43124 : 0;
        EnterCriticalSection(&g_45988);
        for (uint32_t w = Pos(g_459a8); (int32_t)w > 0; w = Storm_Sub4250(&g_459a8)) W("SetEvent", N(U(N(w), 8)));
        Storm_Sub27B80();
        uint32_t picked = 0;
        uint32_t due = Storm_Sub27250(curArchive, curPos, &picked);
        if (picked == 0 || (due == 0 && now - last < slice)) {
            LeaveCriticalSection(&g_45988);
            W("WaitForSingleObject", N(g_4312c), picked == 0 ? 0xfa : (slice - now) + last);
            last = last;  // retail: DVar4 = DVar3 (timestamp kept)
            continue;
        }
        uint8_t* r = N(picked);
        curPos = U(r, 0x14);
        curArchive = (int32_t)U(N(U(r, 0x0c)), 0x110);
        g_43108 = picked;
        if (U(r, 0) != 0) {  // relative-link unlink (0x4C20 body)
            uint8_t* nx = N(U(r, 0));
            uint32_t l = U(r, 4);
            uint32_t* slot = (int32_t)l < 0 ? (uint32_t*)(uintptr_t)~l : (uint32_t*)(r + (l - U(nx, 4)));
            *slot = U(r, 0);
            U(nx, 4) = U(r, 4);
            U(r, 0) = 0; U(r, 4) = 0;
        }
        LeaveCriticalSection(&g_45988);
        uint32_t* src = (uint32_t*)N(U(r, 0x20));
        if (U(r, 0x28) != 0) src = (uint32_t*)N(g_43128);
        uint8_t* arch = N(U(r, 0x0c));
        if (src == nullptr) U(r, 0x3c) = 0;
        else if ((int32_t)U(arch, 0x10c) == -1) {
            U(r, 0x3c) = Storm_Sub28E80(U(r, 0x10), src, U(r, 0x34));
            if (U(arch, 0x14c) != 0 && U(arch, 0x150) == 2 && U(arch, 0x154) == U(N(U(arch, 0x114)), 8)) {
                U(arch, 0x144) = ~U(arch, 0x144);
                U(arch, 0x150) = 4;
            }
        } else {
            int32_t io = -1;
            Storm_Sub26D70(&io, src, U(r, 0x34), (uint32_t*)(arch + 8));
            U(r, 0x3c) = 0;
        }
        if (U(r, 0x28) != 0) {
            if (U(r, 0x3c) == 0) {
                if (U(r, 0x34) != 0) {
                    W("IDirectSoundBuffer::Stop", N(U(r, 0x28)));
                    if (U(r, 0x30) != 0) U(N(U(r, 0x30)), 0x18) = 4;
                }
            } else Storm_Sub26930(r, 0);
        }
        if (U(r, 0x40) != 0 && *(uint32_t*)N(U(r, 0x40)) != 0) {
            arch = N(U(r, 0x0c));
            if (U(arch, 0x148) != 0 && U(arch, 0x14c) != 0 && U(arch, 0x150) == 4 &&
                U(arch, 0x144) != U(N(U(arch, 0x114)), 0x18))
                Storm_Sub1F590(0x85100083, (uint32_t*)(arch + 8), -4, nullptr, 0, 1);
            // completion callback (request +0x40): (buf, bytesDone, self) -- 32-bit function pointer slot
            W("callback", U(r, 0x40), U(r, 0x24), (U(r, 0x3c) + U(r, 0x20)) - U(r, 0x24), U(r, 0x40));
        }
        uint32_t ev = U(r, 8);
        EnterCriticalSection(&g_45988);
        if (U(N(g_43108), 0x38) != 0) {
            Storm_Sub4040((uint32_t)(uintptr_t)N(g_43108));
            Storm_Sub1B960(N(g_43108), "delete", -1);
        }
        g_43108 = 0;
        LeaveCriticalSection(&g_45988);
        if (ev) W("SetEvent", N(ev));
        last = now;
    }
    g_43104 = 0;
    return 0;
}

// 1.13c Storm+0x2fab0: free all __AUHANDLER__ records and the chained blocks, drop the lock.
extern "C" uint32_t Storm_Sub2FAB0(void) {
    while ((int32_t)g_45960 > 0) {
        void* h = N(g_45960);
        uint32_t nx = *(uint32_t*)h;  // node[0] = successor; 4B80 makes it the new head
        Storm_Sub4D60(h);
        Storm_Sub1B960(h, "__AUHANDLER__", -2);
        g_45960 = nx;  // retail head = successor (unlink writes it through ~tag into 45960)
    }
    g_45380 = 0;
    for (uint8_t* b = N(g_4537c); b;) { uint8_t* nx = N(U(b, 8)); free(b); b = nx; }  // HeapFree(GetProcessHeap())
    g_4537c = 0;
    if (g_415f8 != 0xffffffffu) { DeleteCriticalSection(&g_42fe0); g_415f8 = 0xffffffffu; }
    return 1;
}
// Shutdown state, in order: handler head, chain head, flag, lock counter (the 0x2fab0 probe copies the pinned words in and out).
extern "C" void Storm_Sub2FAB0Pins(uint32_t** p) { p[0] = &g_45960; p[1] = &g_4537c; p[2] = &g_45380; p[3] = &g_415f8; }

// 1.13c Storm+0x34300: window procedure; 241D0 may handle the message, else DefWindowProcA.
intptr_t Storm_Sub34300(void* hwnd, uint32_t msg, uint32_t wp, intptr_t lp) {
    int32_t handled = 0;
    intptr_t res = 0;
    int32_t ok = Storm_Sub241D0((uint32_t)(uintptr_t)hwnd, msg, wp, lp, &handled, &res);  // hwnd: retail uint
    if (ok != 0 && handled != 0) return res;
    return W("DefWindowProcA", hwnd, msg, wp, lp);
}

// FN probe seam (src/core/fn/storm_26930.cpp): pins the two module words and installs the Win32 / DirectSound back end.
extern "C" void Storm_26930_FnSetup(uint32_t bytes, uint32_t src, intptr_t (*w)(const char*, const intptr_t*, int)) { g_43114 = bytes; g_43128 = src; g_win32 = w; }
