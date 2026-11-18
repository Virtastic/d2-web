// 1.13c D2Client+0x42C70 -- the player-name / title banner: draws the player's name centred at (half, 0xBB, 4) and, for
// cc390 in 0..2, the D2Win title (Ordinal_10205) + name at (0x115|0x165, 4, 0).
// player = [11B980]; cc390 = byte [11C390]; g991c = byte [11991C]; g9854 = [119854]; g8bc48 = [DBC48].
// Callees through D2cHost: IAT CEB00 (Unicode ctor), CEAC4 (Unicode::toUnicode, ecx = dst, edx = src, stack 0x40), D35A (ecx -> width),
// D37E (ecx = text, edx = x, stack id/4/0, ret 0xC), D414 (stack bl, kind, flags&4, flags&0x20 -> title string or 0).
#include <cstdint>
#include <emscripten.h>
#include "client_host.h"
typedef int32_t i32; typedef uint32_t u32;
namespace {
const u32 STK = 0xB3000000u;   // a stack address in retail
inline i32 half(i32 v) { return (v + (v < 0)) >> 1; }   // cdq; sub; sar 1
}
extern "C" {
EMSCRIPTEN_KEEPALIVE void d2c_42c70(D2cHost h, const uint8_t* player, u32 cc390, u32 g991c, u32 g9854, i32 g8bc48, const char* titleStr) {   // titleStr = the string D414 returned (a host pointer; read only when it returned nonzero)
    auto ctors = [&]() { for (int i = 0; i < 100; ++i) { D2cCall c = {}; c.rva = 0xCEB00; c.ecx = STK; h(&c); } };
    auto toU = [&]() { D2cCall c = {}; c.rva = 0xCEAC4; c.ecx = STK; c.edx = STK; c.s[0] = 0x40; h(&c); };
    auto width = [&]() { D2cCall c = {}; c.rva = 0xD35A; c.ecx = STK; return (i32)h(&c); };
    auto draw = [&](u32 id, i32 x, u32 s1, u32 s2) { D2cCall c = {}; c.rva = 0xD37E; c.ecx = STK; c.edx = (u32)x; c.s[0] = id; c.s[1] = s1; c.s[2] = s2; h(&c); };
    char name[256], title[256];
    ctors();
    { const char* p = (const char*)(player + 0xB9); char* d = name; do *d++ = *p; while (*p++); }
    toU();
    i32 w1 = width();
    draw(0xBB, half(g8bc48) - half(w1), 4, 0);
    const u32 flags = *(const uint16_t*)(player + 0x1EB);
    const u32 a = flags & 4, fl = flags & 0x20;
    ctors();
    u32 kind;
    if (cc390 == 0) kind = 4 + (fl != 0);
    else if (cc390 == 1) kind = 8 + (fl != 0) * 2;
    else if (cc390 == 2) kind = 12 + (fl ? 3 : 0);
    else return;
    D2cCall t = {}; t.rva = 0xD414; t.s[0] = g991c & 0xFF; t.s[1] = kind; t.s[2] = a; t.s[3] = fl;
    if (h(&t) == 0) return;
    { const char* p = titleStr; char* d = title; do *d++ = *p; while (*p++); }          // title -> local B
    { char* e = title; while (*e) ++e; const char* p = name; do *e++ = *p; while (*p++); }   // B += name (rep movs of name incl. NUL)
    toU();
    i32 w2 = width();
    draw(g9854 ? 0x165 : 0x115, half(g8bc48) - half(w2), 4, 0);
}
}
