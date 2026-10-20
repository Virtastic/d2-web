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
};

// 1.13c D2Client+0x90CB0 (509 B): substitute "[mN]/[nN]" selectors of fmt (text before ':') from the vararg list, then emit the text after ':'.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_90cb0(W* out, const W* fmt, const W* const* args) {
    if (!fmt) return;
    W slot[10][200] = {}; int n = 0;
    if (args) for (; args[n] && n < 10; n++) wcpy(slot[n], args[n]);   // retail unbounded (stack overflow past 10)
    const W* colon = wchr(fmt, u':');
    if (!colon) wcpy(out, fmt);
    else {
        const W tag[5] = {u'[', u'm', u's', u']', 0};
        int m = -1, nn = -1;
        const W* p = wchr(fmt, u'm'); if (p && p < colon) m = p[1] - 0x30;
        p = wchr(fmt, u'n');          if (p && p < colon) nn = p[1] - 0x30;
        if (m < -1 || m > 9) m = -1;
        if (nn < -1 || nn > 9) nn = -1;
        if (m == -1) { if (nn != -1) wcpy(slot[nn], slot[nn] + 4); }
        else if (nn == -1) d2::client::sub_8E6D0((char16_t*)slot[m], tag);
        else {
            W t[5] = {}; W* dst = t; const W* src = tag;
            if (slot[nn][0] == 0x5b) { memcpy(t, slot[nn], 4 * sizeof(W)); src = slot[nn] + 4; dst = slot[nn]; }
            wcpy(dst, src);
            if (slot[m][0] == 0x5b) d2::client::sub_8E6D0((char16_t*)slot[m], t);
        }
        wcpy(out, colon + 1);
    }
    for (int i = 0; i < n; i++) d2::client::sub_8E600((char16_t*)out, (uint32_t)i, slot[i]);
}

// 1.13c D2Client+0x90EB0 (171 B): draw a name clipped to 0xA0 pixels.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_90eb0(G* g, Host h) {
    W buf[32] = {};
    H(h, ORD(10184));
    const W* s = S(H(h, 0xA5D90));
    for (int i = 0; i < 32 && s[i]; i++) buf[i] = s[i];
    buf[31] = 0;
    W* p = &buf[31];
    while ((int32_t)H(h, ORD(10028), P(buf)) > 0xa0 && p > buf) { --p; *p = 0; }
    H(h, ORD(10150), 0xd6 - g->cb9a4, 0, 0);
    H(h, ORD(10184));
}

// 1.13c D2Client+0x90F60 (101 B) fastcall(ecx=param_1).
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_90f60(G* g, Host h, uint32_t param_1) {
    if (!H(h, ORD(10227), param_1) || !g->c9854) return;
    H(h, 0x88A70, 0, 0, 0, 0);
    H(h, 0x143E0);
    uint32_t u; memcpy(&u, (const uint8_t*)(uintptr_t)g->bbfc + 0x60, 4);
    if (H(h, ORD(11139), u, 4)) H(h, 0x31CE0);
    if (H(h, ORD(11139), u, 5)) H(h, 0x31CE0);
}

// 1.13c D2Client+0x91070 (300 B)
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_91070(G* g, Host h, int32_t param_1) {
    if (g->be5c || g->be60) H(h, 0x88A70, 0, 0, 0, 0);
    g->be5c = 0; g->be60 = 0; g->be7c = 0; g->c418 = 0;
    if (param_1 == 0) {
        g->bc34 = 10; g->c414 = 0;
        H(h, ORD(10006)); H(h, 0x3A4D0, g->a9e14, 0, 0);
    } else {
        g->bb2a0 = 0; g->bc34 = 0xb; g->c414 = 3;
        H(h, ORD(10006)); H(h, 0x3A4D0, g->a9e14, 0, 0);
        g->c3dc = 0; g->c3e0 = 0; g->c3f8 = 0; g->c3fc = 0;
        if (!g->bb35c) {
            char buf[260]; snprintf(buf, sizeof buf, "%s\\ui\\panel\\trade", "DATA\\GLOBAL");
            g->bb35c = (uint32_t)H(h, 0xDB420, 0, P(buf));
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
    H(h, 0x3F430);
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
    H(h, 0x3F710, g->bc48);
    if (matched) g->bc4c = (int32_t)(H(h, 0xFFFF0001) + 500);   // GetTickCount()
}

// 1.13c D2Client+0x91360 (389 B): out = ESI (hidden), param_1 text; append the title-coloured suffix.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_91360(G* g, Host h, W* out, const W* param_1) {
    W tmp[64] = {}, line[256];
    const W* sep = S(H(h, ORD(10003)));
    wcpy(out, param_1);
    if (g->bc34 == 4 && H(h, ORD(10202), g->bc38, 0x400000, 0xccc, P(INV))) {
        memset(line, 0, sizeof line);
        wcpy(line, S(H(h, ORD(10003))));
        wcat(out, sep);
        wcat(out, S(H(h, 0x6F5F0, 1)));
    }
    if (H(h, 0x46230, g->bc38, P(tmp))) {
        if (tmp[0]) wcat(out, sep);
        wcat(out, tmp);
        return;
    }
    if (g->bc34 != 4) {
        memset(line, 0, 64 * sizeof(W));
        wcpy(line, S(H(h, ORD(10003))));
        wcat(out, sep);
        wcat(out, S(H(h, 0x6F5F0, 1)));
    }
}

// 1.13c D2Client+0x923F0 (77 B)
extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t d2_client_923f0(G* g, Host h) {
    if (g->c394 == 0 || g->c394 == 1) return 1;
    if (H(h, 0x59830)) { H(h, 0x595C0); return 0; }
    uint32_t now = (uint32_t)H(h, 0xFFFF0001);
    if (now - g->bb2a0 > 0xfa) { g->bb2a0 = (uint32_t)H(h, 0xFFFF0001); return 1; }
    return 0;
}

// Shared by 92440/92650: `a` is the tooltip text, finished and drawn.
static void finish_text(G* g, Host h, W* a, W* b, W* o, uintptr_t rect, int32_t esi, bool is92650) {
    H(h, 0x914F0, g->bc38, P(b));
    wcat(a, b);
    if (g->bc34 < 1 || g->bc34 > 9) wcpy(o, a);
    else H(h, 0x91360, P(o), P(a));
    H(h, ORD(10177), rect, P(o));
    if (!is92650) { H(h, ORD(10085), g->e90eac < 1 ? g->e90eb4 : g->e90eac, 0, 0); return; }
    if (g->e90eac != esi && g->e90eac - esi >= 0) H(h, ORD(10085), g->e90eac, 0, 0);
    else H(h, ORD(10085), g->e90eb4 + esi, 0, 0);
}

// 1.13c D2Client+0x92440 (528 B)
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_92440(G* g, Host h) {
    static W a[1024], b[128], o[1024]; memset(a, 0, sizeof a);
    const W* sep = S(H(h, ORD(10003)));
    if (!H(h, ORD(10661), g->bc38)) return;
    uint32_t id = g->bc38 ? ((const uint32_t*)(uintptr_t)g->bc38)[1] : 0xffffffff;
    uintptr_t rec = H(h, ORD(10695), id);
    if (!rec) fatal(h, 0xe61);
    uint32_t rect[8] = {};
    if (!H(h, ORD(10450), ((const uint32_t*)rec)[50], P(rect))) fatal(h, 0xe64);
    wcpy(a, S(H(h, ORD(10003))));
    wcpy(a, S(H(h, ORD(10003))));
    wcat(a, sep);
    wcat(a, S(H(h, ORD(10003))));
    wcat(a, sep);
    memset(b, 0, sizeof b); memset(o, 0, sizeof o);
    finish_text(g, h, a, b, o, P(rect), 0, false);
}

// 1.13c D2Client+0x92650 (499 B); esi = hidden ESI.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_92650(G* g, Host h, int32_t esi) {
    static W a[1024], b[128], o[1024], z[512]; memset(a, 0, sizeof a); memset(z, 0, sizeof z);
    const W* sep = S(H(h, ORD(10003)));
    if (!g->bc38 || H(h, ORD(11088), g->bc38) != 0x12) fatal(h, 0xcf1);
    uintptr_t rec = H(h, ORD(10695), ((const uint32_t*)(uintptr_t)g->bc38)[1]);
    if (!rec) fatal(h, 0xcf5);
    H(h, 0x8D880, g->bc38);
    if (g->bc34 == 0) {
        wcat(a, S(H(h, ORD(10003)))); wcat(a, sep);
        wcat(a, S(H(h, ORD(10003)))); wcat(a, sep);
    }
    memset(b, 0, sizeof b); memset(o, 0, sizeof o);
    uint32_t rect[2] = {};
    finish_text(g, h, a, b, o, P(rect), esi, true);
}

// 1.13c D2Client+0x92850 (106 B)
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_92850(G* g, Host h) {
    uint8_t pkt[7] = {0x5a, 0x0e, 0x01, 0, 0, 0, 0};
    g->bc34 = 1;
    g->b834 = (int32_t)H(h, ORD(11017));
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

// 1.13c D2Client+0x914F0 (698 B): item tooltip name/description line into `out` (== param_2, the retail buffer pointer).
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_914f0(G* g, Host h, uint32_t item, W* out) {
    if (!item) return;
    static W n1[512], n2[512], t1[512], t2[512], r[512];
    const W* sep = S(H(h, ORD(10003)));
    H(h, ORD(10003));
    uintptr_t rec = H(h, ORD(10695), u32(item, 4));
    if (!rec) fatal(h, 0x732);
    H(h, ORD(11088), item);
    auto body = [&]() {
        if (H(h, ORD(10202), item, 0x4000000, 0x735, P(INV))) {
            memset(t1, 0, sizeof t1); memset(t2, 0, sizeof t2); memset(r, 0, sizeof r);
            wcpy(t1, S(H(h, ORD(10003))));
            wcpy(t2, S(H(h, 0x3C720))); wcat(t2, sep);
            H(h, ORD(10257), item, 0);
            const W* s = S(H(h, ORD(10003))); if (!s) s = sep;
            wcpy(r, s);
            color(h, out, 4); wcat(out, r);
            return;
        }
        if (!H(h, ORD(10202), item, 0x10, 0x748, P(INV))) {
            memset(t1, 0, sizeof t1);
            wcpy(t1, S(H(h, ORD(10003)))); H(h, 0x3C720); wcpy(out, t1);
            return;
        }
        const W* a0 = nullptr;                 // first vararg of FUN_6fb40cb0
        uint32_t k = (uint32_t)H(h, ORD(10106), item);
        switch (k) {
        case 1: {
            uint32_t v = (uint32_t)H(h, ORD(10620), item);
            if (!H(h, ORD(10961), v)) return;
            uintptr_t a = H(h, ORD(10003), 0);
            a0 = S(H(h, ORD(10003), a));
            break; }
        case 2:
            if (H(h, ORD(10744), item, 0x16) || H(h, ORD(10744), item, 0x12)) {
                int16_t q = (int16_t)H(h, ORD(11028), item);
                if (q == 0 || q == 1) wcpy(out, S(H(h, ORD(10003))));
                return;
            }
            if (H(h, ORD(10744), item, 7)) {
                char num[3]; W nm[256] = {}, pn[100] = {};
                snprintf(num, sizeof num, "%d", (int)H(h, ORD(10588), item)); widen(nm, num, 3);
                const char* name = (const char*)H(h, ORD(10083), item);
                static W pname[256]; widen(pname, name ? name : "", 256);
                if (!H(h, ORD(10202), item, 0x8000, 0x775, P(INV))) wcpy(out, S(H(h, ORD(10003))));
                else { wcpy(out, S(H(h, ORD(10003)))); wcat(out, sep); wcat(out, S(H(h, ORD(10003)))); }
                H(h, ORD(10620), item); H(h, 0x3C210);
                H(h, ORD(10003));
                H(h, PERSONALIZE, P(pn), P(pname), H(h, ORD(10009)));   // scratch r, size 100 implied
                return;
            }
            if (!H(h, ORD(10744), item, 0x28)) {
                uintptr_t sub = u32(item, 0x60);
                bool ok = H(h, ORD(10202), item, 0x800, 0x799, P(INV)) && sub && H(h, ORD(10508), sub);
                if (ok) {
                    uintptr_t a = H(h, ORD(10003), 0), b = H(h, ORD(10003), a);
                    H(h, 0x40CB0, P(out), H(h, ORD(10003), b), item);   // vararg is the item unit, not a string
                    return;
                }
                memset(t1, 0, sizeof t1);
                wcpy(t1, S(H(h, ORD(10003)))); H(h, 0x3C720); wcpy(out, t1);
                return;
            }
            H(h, ORD(10620), item); H(h, 0x1230);
            { uintptr_t a = H(h, ORD(10003), 0); a0 = S(H(h, ORD(10003), a)); }
            break;
        case 3: { uintptr_t a = H(h, ORD(10003), 0); a0 = S(H(h, ORD(10003), a)); break; }
        case 4: {
            uint16_t id1 = (uint16_t)H(h, ORD(10257), item), id2 = (uint16_t)H(h, ORD(11028), item, 0);
            const char* e1 = (const char*)H(h, ORD(10248), id1); const char* e2 = (const char*)H(h, ORD(10248), id2);
            entry_name(h, n1, e1, e1 ? u16((uintptr_t)e1, 0x20) : 0);
            entry_name(h, n2, e2, e2 ? u16((uintptr_t)e2, 0x20) : 0);
            if ((uintptr_t)out <= wlen(n1) + wlen(n2) + wlen(S(H(h, ORD(10003)))) + 3) fatal(h, 0x8c5);
            const W* args[3] = {n1, n2, nullptr};
            H(h, ORD(10003), 0, 0);
            const W* fmt = S(H(h, ORD(10003), 0));
            d2_client_90cb0(out, fmt, args);
            return; }
        case 5: {
            H(h, ORD(10620), item);
            uintptr_t x = H(h, 0x1870);
            if (!x) return;
            memset(t1, 0, sizeof t1); wcpy(t1, S(H(h, ORD(10003))));
            wcpy(out, S(H(h, 0x3C720))); wcat(out, sep);
            if (u16(x, 0x24)) {
                memset(t2, 0, sizeof t2); memset(r, 0, sizeof r);
                wcpy(r, S(H(h, ORD(10003))));
                if (!H(h, 0x3DFB0, P(t2))) {
                    static W tmp[100]; tmp[0] = 0;
                    const W* args[2] = {r, nullptr};
                    d2_client_90cb0(tmp, S(H(h, ORD(10003), P(r), 0)), args);
                    wcat(out, tmp);
                } else wcat(out, t2);
            }
            return; }
        case 6: case 8: case 9: {
            uint16_t id1 = (uint16_t)H(h, ORD(10380), item), id2 = (uint16_t)H(h, ORD(10433), item);
            uintptr_t e1 = H(h, ORD(10365), id1), e2 = H(h, ORD(10365), id2);
            entry_name(h, n1, e1 ? (const char*)e1 + 0x26 : nullptr, e1 ? u16(e1, 0xc) : 0);
            entry_name(h, n2, e2 ? (const char*)e2 + 0x26 : nullptr, e2 ? u16(e2, 0xc) : 0);
            if ((uintptr_t)out <= wlen(n1) + wlen(n2) + wlen(S(H(h, ORD(10003)))) + 3) fatal(h, 0x822);
            memset(t1, 0, sizeof t1); memset(t2, 0, sizeof t2); memset(r, 0, sizeof r);
            wcpy(t1, S(H(h, ORD(10003))));
            wcpy(t2, S(H(h, 0x3C720))); wcat(t2, sep);
            static W tmp[200]; tmp[0] = 0;
            const W* args[3] = {n1, n2, nullptr};
            d2_client_90cb0(tmp, S(H(h, ORD(10003), 0, 0)), args);
            if (!H(h, 0x3DFB0, P(r))) wcat(t2, tmp); else wcat(t2, r);
            wcat(out, t2);
            return; }
        case 7: {
            memset(t1, 0, sizeof t1); wcpy(t1, S(H(h, ORD(10003))));
            if ((int32_t)H(h, ORD(10620), item) >= 0) {
                H(h, ORD(10620), item);
                if (H(h, 0x18A0)) {
                    if (((const uint8_t*)rec)[0x144] == 0) { wcpy(out, S(H(h, 0x3C720))); wcat(out, sep); }
                    memset(t2, 0, sizeof t2);
                    H(h, ORD(10003)); H(h, 0x3C720);
                    if (!H(h, 0x3DFB0, P(t2))) { H(h, ORD(10003)); wcat(out, S(H(h, 0x3C720))); }
                    else wcat(out, t2);
                    return;
                }
            }
            H(h, 0x3C720); wcpy(out, t1);
            return; }
        default: fatal(h, 0x8ce);
        }
        const W* args[2] = {a0, nullptr};
        d2_client_90cb0(out, S(H(h, ORD(10003))), args);
    };
    body();
    H(h, 0x3E080, P(out));
    uint8_t need = g->cc390;
    if (((const uint8_t*)rec)[0x12a]) {
        if (((const uint8_t*)rec)[0x12b] && (int32_t)H(h, ORD(10973), item, 0x164, 0) < (int32_t)need) { color(h, out, 1); return; }
        if (u32(rec, 0x80) != 0x2067656c) color(h, out, 4);
    }
}

// 1.13c D2Client+0x928C0 (2836 B): build and draw the hovered-item tooltip. ebx/ebp/ref_esi are hidden registers.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_928c0(G* g, Host h, const uint32_t* unit, int32_t ebx, int32_t ebp, int32_t ref_esi) {
    static W desc[1024], t3c64[400], b3e98[512], b3898[512], b40a0[512], b3aa0[512], b42a8[512], b4ea8[512],
             b2cc8[712], b1870[512], b4cc8[512], b1070[1024], b2070[512], tmp[64], sock[512];
    memset(desc, 0, sizeof desc); memset(b1870, 0, sizeof b1870); memset(b4cc8, 0, sizeof b4cc8);
    memset(b4ea8, 0, sizeof b4ea8); memset(b2070, 0, sizeof b2070);
    const W* sep = S(H(h, ORD(10003)));
    const uint32_t* who = unit;
    if (unit && unit[0] == 1 && H(h, ORD(11104)) == 0) who = (const uint32_t*)(uintptr_t)g->bbfc;
    int32_t w = (int32_t)unit[0x18];
    if (!((g->bc2c || g->bc28) && g->bc38)) return;
    if (H(h, ORD(11017)) != 0) return;
    if (!H(h, ORD(10695))) { H(h, ORD(10265)); H(h, ORD(10024)); std::_Exit(-1); }
    H(h, ORD(10620));
    if (!H(h, 0x1870) || !H(h, 0x1840)) return;
    wcpy(t3c64, S(H(h, ORD(10003)))); wcat(t3c64, sep);
    uint8_t c0 = 0, cA = 0, cB = 0;
    H(h, ORD(10244));
    H(h, 0x40770, g->bc38);
    int32_t n = (int32_t)H(h, ORD(11015));
    if (n > 1) { if (ebx == 0) w = 1; H(h, 0x3DE40, n); }
    if (H(h, ORD(10744), g->bc38, 0x2d) || H(h, ORD(10744), g->bc38, 0x32)) {
        int32_t a = 0, b = 0;
        if (H(h, ORD(10910), g->bc38, 0x5b, 0)) {
            a = (int32_t)H(h, 0x14A0, 100, u16((uintptr_t)who, 0x10a));
            b = (int32_t)H(h, 0x14A0, 100, u16((uintptr_t)who, 0x10c));
        }
        if (H(h, ORD(10202), g->bc38, 0x400000, 0xd88, P(INV))) { a -= 10; b -= 10; }
        if (u16((uintptr_t)who, 0x10a)) { if (w == 0) cA = 1; H(h, 0x3DD80, 0, a); }
        if (u16((uintptr_t)who, 0x10c)) { if (ebp == 0) cB = 1; H(h, 0x3DCC0, 0, b); }
    }
    color(h, desc, c0); color(h, desc, cA); color(h, desc, cB);
    int32_t kind = (int32_t)H(h, ORD(10822), g->bc38);
    if (kind != 7) {
        wcat(desc, S(H(h, ORD(10003)))); wcat(desc, sep);
        int32_t e = (who && who[0] == 0 && who[1] != (uint32_t)kind) ? 1 : w;
        color(h, desc, (uint8_t)e);
    }
    color(h, desc, 0);
    if (H(h, ORD(10744), g->bc38, 0x2d)) {
        H(h, 0x3F9E0, P(b3e98)); color(h, desc, 0); wcat(desc, b3e98);
        if ((int32_t)H(h, ORD(10823), g->bc38, 0) >= 0 && (int32_t)H(h, ORD(10925), g->bc38, 0) >= 0) {
            H(h, 0x3FF20, g->bc38, P(b3898)); color(h, desc, 0); wcat(desc, b3898);
        }
    }
    if (H(h, ORD(10744), g->bc38, 0x33)) {
        if (who && who[0] == 0 && who[1] == 3 && (H(h, ORD(10089), g->bc38) == 0 || H(h, ORD(10822), g->bc38) == 3)) {
            H(h, 0x3D940, g->bc38, 0x2d, P(b40a0)); color(h, desc, 0); wcat(desc, b40a0);
        }
        H(h, 0x3FDC0, P(b3aa0)); color(h, desc, 0); wcat(desc, b3aa0);
    }
    if (H(h, ORD(10744), g->bc38, 0x32) && (int32_t)H(h, ORD(10935), g->bc38) > 0) {
        H(h, 0x3FB80, P(who), P(b42a8)); color(h, desc, 0); wcat(desc, b42a8);
    }
    H(h, 0x914F0, g->bc38, P(b4ea8));
    color(h, desc, H(h, ORD(10202), g->bc38, 0x100, 0xddb, P(INV)) ? 1 : 2);
    wcat(desc, b4ea8);
    if (H(h, ORD(10202), g->bc38, 0x800, 0xde4, P(INV))) {
        memset(b2cc8, 0, sizeof b2cc8);
        H(h, 0x3E180, g->bc38, P(b2cc8));
        wcpy(b1870, b2cc8);
    }
    H(h, 0x60B0, g->bc38, 1, 0);
    b2070[0] = 0;
    H(h, 0x5FB0, P(b2070), 0x200);
    sock[0] = 0;
    if (g->bc38 && u32(g->bc38, 0x10) == 1) H(h, 0x5F10);
    // GUESSED: Ghidra's pcVar9 is the item-data record (unit word 5): count at +0xc, socket units from +0x110.
    uintptr_t idata = g->bc38 ? u32(g->bc38, 0x14) : 0;
    if (idata) {
        int32_t cnt = (int32_t)u32(idata, 0xc);
        for (int32_t i = 0; i < cnt; i++) {
            uintptr_t it = u32(idata, 0x110 + 4 * i);
            if (!it) break;
            int32_t r = (int32_t)H(h, 0x3D430, it);
            if (u16(it, 0x24)) {
                uintptr_t a = H(h, ORD(10003), 0);
                const W* args[2] = {S(a), nullptr};
                d2_client_90cb0(b4cc8, S(H(h, ORD(10003))), args);
                wcat(b4cc8, sep);
            }
            color(h, desc, r ? 2 : 1);
            wcat(b1870, b4cc8);
        }
    }
    wcpy(b4cc8, S(H(h, 0x6F5F0, 2))); color(h, b4cc8, 4);
    if (sock[0]) { wcat(b4cc8, sep); color(h, b4cc8, 4); }
    wcat(b4cc8, sep);
    if (b2070[0]) color(h, b4cc8, 2);
    color(h, b4cc8, 3);
    wcat(b4cc8, desc);
    if (g->bc34 <= 0 || g->bc34 > 9) wcpy(b1070, b4cc8);
    else {
        memset(tmp, 0, sizeof tmp);
        if (!H(h, 0x46230, g->bc38, P(tmp))) {
            wcpy(b1070, b4cc8);
            if (g->bc34 != 4) { W line[64] = {}; wcpy(line, S(H(h, ORD(10003)))); wcat(b1070, sep); color(h, b1070, 1); }
        } else {
            wcpy(b1070, b4cc8);
            if (tmp[0]) wcat(b1070, sep);
            wcat(b1070, tmp);
        }
    }
    uint32_t rect[8] = {};
    H(h, ORD(10177), P(rect), P(b1070));
    int32_t y = g->e90eac;
    if (y == ref_esi || y - ref_esi < 0) y = ref_esi + g->e90eb4;
    H(h, ORD(10085), y, 0, 1);
}
