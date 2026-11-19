// src/core/d2client/d2client_batch_90cb0.cpp -- 12 UNMAPPED 1.13c D2Client.dll rows (alpha lane), from
// build-wasm/ghidra-d2client/decomp/0x<RVA>.c. Base 0x6FAB0000. Every global is a field of G (named by its
// retail address) and every unrecovered callee goes through Host, keyed by RVA (DLL ordinals are 0x100000+N),
// called in retail order with the visible arguments. Wide strings are native UCS-2 (char16_t); Unicode::
// strcpy/strcat/strchr/strncpy are done natively.
// GUESSED (decompile shows hidden registers / lost return values): 90CB0 varargs = null-terminated array;
// 91360 out = ESI; 92650 esi = hidden ESI compare operand; 928C0 ebx/ebp = hidden flag registers; in 914F0 and
// 928C0 Ghidra split one stack buffer into overlapping names (aUStack_*), kept here as separate buffers, and
// color-code calls whose result Ghidra dropped (FUN_6fb6f5f0 -> strcat) are appended to the output; varargs of
// FUN_6fb40cb0 beyond the first are not visible and are rebuilt from the name buffers.
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <emscripten.h>
#include "client_string_algorithms.h"

typedef uintptr_t (*Host)(uint32_t id, uintptr_t a, uintptr_t b, uintptr_t c, uintptr_t d);
namespace {
using W = char16_t;
inline uintptr_t H(Host h, uint32_t id, uintptr_t a = 0, uintptr_t b = 0, uintptr_t c = 0, uintptr_t d = 0) { return h(id, a, b, c, d); }
inline uintptr_t P(const void* p) { return (uintptr_t)p; }
constexpr uint32_t ORD(uint32_t n) { return 0x100000 + n; }
const char* const INV = "..\\Source\\D2Client\\UI\\inv.cpp";
void wcpy(W* d, const W* s) { while ((*d++ = *s++)) {} }
void wcat(W* d, const W* s) { while (*d) d++; wcpy(d, s); }
size_t wlen(const W* s) { size_t n = 0; while (s[n]) n++; return n; }
const W* wchr(const W* s, W c) { for (; *s; s++) if (*s == c) return s; return nullptr; }
const W* S(uintptr_t v) { return (const W*)v; }
// Unicode::strcat of the string returned by the color-code helper FUN_6fb6f5f0(c).
void color(Host h, W* d, uint8_t c) { wcat(d, S(H(h, 0x6F5F0, c))); }
[[noreturn]] void fatal(Host h, uint32_t code) { uintptr_t s = H(h, ORD(10265), code); H(h, ORD(10024), 0, s); std::_Exit(-1); }
}

// Globals (6fbxxxxx): current-item / UI state read and written by this batch.
struct G {
    int32_t  bc34;      // 6fbcbc34 ui mode
    uint32_t bc38;      // 6fbcbc38 hovered item unit
    uint32_t bbfc;      // 6fbcbbfc player unit
    int32_t  bc2c, bc28, bc30, bc48, bc88, bc4c;
    int32_t  be5c, be60, be7c, be64, be68, be6c, be70, beec, c418, c414, c3dc, c3e0, c3f8, c3fc, c394;
    uint32_t bb2a0, bb348, bb35c, a9e14;
    int32_t  b834, b854, b858, b864;       // 6fbcb834/54/58/64 (92850 trade/dialog state)
    int32_t  c9854, c9721, c971d;
    int32_t  e90eac, e90eb4;               // 6fb90eac / 6fb90eb4 text y
    int32_t  cb9a4;                        // 6fbcb9a4
    uint8_t  cc390;                        // 6fbcc390
    uint32_t* hash_ba808;                  // 6fbba808[128] unit buckets (next = word 0x39)
    uint8_t* tab_91182; int32_t tab_n;     // 6fb91182 stride 0x12 table, count 6fb911c0
    uint32_t a9e18, cc3c4;                 // 6fba9e18 / 6fbcc3c4 (EAX, ESI of the 91070 0x8A4D0 call)
};

// 1.13c D2Client+0x90EB0: moved to d2client_90eb0.cpp (the name id, font restore and draw position passed as retail does).

// 1.13c D2Client+0x90F60: moved to d2client_90f60.cpp (register and stack arguments passed explicitly).

// 1.13c D2Client+0x91070 (300 B)
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_91070(G* g, Host h, int32_t param_1) {
    if (g->be5c || g->be60) H(h, 0x88A70, 0, 0, 0, 0);
    g->be5c = 0; g->be60 = 0; g->be7c = 0; g->c418 = 0;
    if (param_1 == 0) {
        g->bc34 = 10; g->c414 = 0;
        H(h, ORD(10006)); H(h, 0x8A4D0, g->a9e14, g->a9e18, g->cc3c4, 0);   // ECX, EAX, ESI, stack 0; EDX 0
    } else {
        g->bb2a0 = 0; g->bc34 = 0xb; g->c414 = 3;
        H(h, ORD(10006)); H(h, 0x8A4D0, g->a9e14, g->a9e18, g->cc3c4, 0);   // ECX, EAX, ESI, stack 0; EDX 0
        g->c3dc = 0; g->c3e0 = 0; g->c3f8 = 0; g->c3fc = 0;
        if (!g->bb35c) {
            char buf[260]; snprintf(buf, sizeof buf, "%s\\ui\\panel\\trade", "DATA\\GLOBAL");
            g->bb35c = (uint32_t)H(h, 0x2B420, 0, P(buf));
        }
    }
    g->beec = 0; g->be64 = 0; g->be68 = 0; g->be6c = 0; g->be70 = 0;
}

// 1.13c D2Client+0x911A0 (312 B)
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_911a0(G* g, Host h) {
    g->be7c = 0; g->bc88 = -1; g->bc48 = 0; g->bb348 = 0;
    for (int i = 0; i < g->tab_n; i++) memset(g->tab_91182 + 0x12 * i, 0, 8);   // words at -4/+0 of the 6fb91186 pointer
    g->bc30 = 0; g->bc34 = 1;
    H(h, 0x8F430);
    if (!g->c9721) return;
    const uint32_t* node = nullptr;
    for (uintptr_t n = g->hash_ba808[g->c971d & 0x7f]; n; n = ((const uint32_t*)n)[0x39])
        if (((const uint32_t*)n)[3] == (uint32_t)g->c971d) {
            if (((const uint32_t*)n)[0] != 1) fatal(h, 0x5a);
            node = (const uint32_t*)n; break;
        }
    bool matched = false;
    if (node) {
        uint32_t k = node[1];
        switch (k) {
        case 0x93: case 0x200: g->bc48 = 0; matched = true; break;
        case 0x9a: case 0x1ff: g->bc48 = 1; matched = true; break;
        case 0xff: case 0x94: case 0xb1: case 0xb2: case 0xca: case 0xfc: case 0xfd:
        case 0x101: case 0x195: case 0x201: g->bc48 = 3; matched = true; break;
        default: break;
        }
    }
    H(h, 0x8F710, g->bc48);
    if (matched) g->bc4c = (int32_t)(H(h, 0xFFFF0001) + 500);   // GetTickCount()
}

// 1.13c D2Client+0x91360: moved to d2client_91360.cpp (ESI out, stack text, EDI for the colour helper as retail passes them).

// 1.13c D2Client+0x923F0 (77 B)
extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t d2_client_923f0(G* g, Host h) {
    if (g->c394 == 0 || g->c394 == 1) return 1;
    if (H(h, 0x59830)) { H(h, 0x595C0); return 0; }
    uint32_t now = (uint32_t)H(h, 0xFFFF0001);
    if (now - g->bb2a0 > 0xfa) { g->bb2a0 = (uint32_t)H(h, 0xFFFF0001); return 1; }
    return 0;
}

// 1.13c D2Client+0x92440: moved to d2client_92440.cpp (registers and stack words as retail passes them).

// 1.13c D2Client+0x92650: moved to d2client_92650.cpp (no hidden ESI: the y compare uses the text height 0xD3B4 returns).

// 1.13c D2Client+0x92850 (106 B)
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_92850(G* g, Host h) {
    uint8_t pkt[7] = {0x5a, 0x0e, 0x01, 0, 0, 0, 0};
    g->bc34 = 1;
    g->b834 = (int32_t)H(h, ORD(11017), ((const uint32_t*)(uintptr_t)g->bbfc)[0x18]);   // player+0x60
    g->b858 = 0; g->b854 = 5; g->b864 = g->b834 ? 4 : 1;
    H(h, 0x7DFB0, P(pkt));
}

namespace {
const uint32_t PERSONALIZE = 0x200001;   // Unicode::Personalize(dst, name, scratch, 100, lang)
uint16_t u16(uintptr_t p, uint32_t o) { uint16_t v; memcpy(&v, (const uint8_t*)p + o, 2); return v; }
uint32_t u32(uintptr_t p, uint32_t o) { uint32_t v; memcpy(&v, (const uint8_t*)p + o, 4); return v; }
// Unicode::toUnicode/win2Unicode on ASCII text.
void widen(W* d, const char* s, size_t cap) { size_t i = 0; for (; s[i] && i + 1 < cap; i++) d[i] = (uint8_t)s[i]; d[i] = 0; }
// "(" name ")" when the table entry has no localized string id, else the localized string.
void entry_name(Host h, W* d, const char* e, uint16_t locId) {
    if (!e) { d[0] = 0; return; }
    if (locId == 0) { char n[512]; snprintf(n, sizeof n, "(%s)", e); widen(d, n, 512); }
    else wcpy(d, S(H(h, ORD(10003))));
}
}
