// 1.13c D2Game+0x6B9B0: put a class 0x3B object (type 2) in the free spot beside the unit. Register convention: ECX = ctx {+0 game, +8 unit}, no result.
// room = 0xA478(unit); pos = 0xA544(unit); kind = 0xA56E(0xA556(room)); k2 = 0xAADE(kind). 0xA694(room, pos, 3, 0x1C09, 0) = 0 ends. pos += 5; a null room is Fog fatal
// line 0xE42. With a unit and 0xA808(room) the message 0xDFFD0(0x18) goes out and it ends. Then the low byte of 0xA56E(k2) must equal that of 0xA56E(0xA556(room))
// (else fatal 0xE5C). 0xAE98(room, pos, pos, 3, 0x3E01, 0xC01, 0) = 0 ends; r = 0x1280(room, y) = 0 ends; obj = 0xE1D90(2, 0x3B, x, y, game, r, 1, 1, 0);
// 0xA4A2(obj, 1); o = 0xA22E0(game, obj, k2, roomKind) = 0 ends; 0xAEC8(o, 0xAEE0(o) | 3); 0xA51A(r, 0); 0xA51A(0xA478(o), 0).
#include <cstdint>
#include <cstring>
#include "d2game_6b9b0.h"

static inline uint32_t rd32(uint32_t p, uint32_t o) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)(p + o), 4); return v; }

// 1.13c D2Game+0x6B9B0
void D2Game_6B9B0(uint32_t pCtx, const D2GameDropObjEnv* pEnv)
{
    const uint32_t game = rd32(pCtx, 0), unit = rd32(pCtx, 8);
    const uint32_t room = pEnv->room(unit);
    uint32_t pos[2];
    pEnv->pos(unit, pos);
    const uint8_t k1 = (uint8_t)pEnv->a56e(pEnv->kindOf(room));
    const uint32_t k2 = pEnv->aade(k1);
    if (!pEnv->a694(pEnv->room(unit), pos, 3, 0x1C09, 0)) return;
    uint32_t blk[2] = { pos[0] + 5, pos[1] + 5 };
    if (!room) { pEnv->fatal(0xE42); return; }
    if (unit && pEnv->a808(room)) { pEnv->dffd0(unit, 0x18); return; }
    const uint8_t b = (uint8_t)pEnv->a56e(k2);
    const uint32_t kind = pEnv->kindOf(room);
    if (b != (uint8_t)pEnv->a56e(kind)) { pEnv->fatal(0xE5C); return; }
    if (!pEnv->ae98(room, blk, blk, 3, 0x3E01, 0xC01, 0)) return;
    const uint32_t r = pEnv->f1280(room, blk[1]);
    if (!r) return;
    const uint32_t obj = pEnv->create(2, 0x3B, blk[0], blk[1], game, r, 1, 1, 0);
    pEnv->a4a2(obj, 1);
    const uint32_t o = pEnv->a22e0(game, obj, k2, kind);
    if (!o) return;
    pEnv->aec8(o, (uint8_t)(pEnv->aee0(o) | 3));
    pEnv->a51a(r, 0);
    pEnv->a51a(pEnv->room(o), 0);
}
