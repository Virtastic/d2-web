// 1.13c D2Client+0x22EB0 sound-list sweep by configured category.
#include <cstdint>
#include <cstring>
struct D2c22eb0Env { const uint8_t* (*mem)(uint32_t); uint32_t (*gate)(uint32_t); uint32_t (*config)(uint32_t); uint32_t (*head)(); uint32_t (*table)(); uint32_t (*count)(); void (*drop)(uint32_t, uint32_t, uint32_t); void (*fatal)(); };
static uint32_t rd32(const D2c22eb0Env* e, uint32_t a) { uint32_t v; std::memcpy(&v, e->mem(a), 4); return v; }
extern "C" void d2_client_22eb0(uint32_t unit, const D2c22eb0Env* e) {
    if (e->gate(unit) != 1) return;
    uint32_t cfg = e->config(unit);
    if (!cfg) return;
    if (!unit) { e->fatal(); return; }
    uint32_t node = rd32(e, unit + 0x78), head = e->head();
    while (node) {
        uint32_t nxt = rd32(e, node + 4), id = rd32(e, node), cls = 0;
        if (id) for (uint32_t n = head; n; n = rd32(e, n + 4)) { uint32_t d = rd32(e, n); if (rd32(e, d + 8) == id) { cls = rd32(e, d + 4); break; } }
        uint32_t tbl = e->table();
        int32_t cat = 0;
        if (tbl) {
            if ((int32_t)cls < 0 || (int32_t)cls >= (int32_t)e->count()) e->fatal();
            else cat = (int32_t)rd32(e, cls * 0x8E + tbl + 0x60);
        }
        node = nxt;
        if (cat == (int32_t)rd32(e, cfg + 0x44) || cat == (int32_t)rd32(e, cfg + 0x48) || cat == (int32_t)rd32(e, cfg + 0x4C) || cat == (int32_t)rd32(e, cfg + 0x50)) {
            e->drop(id, unit, 1);
            head = e->head();
        }
    }
}
