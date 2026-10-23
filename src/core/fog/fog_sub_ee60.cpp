// 1.13c Fog+0x0EE60 -- QServer per-connection receive loop (QSrvProcessMsg): drain parsed messages
// from the connection's 0x5b4-byte inbuf into pooled message nodes queued on the server's per-slot list.
#include <cstdint>
#include <cstring>
#include <cstdlib>

namespace {
// Host seams for the Win32/Fog callees (null-safe defaults make the loop a no-op).
struct QsHost {
    void (*lock)(void* cs) = nullptr;          // EnterCriticalSection (+ Fog+0x11690 tracer)
    void (*unlock)(void* cs) = nullptr;        // LeaveCriticalSection
    uint8_t* (*allocNode)() = nullptr;         // Fog+0x1B560(null) pool node, 0x5c0 bytes; null on failure
    int (*spamCheck)(void* srv, int v) = nullptr;  // Fog+0xD520
    void (*ban)(uint32_t sock) = nullptr;      // Ordinal_11 + log + Fog+0xE910 disconnect
    uint32_t (*tick)() = nullptr;              // GetTickCount
    void (*fatal)(int line) = nullptr;         // Fog+0xFC20/Ordinal_116/log/Fog+0x184A0, then _exit(-1)
};
QsHost g_qs;
inline uint32_t& U(void* b, size_t o) { return *reinterpret_cast<uint32_t*>(static_cast<uint8_t*>(b) + o); }
inline uint8_t*& P(void* b, size_t o) { return *reinterpret_cast<uint8_t**>(static_cast<uint8_t*>(b) + o); }
}

// srv = param_1 (server), conn = param_2 (connection). Offsets are retail 32-bit layout;
// ponytail: pointer slots (0x870/0x874/0x878, node link 0x5bc) assumed host-width here, remap if structs are shared with retail memory.
void fog_qsrvProcessMsg(uint8_t* srv, uint8_t* conn) {
    using Cb = int (*)(int*, int*, int*, int*, uint32_t, uint32_t);
    auto cb = [&]() { return *reinterpret_cast<Cb*>(srv + 0xbc0); };
    if (!cb()) return;
    uint8_t* inbuf = conn + 0xb0;
    void* cs = conn + 0x2c;
    for (;;) {
        int consumed = 0, start = 0, slot = 0, aux = 0;
        g_qs.lock(cs); g_qs.unlock(cs);
        int r = cb()(&consumed, &start, &slot, &aux, U(conn, 0), U(conn, 0x20));
        uint8_t* node = nullptr;
        if (r == 3) return;
        if (r == 4) { g_qs.ban(U(conn, 0x18)); return; }
        if (r == 2 && U(srv, 0xbd0) && !g_qs.spamCheck(srv, aux)) { g_qs.ban(U(conn, 0x18)); return; }
        if (r == 2 || r != 5) {
            void* scs = srv + 0x854;
            if (!P(srv, 0x878)) node = g_qs.allocNode();
            else {
                g_qs.lock(scs);
                node = P(srv, 0x878);
                if (!node) node = g_qs.allocNode();
                else P(srv, 0x878) = reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(U(node, 0x5bc)));
                g_qs.unlock(scs);
            }
            uint32_t len = (uint32_t)(consumed - start);
            if ((int)len < 1) node[0] = 0;
            else { g_qs.lock(cs); memcpy(node + 4, inbuf + start, len); g_qs.unlock(cs); }
            U(node, 0x5b4) = len;
            U(node, 0x5b8) = g_qs.tick();
            U(node, 0x5bc) = 0;
            U(node, 0) = U(conn, 0);
            if ((int)U(srv, 0x87c) <= slot) { g_qs.fatal(0x69c); return; }
            g_qs.lock(srv + 0x854);
            uint8_t* tail = P(P(srv, 0x874), slot * 8);
            if (!tail) P(P(srv, 0x870), slot * 8) = node; else P(tail, 0x5bc) = node;
            P(P(srv, 0x874), slot * 8) = node;
            g_qs.unlock(srv + 0x854);
        }
        g_qs.lock(cs);
        uint32_t rest = U(conn, 0xa8) - consumed;
        U(conn, 0xa8) = rest;
        if (0x5b4 < (int)(rest + consumed)) { g_qs.fatal(0x6a8); return; }
        memmove(inbuf, inbuf + consumed, rest);
        g_qs.unlock(cs);
        if (!cb()) return;
    }
}
