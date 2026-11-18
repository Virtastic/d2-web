// 1.13c D2Client+0x219F0 unit relation.
#include <cstdint>
#include <cstring>
struct D2c219f0Env { const uint8_t* (*mem)(uint32_t); uint32_t head, cur, tblSlot, bmSlot; uint32_t (*findOwner)(uint32_t); uint32_t (*ownerGroup)(uint32_t); };
static uint32_t rd32(const D2c219f0Env* e, uint32_t a) { uint32_t v; std::memcpy(&v, e->mem(a), 4); return v; }
extern "C" uint32_t d2_client_219f0(uint32_t id, const D2c219f0Env* e) {
    uint32_t n = e->head;
    while (n) { if (rd32(e, n + 8) == id) break; n = rd32(e, n + 0x30); }
    if (!n) return 0;
    int32_t cls = (int32_t)rd32(e, n + 4);
    uint32_t tbl = rd32(e, e->tblSlot);
    uint32_t row = 0;
    bool flagged = false;
    if (cls >= 0 && cls < (int32_t)rd32(e, tbl + 0xBF0)) {
        row = (uint32_t)cls * 0xE0 + rd32(e, tbl + 0xBE8);
        if (row) flagged = (e->mem(e->bmSlot)[0x10] & e->mem(row)[4]) != 0;
    }
    if (!flagged) return 3;
    int32_t cur = e->cur ? (int32_t)rd32(e, e->cur + 0xC) : -1;
    int32_t own = (int32_t)rd32(e, n + 0xC);
    if (own == cur) return 1;
    int16_t g1 = (int16_t)(e->ownerGroup(e->findOwner((uint32_t)cur)) & 0xFFFF);
    if (g1 == -1) return 0;
    int16_t g2 = (int16_t)(e->ownerGroup(e->findOwner((uint32_t)own)) & 0xFFFF);
    return g1 == g2 ? 2 : 0;
}
