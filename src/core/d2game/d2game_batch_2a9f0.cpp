// Stage A: 1.13c game/client cleanup and object placement helpers.
// Prior art: D2MOO GAME/Game.cpp and OBJECTS/Objects.cpp (1.10f); all rules below
// were checked against the cached 1.13c functions. Host pointers use the native
// D2MOO structures; retail coordinates, IDs and RNG words remain 32-bit.
#include <algorithm>
#include <cstddef>
#include <cstdlib>
#include <D2Collision.h>
#include <D2Dungeon.h>
#include <D2DataTbls.h>
#include <DataTbls/ObjectsTbls.h>
#include <GAME/Game.h>
#include <GAME/Clients.h>
#include <GAME/Event.h>
#include <ITEMS/Items.h>
#include <MONSTER/MonsterSpawn.h>
#include <OBJECTS/Objects.h>
#include <OBJECTS/ObjRgn.h>
#include <UNIT/SUnit.h>

namespace d2game_wave3_1 {
// Register-only callees are explicit typed hooks. No game/global state is
// recreated here: the caller supplies the same live state used by the engine.
struct Calls {
    void (*queuePacket)(D2ClientStrc*, const void*, uint32_t); // 0x8A3E0
    void (*savePlayer)(D2GameStrc*, D2UnitStrc*, const char*, int32_t); // 0x54320
    void (*flushClient)(D2ClientStrc*, D2GameStrc*); // 0x2D790
    void (*leaveClient)(uint32_t, int32_t); // 0x2DC70
    void (*spawnAt)(D2GameStrc*, D2ActiveRoomStrc*, D2CoordStrc*, void*); // 0x12230
    void (*initializeFire)(D2ObjInitFnStrc*); // 0x2ECB0
};
[[noreturn]] static void fatal() { std::abort(); }
static_assert(sizeof(D2DrlgCoordsStrc) == 32);
static_assert(offsetof(D2DrlgCoordsStrc, nSubtileHeight) == 12);
static_assert(offsetof(D2ObjectsTxt, dwParm) == 0x178);

// 1.13c D2Game+0x2A9F0
// ECX = client, EDX = unit. The adjacent list/count are stack outputs of
// D2Common #10383, which the decompiler confuses with the live unit register.
void removeVisibleUnit(D2ClientStrc* client, D2UnitStrc* unit, const Calls& calls)
{
    if (!unit) return;
    auto* room = UNITS_GetRoom(unit);
    if (!client->pRoom) fatal();
    D2ActiveRoomStrc** adjacent = nullptr;
    int32_t adjacentCount = 0;
    DUNGEON_GetAdjacentRoomsListFromRoom(client->pRoom, &adjacent, &adjacentCount);
    auto* rooms = adjacent;
    const uint32_t count = static_cast<uint32_t>(adjacentCount);
    for (uint32_t i = 0; i < count; ++i) {
        if (!rooms[i]) fatal();
        if (rooms[i] != room) continue;
        if (static_cast<uint8_t>(unit->dwUnitType) != 3) {
            uint8_t packet[6] = {10, static_cast<uint8_t>(unit->dwUnitType)};
            uint32_t id = unit->dwUnitId;
            for (int j = 0; j < 4; ++j) packet[j + 2] = static_cast<uint8_t>(id >> (j * 8));
            calls.queuePacket(client, packet, sizeof packet);
        }
        return;
    }
}

// 1.13c D2Game+0x2DE30
void saveAndFlush(D2GameStrc* game, const Calls& calls)
{
    if (game->nGameType != 1 && game->nGameType != 2) return;
    for (auto* client = game->pClientList; client;) {
        auto* next = client->pNext;
        auto* player = client->pPlayer;
        if (!(client->dwFlags & 1) && player) {
            player = SUNIT_GetServerUnit(client->pGame, client->dwUnitType, client->dwUnitGUID);
            if (!player) client->pPlayer = nullptr;
        }
        if (player) {
            calls.savePlayer(game, player, client->szName, 0);
            calls.flushClient(client, game);
        }
        client = next;
    }
}

// 1.13c D2Game+0x2E160
// ESI = game, stack = optional client. Disassembly resolves two output
// callbacks that Ghidra marks unreachable: first choose the last flagged
// ingame client; otherwise use the supplied client or the first ingame one.
void leaveSelectedClient(D2GameStrc* game, D2ClientStrc* client, const Calls& calls)
{
    if (!game) fatal();
    D2ClientStrc* selected = nullptr;
    for (auto* c = game->pClientList; c; c = c->pNext)
        if (c->dwClientState == 4 && (c->dwFlags & 0x20)) selected = c;
    if (!selected) {
        selected = client;
        if (!selected)
            for (auto* c = game->pClientList; c; c = c->pNext)
                if (c->dwClientState == 4 && !selected) selected = c;
    }
    if (selected) calls.leaveClient(selected->dwClientId, 0);
}

// The helpers receive coords in ESI, room rectangle in EAX, sizeX in EDI
// (0x2E660) or on the stack (0x2E5D0). Boundary tests use unsigned words,
// while collision calls receive the complete coordinate dwords.
// 1.13c D2Game+0x2E5D0
int32_t placement(D2ActiveRoomStrc* room, const D2DrlgCoordsStrc& r,
                  const D2CoordStrc& p, int32_t sx, int32_t sy)
{
    const int32_t x = static_cast<uint16_t>(p.nX), y = static_cast<uint16_t>(p.nY);
    return r.nSubtileWidth > 1 && r.nSubtileHeight > 1 &&
        x > r.nSubtileX + 2 && y > r.nSubtileY + 2 &&
        x < r.nSubtileX - sx + r.nSubtileWidth && y < r.nSubtileY - sy + r.nSubtileHeight &&
        !COLLISION_CheckMaskWithSizeXY(room, p.nX, p.nY, sx + 2, sy + 2, 0x3f11);
}

// 1.13c D2Game+0x2E660
int32_t widePlacement(D2ActiveRoomStrc* room, const D2DrlgCoordsStrc& r,
                      const D2CoordStrc& p, int32_t sx, int32_t sy)
{
    const int32_t x = static_cast<uint16_t>(p.nX), y = static_cast<uint16_t>(p.nY);
    return r.nSubtileWidth >= sx + 2 && r.nSubtileHeight >= sy + 2 &&
        x > r.nSubtileX + 1 && y > r.nSubtileY + 1 &&
        x < r.nSubtileX - sx - 2 + r.nSubtileWidth && y < r.nSubtileY - sy - 2 + r.nSubtileHeight &&
        !COLLISION_CheckMaskWithSizeXY(room, p.nX, p.nY, sx + 7, sy + 7, 0xc01) &&
        !COLLISION_CheckMaskWithSizeXY(room, p.nX, p.nY, sx, sy, 0x3f11);
}

// 1.13c D2Game+0x2E7D0
D2TimerArgStrc* allocTimerArgs(void* pool)
{
    auto* timer = D2_ALLOC_STRC_POOL(pool, D2TimerArgStrc);
    timer->unk0x00 = nullptr;
    timer->dwUnitId = 0;
    timer->dwUnitId = UINT32_MAX;
    auto* node = D2_ALLOC_STRC_POOL(pool, D2TimerArg2Strc);
    node->nUnitGUID = -1;
    node->pNext = nullptr;
    timer->unk0x00 = node;
    return timer;
}

// 1.13c D2Game+0x2E8B0
void changeObjectClass(D2UnitStrc* object)
{
    // +0x17c is dwParm[1], not the object's subtype byte.
    const int32_t replacement = static_cast<int32_t>(DATATBLS_GetObjectsTxtRecord(object ? object->dwClassId : -1)->dwParm[1]);
    object->dwClassId = replacement != 0 && replacement < 573 ? replacement : 84;
}

// 1.13c D2Game+0x2ED70
int32_t shrineId(D2GameStrc* game, uint8_t shrineClass, uint32_t level)
{
    if (!shrineClass || shrineClass > 4) shrineClass = 2;
    auto* data = OBJRGN_GetShrineDataFromGame(game);
    if (!data->nShrineSubTypes[shrineClass]) fatal();
    int32_t id;
    for (int32_t attempt = 0; attempt < 8; ++attempt) {
        auto& seed = game->pObjectControl->pSeed;
        const uint32_t n = data->nShrineSubTypes[shrineClass];
        const uint32_t pick = static_cast<int32_t>(n) < 1 ? 0 : ITEMS_RollRandomNumber(&seed) % n;
        id = data->pShrineSubTypeIds[shrineClass][pick];
        if (!id) id = 1;
        if (level >= DATATBLS_GetShrinesTxtRecord(id)->dwLevelMin) break;
    }
    return id;
}

// 1.13c D2Game+0x2EF50
void initializeFireAndFlag(D2ObjInitFnStrc* op, const Calls& calls)
{
    calls.initializeFire(op);
    if (!op->pObject) fatal();
    op->pObject->bSparkChest = (op->pObject->bSparkChest & ~0xff) | 1;
}

// 1.13c D2Game+0x2F4B0
int32_t populateSmallGrid(D2GameStrc* game, D2ActiveRoomStrc* room,
                         const D2DrlgCoordsStrc& r, const Calls& calls)
{
    uint32_t endX = r.nSubtileX + std::min(r.nSubtileWidth, 12);
    uint32_t endY = r.nSubtileY + std::min(r.nSubtileHeight, 12);
    int32_t count = 0;
    for (uint32_t x = r.nSubtileX + 1; x < endX; x += 2)
        for (uint32_t y = r.nSubtileY + 1; y < endY; y += 2) {
            D2CoordStrc p{static_cast<int32_t>(x), static_cast<int32_t>(y)};
            if (placement(room, r, p, 1, 1)) {
                calls.spawnAt(game, room, &p, nullptr);
                if (++count >= 32) return 1;
            }
        }
    return 1;
}

static uint32_t limited(D2SeedStrc* seed, int32_t n)
{
    return n < 1 ? 0 : ITEMS_RollRandomNumber(seed) % static_cast<uint32_t>(n);
}

// 1.13c D2Game+0x2F690
void trappedSouls(D2ObjInitFnStrc* op)
{
    if (op->pObject && op->pObject->dwAnimMode) return;
    D2DrlgCoordsStrc r{};
    DUNGEON_GetRoomCoordinates(op->pRoom, &r);
    auto* seed = &op->pGame->pObjectControl->pSeed;
    int32_t remaining = ((r.nSubtileHeight * r.nSubtileWidth >> 7) * 30) >> 8;
    constexpr int32_t dx[8] = {-1,0,1,-1,1,-1,0,1};
    constexpr int32_t dy[8] = {-1,-1,-1,0,0,1,1,1};
    for (int outer = 12; remaining > 0 && outer > 0; --outer) {
        D2CoordStrc p;
        p.nX = limited(seed, r.nSubtileWidth - 4) + r.nSubtileX;
        p.nY = limited(seed, r.nSubtileHeight - 4) + r.nSubtileY;
        if (!widePlacement(op->pRoom, r, p, 3, 3)) continue;
        auto spawn = [&](int32_t x, int32_t y) {
            const int32_t monster = (ITEMS_RollRandomNumber(&op->pObjectregion->pSeed) & 1) ? 404 : 403;
            if (D2GAME_SpawnMonster_6FC69F10(op->pGame, op->pRoom, x, y, monster, 1, -1, 0)) {
                UNITS_ChangeAnimMode(op->pObject, 2);
                --remaining;
            }
        };
        spawn(op->nX, op->nY);
        int32_t base = 1;
        bool found = true;
        for (;;) {
            if (limited(seed, base >> 1) != 0 || !found) break;
            found = false;
            const int32_t tries = std::max(remaining, 4) * 3;
            for (int32_t attempt = 0; attempt < tries; ++attempt) {
                const uint32_t direction = ITEMS_RollRandomNumber(seed) & 7;
                p.nX += (ITEMS_RollRandomNumber(seed) % 5 + 5) * dx[direction] * 2;
                p.nY += (ITEMS_RollRandomNumber(seed) % 5 + 5) * dy[direction] * 2;
                if (widePlacement(op->pRoom, r, p, 3, 3)) { found = true; break; }
            }
            // Exhaustion returns through the RNG gate before the failed-position
            // gate; it neither advances base nor consumes the monster seed.
            if (found) {
                ++base;
                spawn(p.nX, p.nY);
            }
        }
    }
}

// 1.13c D2Game+0x2FA90
void populateRack(D2GameStrc* game, D2ActiveRoomStrc* room,
                   const D2DrlgCoordsStrc& r, int32_t kind)
{
    constexpr int32_t ids[4] = {104,105,106,107};
    if (kind >= 2) return;
    const uint32_t orientation = ITEMS_RollRandomNumber(&game->pObjectControl->pSeed) & 1;
    const int32_t objectId = ids[orientation + kind * 2];
    auto* txt = DATATBLS_GetObjectsTxtRecord(objectId);
    const int32_t sx = txt->dwSizeX, sy = txt->dwSizeY;
    if (!orientation) {
        const uint32_t end = r.nSubtileX + r.nSubtileWidth - sx;
        for (uint32_t x = r.nSubtileX + sx; x < end; x += sx * 2) {
            D2CoordStrc p{static_cast<int32_t>(x), r.nSubtileY + 1};
            if (widePlacement(room, r, p, sx, sy)) {
                if (!room) fatal();
                SUNIT_AllocUnitData(2, objectId, p.nX, p.nY, game, room, 1, 0, 0);
            }
        }
    } else {
        const uint32_t end = r.nSubtileY + r.nSubtileHeight - sy;
        for (uint32_t y = r.nSubtileY + sy; y < end; y += sy * 2) {
            D2CoordStrc p{r.nSubtileX + 1, static_cast<int32_t>(y)};
            if (widePlacement(room, r, p, sx, sy)) {
                if (!room) fatal();
                SUNIT_AllocUnitData(2, objectId, p.nX, p.nY, game, room, 1, 0, 0);
            }
        }
    }
}
} // namespace d2game_wave3_1
