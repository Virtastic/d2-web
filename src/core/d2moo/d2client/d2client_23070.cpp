// 1.13c D2Client+0x23070 sound-list sweep by table flag.
#include <cstdint>
#include <cstring>
struct D2c23070Env { const uint8_t* (*mem)(uint32_t); uint32_t (*head)(); uint32_t (*table)(); uint32_t (*count)(); void (*drop)(uint32_t, uint32_t, uint32_t); void (*fatal)(); };
static uint32_t rd32(const D2c23070Env* e, uint32_t a) { uint32_t v; std::memcpy(&v, e->mem(a), 4); return v; }
extern "C" void d2_client_23070(uint32_t unit, const D2c23070Env* e) {
    if (!unit) { e->fatal(); return; }
    uint32_t node = rd32(e, unit + 0x78), head = e->head();
    while (node) {
        uint32_t nxt = rd32(e, node + 4), id = rd32(e, node), cls = 0;
        if (id) for (uint32_t n = head; n; n = rd32(e, n + 4)) { uint32_t d = rd32(e, n); if (rd32(e, d + 8) == id) { cls = rd32(e, d + 4); break; } }
        uint32_t tbl = e->table();
        node = nxt;
        if (!tbl) continue;
        if ((int32_t)cls < 0 || (int32_t)cls >= (int32_t)e->count()) { e->fatal(); continue; }
        if (e->mem(cls * 0x8E + tbl)[0x3E]) { e->drop(id, unit, 0); head = e->head(); }
    }
}
