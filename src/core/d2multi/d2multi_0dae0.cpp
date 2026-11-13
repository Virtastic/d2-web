// D2Multi 0xDAE0: the connect-screen poll (one call runs the message loop until a pass handles nothing). stdcall, one unused
// stack word. The loop pulls a message through the client function table's slot 0x24 (ECX = a stack record whose first dword is the
// message type, EDX = a stack buffer), dispatches it, then steps the connect state machine in [39f20] (0 idle, 1 request sent,
// 2 request answered, 3 connected-wait) on the answers of the D2Win/D2Lang/D2MCPClient thunks, and runs three timers (15 s, 15 s,
// 1 s) off GetTickCount. Every callee goes through the D2mEnv seam (d2multi_env.h); module globals by retail RVA through
// D2mDae0Mem; GetTickCount through mem.tick. The 256/512-element Unicode constructor loops and SetRect fill stack locals only
// and are not modelled. Returns 0 when the pass did no timer work, 1 on connect success (state 2 answer 0) or on a clean pass.
#include "d2multi_env.h"
#include <cstring>

struct D2mDae0Mem {
    void* u;
    uint8_t* (*at)(void* u, uint32_t rva);     // the bytes of module global `rva`
    uint32_t (*tick)(void* u);                 // one GetTickCount call
};

namespace {
struct M {
    const D2mDae0Mem& m;
    uint32_t r(uint32_t rva) const { uint32_t v; memcpy(&v, m.at(m.u, rva), 4); return v; }
    uint16_t h(uint32_t rva) const { uint16_t v; memcpy(&v, m.at(m.u, rva), 2); return v; }
    void w(uint32_t rva, uint32_t v) const { memcpy(m.at(m.u, rva), &v, 4); }
    void w16(uint32_t rva, uint16_t v) const { memcpy(m.at(m.u, rva), &v, 2); }
    uint8_t* G() const { return (uint8_t*)(uintptr_t)r(0x39f14); }
    uint32_t t() const { return m.tick(m.u); }
};
uint32_t slot(uint32_t table, uint32_t off) { uint32_t v; memcpy(&v, (const void*)(uintptr_t)(table + off), 4); return v & 0xFFFFFFu; }
// failure: reset the state, look the message up, show it (callback 0x8fc0 unless the timeout path)
void fail(const D2mEnv& e, const M& g, uint32_t id, bool cb) {
    g.w(0x39f20, 0);
    const uint32_t s = d2m_call(e, 0x7ea8, 0, id);
    d2m_call(e, 0xc610, 0, 0, 0, 0, 0, s, cb ? D2M_MOD(0x8fc0) : 0);
}
}

// 1.13c D2Multi+0xDAE0
extern "C" uint32_t d2multi_dae0(const D2mEnv& e, const D2mDae0Mem& mem) {
    const M g{mem};
    uint32_t rect[8] = {};
    uint32_t rec[8] = {};
    uint8_t buf[0x300] = {};
    {
        const uint32_t r = d2m_call(e, 0x7d9a, 0, 0, 0, 0, 0, d2m_ptr(rect));
        d2m_call(e, 0x7d4c, 0, 0, 0, 0, 0, g.r(r ? 0x39f54 : 0x39f50));
    }
    if (g.r(0x39f20) == 2 && g.r(0x39f2c) != 0 && g.t() - g.r(0x39f2c) > 0x7530) {   // answer timeout (30 s)
        fail(e, g, 0x1415, false);
        g.w(0x39f2c, 0);
    }
    for (;;) {
        uint32_t ebp = 0;
        const uint32_t ft = g.r(0x3917c);
        if (d2m_call(e, slot(ft, 0x24), 0, d2m_ptr(rec), d2m_ptr(buf)) != 0) {
            uint32_t idx; memcpy(&idx, buf, 4);
            if (rec[0] == 0) {                                    // a text message: handler table [19bc8 + 4 * idx]
                const uint32_t n = (uint32_t)strlen((const char*)buf + 0x18);
                uint32_t b1, b2; memcpy(&b1, buf + 4, 4); memcpy(&b2, buf + 8, 4);
                if (idx < 0x18) {
                    const uint32_t hnd = g.r(0x19bc8 + 4 * idx);
                    if (hnd) d2m_call(e, hnd & 0xFFFFFFu, 0, b1, b2, 0, 0, d2m_ptr(buf + 0x18), d2m_ptr(buf + 0x19 + n));
                }
            } else if (rec[0] == 2) {
                d2m_call(e, 0x7ba2, 0, 0, 0, 0, 0, D2M_MOD(0x38d10), d2m_ptr(buf), 0x32);
            } else if (rec[0] == 3) {
                d2m_call(e, 0xea50, 0, 0, 0, 0, d2m_ptr(buf));
            }
            ebp = 1;
        }
        ebp |= d2m_call(e, 0x7e12);
        const uint32_t state = g.r(0x39f20);
        if (state == 3) {
            if (d2m_call(e, 0x7e2a) != 0) {
                const uint32_t edi = d2m_call(e, 0x7e90);
                uint16_t ax; memcpy(&ax, (const void*)(uintptr_t)(edi + 4), 2);
                if (ax != 0) {
                    if (ax == g.h(0x39f18)) {
                        const uint32_t sw = d2m_call(e, 0x7e1e);
                        if (sw == 0x33) { d2m_call(e, 0x11f80); d2m_call(e, 0x7e78); }
                        else if (sw == 0x32) { d2m_call(e, 0x11da0); d2m_call(e, 0xce70, 1); d2m_call(e, 0x7e78); }
                        else {
                            uint32_t first; memcpy(&first, (const void*)(uintptr_t)edi, 4);
                            if (first != 0) d2m_call(e, 0x11f80);
                            else { d2m_call(e, 0x12120); d2m_edi(e, 0x12120, edi); }
                            d2m_call(e, 0x7e78);
                        }
                    } else if (ax == g.h(0x39f1c)) {
                        d2m_call(e, 0x13280, 0, 0, 0, edi);
                        d2m_call(e, 0x7e78);
                    } else d2m_call(e, 0x7e78);
                }
            }
        } else if (state == 1 || state == 2) {
            uint8_t* G = g.G();
            bool pass = d2m_call(e, 0x7e2a) != 0;
            if (pass) { const uint32_t ax = d2m_call(e, 0x7e66) & 0xFFFF; pass = ax != 0 && ax == g.h(0x39f18); }
            if (pass && state == 1) {
                const uint32_t sw = d2m_call(e, 0x7e1e);
                if (sw == 0) {
                    uint16_t c = g.h(0x19c34);
                    g.w(0x39f20, 2);
                    do { ++c; } while (c == 0);
                    g.w16(0x19c34, c);
                    d2m_call(e, 0x7e54, 0, d2m_ptr(G + 0x1b), d2m_ptr(G + 0x241), 0, 0, c);
                    g.w16(0x39f18, g.h(0x19c34));
                    d2m_call(e, slot(g.r(0x3917c), 0x34), 0, d2m_ptr(G + 0x1b), d2m_ptr(G + 0x241), 0, 0, D2M_MOD(0x175d7), 0);
                } else if (sw == 0x1e) fail(e, g, 0x1411, true);
                else if (sw == 0x1f) fail(e, g, 0x1412, true);
                else if (sw == 0x20) fail(e, g, 0x1413, true);
                else fail(e, g, 0x1414, true);
            } else if (pass) {
                const uint32_t sw = d2m_call(e, 0x7e1e);
                uint32_t id = 0x1415;                              // retail's byte table collapses to these 12 cases
                switch (sw) {
                case 0: {                                          // connected
                    const uint32_t three = 3; memcpy(G + 0x15, &three, 4);
                    const uint16_t v = (uint16_t)d2m_call(e, 0x7e0c); memcpy(G + 0x19, &v, 2);
                    const uint32_t w = d2m_call(e, 0x7e8a); memcpy(G + 0x225, &w, 4);
                    const uint8_t* src = (const uint8_t*)(uintptr_t)d2m_call(e, 0x7e4e);
                    uint8_t* dst = G + 0x33;
                    for (;;) { const uint8_t ch = *src++; *dst++ = ch; if (ch == 0) break; }
                    g.w(0x3a0dc, 1);
                    d2m_call(e, slot(g.r(0x3917c), 0x3c), 0, d2m_ptr(G + 0x1b), d2m_ptr(G + 0x241));
                    d2m_call(e, 0x7d4c, 0, 0, 0, 0, 0, 0);
                    d2m_call(e, 0x7ca4);
                    return 1;
                }
                case 41: id = 0x1428; break;
                case 42: id = 0x1427; break;
                case 43: id = 0x1429; break;
                case 44: id = 0x142a; break;
                case 113: id = 0x1426; break;
                case 115: id = (G[0x1eb] & 0x20) ? 0x5522 : 0x14f4; break;
                case 116: id = (G[0x1eb] & 0x20) ? 0x5521 : 0x14f3; break;
                case 120: id = 0x2775; break;
                case 121: id = 0x2776; break;
                case 125: id = G[0x3be] ? 0x2ab1 : 0x2ab2; break;
                default: break;
                }
                fail(e, g, id, true);
            }
        }
        // timers
        uint32_t esi = 0;
        uint32_t now = g.t(), last = g.r(0x3a108);
        if (now - last >= 0x3a98) {                                // 15 s: keep-alive through slot 0x30
            g.w(0x3a108, now);
            esi = 1;
            d2m_call(e, slot(g.r(0x3917c), 0x30), 0, g.r(0x3917c));
            const uint8_t* G = g.G();
            uint16_t fl; memcpy(&fl, G + 0x1eb, 2);
            if (!((fl & 8) && (fl & 4))) {
                uint32_t edi = 0;
                if (G[0x229] != 0 && d2m_call(e, 0x7e2a) == 0) edi = 1;
                d2m_call(e, 0xce70, edi);
            }
        }
        if (g.r(0x39f20) == 3) {
            now = g.t(); last = g.r(0x3a104);
            if (now - last >= 0x3a98) { g.w(0x3a104, now); esi = 1; d2m_call(e, 0x14850); }
        }
        now = g.t(); last = g.r(0x3a100);
        if (now - last >= 0x3e8) {                                 // 1 s: hand the pending value over
            g.w(0x3a100, now);
            esi = 1;
            const uint32_t p = g.r(0x19bb8);
            if (p != 0xFFFFFFFFu) {
                d2m_call(e, 0x7e24, 0, D2M_MOD(0x38c10), p);
                g.w16(0x39f1c, (uint16_t)p);
                g.w(0x19bb8, 0xFFFFFFFFu);
            }
        }
        d2m_call(e, 0xb1f0);
        if (esi == 0) return 0;
        if (ebp == 0) return 1;
    }
}
