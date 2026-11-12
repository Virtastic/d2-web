// Module globals the server-packet handlers in D2Client read and write. Weak definitions: the owner of the live
// state (or a replay probe) pins them; each array mirrors the module range starting at the named offset.
#pragma once
#include <cstdint>
#include "../fn/d2game_gp.h"
#define D2C_PKTG(name, n) __attribute__((weak)) uint8_t name[n]
D2C_PKTG(g_d2client_10a608, 512);   // rec table (bucket heads, 128 dwords)
D2C_PKTG(g_d2client_10a808, 512);   // second rec table
D2C_PKTG(g_d2client_10f2f0, 4);
D2C_PKTG(g_d2client_1223d8, 4);
D2C_PKTG(g_d2client_1223f0, 4);
D2C_PKTG(g_d2client_122494, 0x2c);
D2C_PKTG(g_d2client_123394, 8);
D2C_PKTG(g_d2client_11c394, 4);
D2C_PKTG(g_d2client_11bf4c, 4);
D2C_PKTG(g_d2client_11bbfc, 4);
D2C_PKTG(g_d2client_11991c, 4);
D2C_PKTG(g_d2client_11c3f0, 4);
D2C_PKTG(g_d2client_ce474, 4);
namespace d2cl {
constexpr uint32_t kBase = 0x6FAB0000u;
inline void wr16(uint32_t a, uint32_t v) { uint16_t w = (uint16_t)v; __builtin_memcpy((void*)(uintptr_t)a, &w, 2); }
inline uint32_t gw(const uint8_t* g, uint32_t o = 0) { uint32_t v; __builtin_memcpy(&v, g + o, 4); return v; }
inline void gw32(uint8_t* g, uint32_t v, uint32_t o = 0) { __builtin_memcpy(g + o, &v, 4); }
// walk a rec chain: bucket head from table[id & 0x7f], links at +0xe4, ids at +0xc; 0 on a miss
inline uint32_t recFind(GE& e, const uint8_t* table, uint32_t id) {
    uint32_t p = gw(table, (id & 0x7f) * 4);
    while (p && e.rd32(p + 0xc) != id) p = e.rd32(p + 0xe4);
    return p;
}
}
