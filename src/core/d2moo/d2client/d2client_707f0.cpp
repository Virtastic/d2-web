// 1.13c D2Client+0x707F0: chat command interpreter. EAX = typed Unicode line, EDX + [esp+4] = the writable
// ANSI copy, ret 4, void. Every callee is reached through the injected dispatcher (D2cSvc) by its D2Client RVA; the CRT string calls and the
// D2Lang Unicode::strlen run here. Globals arrive as pointers (D2c707F0G): the caller pins them, the port reads and writes through them.
#include <cstdint>
#include <cstdio>
#include <cstring>
#include "d2client_callee.h"

struct D2c707F0G {
    uint8_t* gameType; uint8_t* ime;                         // 0x113F60, 0x11C038
    uint32_t *c2c0, *c2c8, *c2f0, *c394, *c434, *dd5d0, *t3498, *t349c, *fad94, *fada8;
    const uint32_t* modal[9];                                // 0xFADB0 0xFADDC 0xFADE4 0xFADE8 0xFADEC 0xFADF0 0xFADF4 0xFADF8 0xFAE00
    uint32_t vtFnRva;                                        // callee RVA of the Battle.net vtable slot +0x14, 0 when [gs+0x221] is null
};

static int d2c707_lc(int c) { return c >= 'A' && c <= 'Z' ? c + 32 : c; }
static int d2c707_stricmp(const char* a, const char* b, int n = -1) {
    for (; n != 0; ++a, ++b) {
        int x = d2c707_lc((uint8_t)*a), y = d2c707_lc((uint8_t)*b);
        if (x != y) return x < y ? -1 : 1;
        if (!x) return 0;
        if (n > 0) --n;
    }
    return 0;
}
// MSVCRT atol: leading white space, one sign, digits accumulated in 32 bits (wraps).
static int32_t d2c707_atol(const char* s) {
    while (*s == ' ' || (*s >= 9 && *s <= 13)) ++s;
    bool neg = false;
    if (*s == '-') { neg = true; ++s; } else if (*s == '+') ++s;
    uint32_t t = 0;
    while (*s >= '0' && *s <= '9') t = t * 10u + (uint32_t)(*s++ - '0');
    return (int32_t)(neg ? 0u - t : t);
}
static uint32_t d2c707_p(const void* p) { return (uint32_t)(uintptr_t)p; }

// 1.13c D2Client+0x707F0
extern "C" void d2c_707f0_svc(const D2cSvc& v, const D2c707F0G& g, const uint16_t* uni, char* line) {
    uint8_t pkt[0x140] = {};                                 // the retail frame: {op, kind, gameType, 0x100 bytes of text, ...}
    char* linePtr = line;                                    // local_124
    pkt[2] = *g.gameType;

    // Edit-line close block shared by "/fps" and "/framerate" (ebx = 1 in both).
    auto closeBlock = [&]() {
        const uint32_t saved = *g.fad94;
        d2c_call(v, 0xbf8c, 0, 0, 0, 0, 0);                  // Ordinal_10227
        if (*g.dd5d0 != 0x26) {                              // fatal 0x139B, then _exit(-1)
            const uint32_t m = d2c_call(v, 0xbf92, 0, 0, 0, 0, 0, 0x139b);
            d2c_call(v, 0xbf80, 0, 0, 0, 0, 0, 0xd359f, m);
            d2c_call(v, 0x3664, 0, 0, 0, 0, 0, 0xffffffffu);
            return;
        }
        *g.fad94 = 0;
        if (saved == 0) return;
        d2c_call(v, 0x6f4a0, 0, 0, 0, 0, 0);
        bool clear = true;
        for (const uint32_t* m : g.modal) if (*m) { clear = false; break; }
        if (clear) {
            *g.c434 = 1;
            if (*g.t349c == 0) {
                *g.t349c = 1;
                if (*g.t3498 != 0) {
                    const uint32_t w = d2c_call(v, 0xd0b4, 0, 0, 0, 0, 0, 0xf51ec);
                    d2c_call(v, 0x3cc60, 2, 0, 0, 0, 0, w, 0xf51ec);
                    *g.t3498 = 0;
                }
                const uint32_t w = d2c_call(v, 0xd0b4, 0, 0, 0, 0, 0, 0xf51bc);
                d2c_call(v, 0x3ccc0, 4, 0, 0, 0, 0, w, 0xf51bc);
            }
        }
        if (*g.fada8) {
            const uint32_t w = d2c_call(v, 0xd0b4, 0, 0, 0, 0, 0, 0xd0780);
            d2c_call(v, 0x3ccc0, 5, 0, 0, 0, 0, w, 0xd0780);
        }
    };

    if (d2c707_stricmp(line, "/fps") == 0) {
        if (*g.c2c8 != 0 || *g.c2c0 == 0) d2c_call(v, 0x5a5e0, 0, 0, 0, 0, 0);
        *g.c2c8 = 1;
        closeBlock();
        return;
    }
    if (d2c707_stricmp(line, "/framerate") == 0) {
        d2c_call(v, 0x5a5e0, 0, 0, 0, 0, 0);
        closeBlock();
        return;
    }
    if (d2c707_stricmp(line, "/players", 8) == 0 && (*g.c394 == 0 || *g.c394 == 1 || *g.c394 == 6 || *g.c394 == 8)) {
        int32_t n = d2c707_atol(line + 8);
        if (n <= 1) n = 1; else if (n >= 8) n = 8;
        d2c_call(v, 0xd054, 0, (uint32_t)n, 0, 0, 0);        // Ordinal_10049
        std::snprintf(line, 64, "players set to %d", n);
    }
    if (d2c707_stricmp(line, "/nopickup") == 0) {
        *g.c2f0 = *g.c2f0 == 0;
    } else if (d2c707_stricmp(line, "soundchaosdebug") == 0) {
        d2c_call(v, 0x88d00, 0, 0, 0, 0, 0);
    } else {
        pkt[1] = 1; pkt[0] = 0x15; pkt[2] = *g.gameType;
        const uint32_t uniP = d2c707_p(uni);
        bool sendPacket = true;
        auto ime = [&]() { return *g.ime == 0 || (uint8_t)d2c_call(v, 0x6ec00, uniP, 0, 0, 0, 0) != 0; };
        bool classified = false;
        if (ime()) {
            const uint32_t r = d2c_call(v, 0x6ec30, 0, 0, 0, 0, 0, d2c707_p(&linePtr), d2c707_p(pkt + 1), d2c707_p(pkt + 0x103));
            if (r != 0 && pkt[1] == 0) { pkt[0] = 0x14; classified = true; }
        }
        if (!classified && pkt[1] == 3 && ime()) {
            const uint32_t lp = d2c707_p(linePtr);
            static const uint32_t w5[4] = { 0xd4ddc, 0xd4dd8, 0xd4dcc, 0xd4dc8 };
            static const uint32_t w7[2] = { 0xd4dc0, 0xd4dbc };
            bool done = false;
            for (uint32_t b : w5) if (d2c_call(v, 0x70500, lp, 0, 0, b, 0, 1) != 0) { done = true; break; }
            if (!done) for (uint32_t b : w7) if (d2c_call(v, 0x70700, lp, 0, 0, b, 0) != 0) { done = true; break; }
            if (!done && g.vtFnRva) d2c_call(v, g.vtFnRva, 0, lp, 0, 0, 0, lp);
            sendPacket = false;
        }
        if (sendPacket) {
            uint32_t len = 0;
            while (uni[len]) ++len;                          // Unicode::strlen
            if ((int32_t)len > 0) {
                std::memcpy(pkt + 3, linePtr, 0xff);
                pkt[0x102] = 0;
                d2c_call(v, 0x147a0, (len & ~0xffu) | pkt[1], 0, 0, 0, d2c707_p(pkt), d2c707_p(pkt));
            }
        }
    }
    d2c_call(v, 0xc2790, 0, 5, 1, 0, 0, 0);
}
