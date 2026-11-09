// 1.13c D2Client+0x6FCE0: Battle.net chat message pump. int(void): 0 when there is no source object or its fetch fails, else 1.
// The source object is *( *(0x11B980) + 0x221 ); its +0x24 field is a fastcall fetch(ECX = &handled, EDX = &msg) that fills the
// caller's stack. msg: +0 type, +0x18 sender name (NUL), then the body (NUL). Types 4/6/10 (whispers) print
// "name (*account)" (or just the name when the account part repeats it) in front of the body through string 0x14D4/0x14D5;
// types 1,2,3,5,7,9,21,22,23 are consumed silently; every other type prints "<BattleNet>: " + body, the body cut so the joined
// text fits 0x154 chars. The indirect fetch goes through e.H keyed by the pointer value it reads (a recorder rva under capture).
#include <cstdint>
#include "cl_env.h"
namespace {
constexpr uint32_t G(uint32_t rva) { return 0x6FAB0000u + rva; }
inline uint32_t P(const volatile void* p) { return (uint32_t)(uintptr_t)p; }
constexpr uint32_t kUtf8 = 0xCEAE0, kSprintf = 0xCEAB4, kStrlen = 0xCEAB0, kStrcat = 0xCEABC;
constexpr uint32_t kMsg = 0x1EC, kCap = 0x154;
const char* tail(const char* s) {                       // 0x6EBB0: the text after the first '*', or s when there is none
    for (const char* p = s; *p; ++p) if (*p == '*') return p[1] ? p + 1 : s;
    return s;
}
bool hasStar(const char* s) { for (; *s; ++s) if (*s == '*') return true; return false; }
uint32_t append(volatile char* d, uint32_t n, const char* s) { while (*s && n < 0x14F) d[n++] = *s++; return n; }
}
extern "C" uint32_t d2c_6fce0(const cl::Env& e) {
    const uint32_t obj = e.R(e.R(G(0x11B980)) + 0x221);
    if (!obj) return 0;
    volatile uint32_t handled = 0, msg[kMsg / 4] = {};
    if (!e.H(e.R(obj + 0x24), 0, 0, 0, 0, 0, 0, P(&handled), P(msg))) return 0;
    if (handled) return 1;
    volatile uint16_t b1[kCap] = {}, b2[kCap] = {}, b3[kCap] = {};   // Unicode ctor: each char := 0
    char m[kMsg + 1] = {};
    for (uint32_t i = 0; i < kMsg; ++i) m[i] = (char)((const volatile uint8_t*)msg)[i];
    const uint32_t type = (uint32_t)(uint8_t)m[0] | (uint32_t)(uint8_t)m[1] << 8 | (uint32_t)(uint8_t)m[2] << 16 | (uint32_t)(uint8_t)m[3] << 24;
    const char* name = m + 0x18;
    uint32_t nlen = 0; while (0x18 + nlen < kMsg && name[nlen]) ++nlen;
    const uint32_t body = P(msg) + 0x18 + nlen + 1;
    static const uint8_t kPath[23] = { 0, 0, 0, 1, 0, 1, 0, 2, 0, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0 };
    const uint32_t path = type - 1 <= 0x16 ? kPath[type - 1] : 2;
    if (path == 0) return 1;
    if (path == 1) {
        char out[kCap] = {};                                 // 0x6EB70: the name up to its first '*' (whole when it leads or is absent)
        uint32_t k = 0;
        if (name[0] == '*' || !hasStar(name)) { while (name[k] && k < kCap - 1) { out[k] = name[k]; ++k; } }
        else { while (name[k] != '*' && k < kCap - 1) { out[k] = name[k]; ++k; } }
        volatile char cb[0x150] = {};
        uint32_t n = append(cb, 0, out);
        bool same = out[0] == 0;
        if (!same && out[0] == '*') { const char* a = tail(name); const char* b = tail(out); while (*a && *a == *b) { ++a; ++b; } same = *a == *b; }
        if (!same) { n = append(cb, n, " (*"); n = append(cb, n, tail(name)); append(cb, n, ")"); }
        e.H(kUtf8, kCap, 0, 0, 0, 0, 0, P(b1), P(cb)); if (e.blk) e.blk(e.ctx, kUtf8, P(cb), 16);
        e.H(kUtf8, kCap, 0, 0, 0, 0, 0, P(b2), body); if (e.blk) e.blk(e.ctx, kUtf8, body, 16);
        b1[kCap - 1] = 0; b2[kCap - 1] = 0;
        const uint32_t fmt = e.H(0xD1EC, 0, 0, 0, 0, 0, 0, type == 10 ? 0x14D5u : 0x14D4u);
        volatile uint32_t a[5] = { kCap - 1, P(b3), fmt, P(b1), P(b2) };
        e.H(kSprintf, a[0], a[1], a[2], a[3], a[4]); if (e.blk) e.blk(e.ctx, kSprintf, P(a), 5);
        b3[kCap - 1] = 0;
        e.H(0x7D850, P(b3), 2); if (e.blk) e.blk(e.ctx, 0x7D850, P(b3), 8);
        return 1;
    }
    volatile char bn[16] = { '<', 'B', 'a', 't', 't', 'l', 'e', 'N', 'e', 't', '>', ':', ' ', 0 };
    e.H(kUtf8, kCap, 0, 0, 0, 0, 0, P(b1), P(bn)); if (e.blk) e.blk(e.ctx, kUtf8, P(bn), 3);
    e.H(kUtf8, kCap, 0, 0, 0, 0, 0, P(b2), body); if (e.blk) e.blk(e.ctx, kUtf8, body, 3);
    b1[kCap - 1] = 0; b2[kCap - 1] = 0;
    const int32_t l2 = (int32_t)e.H(kStrlen, 0, 0, 0, 0, 0, 0, P(b2));
    const int32_t l1 = (int32_t)e.H(kStrlen, 0, 0, 0, 0, 0, 0, P(b1));
    if (l1 + l2 >= (int32_t)kCap) {
        const int32_t l = (int32_t)e.H(kStrlen, 0, 0, 0, 0, 0, 0, P(b1));
        if (l < (int32_t)kCap - 1) { if (l >= 0) b2[kCap - 1 - l] = 0; }   // retail indexes unchecked; strlen is never negative
        else b2[0] = 0;
    }
    e.H(kStrcat, 0, 0, 0, 0, 0, 0, P(b1), P(b2)); if (e.blk) e.blk(e.ctx, kStrcat, P(b2), 8);
    b1[kCap - 1] = 0;
    e.H(0x7D850, P(b1), 3); if (e.blk) e.blk(e.ctx, 0x7D850, P(b1), 8);
    return 1;
}
