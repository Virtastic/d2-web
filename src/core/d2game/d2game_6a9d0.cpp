// D2Game+0x6A9D0 -- door operate (fastcall ECX = pOp {game, obj, player}, EDX unused, plain ret; always 1).
// Rate limit: nothing until tick >= obj+0xD4 + 500 (32-bit wrap). Mode 6 (locked): key check passes -> free path, mode 2, stamp; else
// sound 0x16 on the player. Mode 0: free path, mode 2, stamp. Modes 2/5: no collision under mask 0x8180 -> block path, mode 0, stamp;
// none under 0x8000 -> mode 5 (nothing if already 5); else mode 4, stamp. Collision results are tested as 16-bit. Other modes: nothing.
#include "d2game_6a9d0.h"

// 1.13c D2Game+0x6A9D0
int32_t d2g_6a9d0(void* game, void* obj, void* player, const D2G6A9D0Env* e)
{
    void* room = e->room(obj);
    int32_t xy[2] = {};
    e->coords(obj, xy);
    const int32_t sx = e->sizeX(obj), sy = e->sizeY(obj);
    const uint32_t now = e->tick();
    const uint32_t mode = e->mode(obj);
    if (now < e->stamp(obj) + 500u) return 1;
    int32_t next;
    switch (mode) {
    case 6:
        if (!e->keyCheck(player, game)) { e->sound(player, 0x16, player); return 1; }
        [[fallthrough]];
    case 0:
        e->freePath(obj);
        next = 2;
        break;
    case 2:
    case 5:
        if ((uint16_t)e->collide(room, xy[0], xy[1], sx, sy, 0x8180) == 0) {
            e->blockPath(obj, room, xy[0], xy[1]);
            next = 0;
            break;
        }
        if ((uint16_t)e->collide(room, xy[0], xy[1], sx, sy, 0x8000) == 0) {
            if (mode == 5) return 1;
            next = 5;
            break;
        }
        next = 4;
        break;
    default:
        return 1;
    }
    e->setMode(obj, next);
    e->setStamp(obj, now);
    return 1;
}
