// 1.13c D2Game+0xCE150: tell a client where a unit is. Disassembled convention: ECX = game, EDX = unit, stack = client, table index;
// ret 8. A null client is retail's fatal assert (line 0xFB) and is not reached here. The client's current target is client+0x174 when
// byte client+0x3D4 bit 0 is set; otherwise 0 when that word is 0, else 0xE03A0 (EAX = client+0x16C, ECX = client+0x1A8,
// EDX = client+0x170), a zero result clearing client+0x174. The target itself returns. Then 0xE07A0 (EAX = unit, ECX = game); when
// the 0xA598 owner of unit+0x2C is neither the unit nor 0 and its 0xA478 room lists the client among its [room+0x48] dwords
// ([room+0x78] of them), 0x8C230 is called (EAX = index*3 with AL = 0x10, ECX = 0, EDX = unit id; stack = client, the table's
// +4 byte, owner type, owner id, 0x1100 x, 0x1130 y) and returns. Otherwise the 16-byte packet 0x0F {0, id, table +0 byte,
// 0xAAC6 word, 0xAAD8 word, 0, word12, word14} goes out through 0x8A3E0 (EAX = client, stack = packet, 0x10); types 2, 4 and 5
// take word12/word14 from path+0xC / path+0x10, every other type (signed compare) from thunks 0xA454 / 0xA3B8, 0 for a null path.
#include <cstdint>
#include <cstring>
#include "d2game_ce150.h"

static inline uint32_t rd32(uint32_t a, uint32_t o) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)(a + o), 4); return v; }
static inline uint8_t rd8(uint32_t a, uint32_t o) { return *(const uint8_t*)(uintptr_t)(a + o); }
static inline bool pathDirect(int32_t t) { return t == 2 || (t > 3 && t <= 5); }

void d2game_ce150_port(uint32_t game, uint32_t unit, uint32_t client, uint32_t idx, const D2GameCE150Env* e) {
    if (!client) return;   // retail: fatal assert, _exit(-1)
    uint32_t target;
    if (rd8(client, 0x3d4) & 1) target = rd32(client, 0x174);
    else if (rd32(client, 0x174) == 0) target = 0;
    else {
        target = e->f_e03a0(rd32(client, 0x16c), rd32(client, 0x1a8), rd32(client, 0x170));
        if (!target) std::memset((void*)(uintptr_t)(client + 0x174), 0, 4);
    }
    if (unit == target) return;
    e->f_e07a0(unit, game);
    const uint32_t owner = e->o10392(rd32(unit, 0x2c));
    if (owner != unit && owner) {
        const uint32_t room = e->o10331(owner);
        if (room) {
            const uint32_t n = rd32(room, 0x78), list = rd32(room, 0x48);
            for (uint32_t i = 0; i < n; i++) {
                if (rd32(list, i * 4) != client) continue;
                const uint32_t oid = rd32(owner, 0xc), otype = rd32(owner, 0), uid = rd32(unit, 0xc);
                const uint32_t y = e->f_1130(unit), x = e->f_1100(unit);
                uint8_t row[12]; std::memcpy(row, e->tab + idx * 12, 12);
                e->f_8c230(((idx * 3) & ~0xffu) | 0x10, 0, uid, client, row[4], otype, oid, x, y);
                return;
            }
        }
    }
    const int32_t t = (int32_t)rd32(unit, 0);
    const uint32_t path = rd32(unit, 0x2c);
    uint32_t w14 = 0, w12 = 0;
    if (pathDirect(t)) w14 = rd32(path, 0x10); else if (path) w14 = e->f_a3b8(path);
    if (pathDirect(t)) w12 = rd32(rd32(unit, 0x2c), 0xc); else if (rd32(unit, 0x2c)) w12 = e->f_a454(rd32(unit, 0x2c));
    const uint32_t id = rd32(unit, 0xc);
    const uint16_t wy = (uint16_t)e->f_aad8(path), wx = (uint16_t)e->f_aac6(path);
    uint8_t pk[16];
    pk[0] = 0x0f; pk[1] = 0;
    std::memcpy(pk + 2, &id, 4);
    pk[6] = e->tab[idx * 12];
    std::memcpy(pk + 7, &wx, 2);
    std::memcpy(pk + 9, &wy, 2);
    pk[11] = 0;
    const uint16_t a = (uint16_t)w12, b = (uint16_t)w14;
    std::memcpy(pk + 12, &a, 2);
    std::memcpy(pk + 14, &b, 2);
    e->f_8a3e0(client, pk, 0x10);
}
