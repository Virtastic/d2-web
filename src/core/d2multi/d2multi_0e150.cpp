// 1.13c D2Multi+0xE150: the multiplayer-screen run (the body of the screen entry 0xE730). Context in EAX (`ctx`); the same
// object is also re-read through the module global [39f14] (`G`) at the points retail does. Every callee goes through the
// D2mEnv seam (d2multi_env.h): the import thunks by their RVA, the sibling D2Multi functions by theirs, and the client
// function table at ctx+0x221 (cached in [3917c]) through the slot word it holds (its low 24 bits name the callee).
// Module globals are reached by retail RVA through D2mE150Mem. GetTickCount (twice, result unused) and the
// Initialize/DeleteCriticalSection pair on [38f00] have no effect on the rule and are not modelled. Returns 1 at every exit.
#include "d2multi_env.h"
#include <cstring>

struct D2mE150Mem {
    void* u;
    uint8_t* (*at)(void* u, uint32_t rva);   // the 4 bytes of module global `rva`
};

namespace {
struct M {
    const D2mE150Mem& m;
    uint32_t r(uint32_t rva) const { uint32_t v; memcpy(&v, m.at(m.u, rva), 4); return v; }
    void w(uint32_t rva, uint32_t v) const { memcpy(m.at(m.u, rva), &v, 4); }
    void b(uint32_t rva, uint8_t v) const { *m.at(m.u, rva) = v; }
    uint8_t* G() const { return (uint8_t*)(uintptr_t)r(0x39f14); }
};
uint32_t slot(uint32_t table, uint32_t off) { uint32_t v; memcpy(&v, (const void*)(uintptr_t)(table + off), 4); return v & 0xFFFFFFu; }
}

extern "C" uint32_t d2multi_e150(const D2mEnv& e, const D2mE150Mem& mem, uint8_t* ctx) {
    const M g{mem};
    uint32_t t = d2m_call(e, 0x7c02);                          // Fog #10227
    t = d2m_call(e, 0x7d70, 0, 0, 0, 0, 0, t);                 // D2Win #10016
    if (d2m_call(e, 0x7eae, 0, t, 0) == 0) return 1;           // D2Lang #10008
    uint32_t ft; memcpy(&ft, ctx + 0x221, 4);
    g.w(0x3a0dc, 0); g.w(0x39f20, 0); g.w(0x39f34, 1); g.w(0x3917c, ft);
    if (ft == 0) {                                             // no client table: report and _exit(-1), which does not return
        const uint32_t s = d2m_call(e, 0x7bde, 0, 0, 0, 0, 0, 0xce1);
        d2m_call(e, 0x7bd2, 0, 0, 0, 0, 0, D2M_MOD(0x175d7), s);
        d2m_call(e, 0x14ba, 0, 0, 0, 0, 0, 0xffffffffu);
        return 1;
    }
    if (d2m_call(e, slot(ft, 0x68)) == 0) { g.w(0x3a0dc, 4); d2m_call(e, 0x7eba); return 1; }
    d2m_call(e, 0x9550);
    d2m_call(e, 0x7d4c, 0, 0, 0, 0, 0, g.r(0x39f50));          // D2Win #10099
    g.w(0x3a0c8, 0);
    d2m_call(e, 0x7d7c, 0, 0, 0, 0, 0, g.r(g.G()[0x35d] ? 0x39f40 : 0x39f44));   // D2Win #10006
    if (g.G()[0x35d]) {
        for (uint32_t i = 0; i < 17; i++) g.w(0x39138 + 4 * i, d2m_call(e, 0x7caa, 0, 0, 0, 0, 0, D2M_MOD(0x19d30 + 0x30 * i)));
        g.w(0x3a0d4, d2m_call(e, 0x7caa, 0, 0, 0, 0, 0, D2M_MOD(0x1a180)));
    } else {
        d2m_call(e, 0xb300);
        d2m_call(e, 0x8e70, 0, 0, 0, 0, d2m_ptr(ctx));
    }
    g.w(0x3a0c8, d2m_call(e, 0x7caa, 0, 0, 0, 0, 0, D2M_MOD(0x1a120)));
    if (g.G()[0x35d]) {
        const uint32_t h = d2m_call(e, 0x7caa, 0, 0, 0, 0, 0, D2M_MOD(0x1a150));
        g.w(0x3a0d0, h);
        d2m_call(e, 0x7c98, 0, h, D2M_MOD(0xb760));            // D2Win #10170
        d2m_call(e, 0x7c32, 0, g.r(0x3a0d0), D2M_MOD(0xb750)); // D2Win #10010
        d2m_call(e, 0x7c26, 0, 0, 0, 0, 0, g.r(0x3a0d0));      // D2Win #10053
        d2m_call(e, 0x7cc8, 0, g.r(0x3a0d0), 0xc8);            // D2Win #10202
        d2m_call(e, 0xe760);
        for (uint32_t a = 0x39164; a <= 0x39170; a += 4) d2m_call(e, 0x7d5e, 0, 0, 0, 0, 0, g.r(a), 1);   // D2Win #10001
        // D2Win #10068: 9 stack words 0x3c, 0x64, 0, &0xE0FD0, 0, 0, 4, 0, 0 (words 4..8 are constants, not logged)
        for (uint32_t i = 0; i < 11; i++) g.w(0x38e50 + 4 * i, d2m_call(e, 0x7da6, 0, 0x28 + 0x3c * i, 0x24f, 0, 0, 0x3c, 0x64, 0, D2M_MOD(0x10fd0)));
        const uint32_t h2 = d2m_call(e, 0x7caa, 0, 0, 0, 0, 0, D2M_MOD(0x1a0f0));
        g.w(0x39f80, h2);
        d2m_call(e, 0x7c20, 0, h2, 0);                         // D2Win #10131
        const uint32_t h3 = d2m_call(e, 0x7caa, 0, 0, 0, 0, 0, D2M_MOD(0x1a270));
        const uint32_t f28 = g.r(0x39f28);
        g.w(0x39f84, h3);
        if (f28) d2m_call(e, 0x7c86, 0, h3, 2);                // D2Win #10181
    }
    for (uint32_t i = 0; i < 10; i++) g.b(0x392b8 + 0x100 * i, 0);
    g.w(0x19c48, 0xffffffffu); g.w(0x3a08c, 0);
    d2m_call(e, 0xbe00);
    static const uint32_t k97[4] = { 0x39154, 0x39150, 0x39158, 0x3915c };
    for (uint32_t a : k97) d2m_call(e, 0x7c1a, 0, g.r(a), 0);  // D2Win #10097
    g.w(0x39f20, 0);
    uint32_t L0, L1;                                           // two handles kept in locals, freed at the end
    L1 = d2m_call(e, 0x7caa, 0, 0, 0, 0, 0, D2M_MOD(0x1a1b0));
    g.w(0x3a044, d2m_call(e, 0x7caa, 0, 0, 0, 0, 0, D2M_MOD(0x1a1e0)));
    g.w(0x39f88, d2m_call(e, 0x7caa, 0, 0, 0, 0, 0, D2M_MOD(0x1a210)));
    d2m_call(e, 0x7e3c, 0, D2M_MOD(0x9e30));                   // D2MCPClient #10001
    d2m_call(e, 0x7e60);                                       // D2MCPClient #10052
    d2m_call(e, 0x9d60);
    L0 = d2m_call(e, 0x7caa, 0, 0, 0, 0, 0, D2M_MOD(0x1a240));
    const uint8_t f = g.G()[0x1eb];
    if ((f & 8) && (f & 4)) { d2m_call(e, 0x7c14, 0, g.r(0x39150), 0); d2m_call(e, 0x7c14, 0, g.r(0x39158), 0); }   // D2Win #10147
    d2m_call(e, 0x7c38, 0, 0, 0, 0, 0, g.r(0x39160));          // D2Win #10031
    g.b(0x392b4, 0); g.b(0x39298, 0);
    d2m_call(e, 0x7dee);                                       // D2sound #10021
    d2m_call(e, 0x7dfa, 0, D2M_MOD(0x17968));                  // D2sound #10048
    if (d2m_call(e, 0x7de2)) d2m_call(e, 0x7df4, 0, 1);        // D2sound #10035 / #10045
    const uint32_t m4 = g.G()[0x35a] == 4;
    const uint32_t h15c = g.r(0x3915c);
    g.w(0x3a094, m4);
    d2m_call(e, 0x7c14, 0, h15c, m4 == 0);
    if (d2m_call(e, 0xb670)) {
        uint8_t buf[0x100];
        d2m_call(e, 0x1000, 0, 0, 0, 0, 0, d2m_ptr(buf), D2M_MOD(0x179b0), d2m_ptr(ctx + 0x229), d2m_ptr(ctx + 0xb9));   // _sprintf "%s,%s"
        uint32_t fl = (ctx[0x1eb] & 0x20) ? 1 : 0;
        if (g.G()[0x35d]) fl |= 2;
        d2m_call(e, slot(g.r(0x3917c), 0x54), 0, d2m_ptr(ctx + 0xb9), d2m_ptr(buf), 0, 0, 0x25, d2m_ptr(ctx + 0x35d), fl);
    } else {
        d2m_call(e, 0x7c0e, 0, 0, 0, 0, 0, D2M_MOD(0x17938));  // Fog #10029 "Could not restore MCP connection."
        const uint32_t tb = g.r(0x3917c);
        d2m_call(e, slot(tb, 0x10), 0, tb);
    }
    if (d2m_call(e, slot(g.r(0x3917c), 0x68)) == 0) {
        g.w(0x3a0dc, 4);
    } else {
        d2m_call(e, 0x7e36, 0, 0, 0, 0, 0, 0);                 // D2MCPClient #10016(0)
        if (g.G()[0x35d] == 0) d2m_call(e, slot(g.r(0x3917c), 0xa8), 0, D2M_MOD(0xc510), D2M_MOD(0xc490));
        d2m_call(e, 0x7cce, 0, 0, 0, 0, 0, D2M_MOD(0xdae0));   // D2Win #10014
        if (d2m_call(e, 0x7e18)) { d2m_call(e, 0x7e7e); d2m_call(e, 0x7e36, 0, 0, 0, 0, 0, 0); }
        if (g.G()[0x35d] == 0) d2m_call(e, slot(g.r(0x3917c), 0xac));
        d2m_call(e, slot(g.r(0x3917c), 0x58));
        if (g.r(0x3a0dc) != 4) d2m_call(e, 0x7e30);            // D2MCPClient #10018
    }
    d2m_call(e, 0x7de8);                                       // D2sound #10017
    d2m_call(e, 0x10aa0);
    d2m_call(e, 0x7dbe);                                       // D2Win #10123
    d2m_call(e, 0xbe00);
    d2m_call(e, 0x83e0);
    auto fr = [&](uint32_t p) { d2m_call(e, 0x7c8c, 0, 0, 0, 0, 0, p); };   // D2Win #10050 (free handle through its slot)
    fr(D2M_MOD(0x3a0c8));
    if (g.r(0x3a0d4)) fr(D2M_MOD(0x3a0d4));
    fr(d2m_ptr(&L1)); fr(d2m_ptr(&L0));
    fr(D2M_MOD(0x3a044)); fr(D2M_MOD(0x39f88));
    if (g.r(0x39f84)) fr(D2M_MOD(0x39f84));
    if (g.r(0x3a0d0)) fr(D2M_MOD(0x3a0d0));
    for (uint32_t a = 0x38e50; a < 0x38e7c; a += 4) { if (g.r(a)) fr(D2M_MOD(a)); g.w(a, 0); }
    fr(D2M_MOD(0x39f80));
    for (uint32_t a = 0x39138; a < 0x3917c; a += 4) { if (g.r(a)) fr(D2M_MOD(a)); g.w(a, 0); }
    if (g.G()[0x35d] == 0) {
        for (uint32_t a = 0x3912c; a <= 0x39130; a += 4) if (g.r(a)) { fr(D2M_MOD(a)); g.w(a, 0); }
        if (g.r(0x39f7c)) { fr(D2M_MOD(0x39f7c)); g.w(0x39f7c, 0); }
    }
    d2m_call(e, 0x7d7c, 0, 0, 0, 0, 0, 0);
    d2m_call(e, 0x7d4c, 0, 0, 0, 0, 0, 0);
    d2m_call(e, 0x90d0);
    d2m_call(e, 0x7eba);                                       // D2Lang #10005
    return 1;
}
