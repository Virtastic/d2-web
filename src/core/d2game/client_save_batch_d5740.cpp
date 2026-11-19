// Prior art: the 1.10f source Clients, SUnit, SCmd and PlrSave2; contracts checked against
// the cached 1.13c functions. These entry points preserve explicit register inputs.
#include <cassert>
#include <cstdint>
#include <vector>
#include <D2Skills.h>
#include <D2Inventory.h>
#include <Units/Units.h>
#include <GAME/Clients.h>
#include <GAME/SCmd.h>
#include <UNIT/SUnit.h>
#include <D2PacketDef.h>

// Stand-alone body of D2Game 0x49D70 (the marker is on the play-path CLIENTS_SetActNo)
void D2GAME_49D70_SetAct(D2ClientStrc* client, uint8_t act)
{
    assert(client && act < 5);
    client->nAct = act;
}

// Stand-alone body of D2Game 0x49AE0 (the marker is on the play-path CLIENTS_SetCreateTime)
void D2GAME_49AE0_SetCreateTime(D2ClientStrc* client, uint32_t time)
{
    assert(client);
    client->nCreateTime = time;
}

// 1.13c D2Game+0xEB5D0
void D2GAME_EB5D0_DecodeSkill(const uint16_t* saved, uint32_t* owner,
                              uint32_t* skill, uint32_t* flags)
{
    if (saved[0] == UINT16_MAX) {
        *skill = UINT32_MAX;
        *flags = 0;
        *owner = UINT32_MAX;
        return;
    }
    *skill = saved[0] & 0xfff;
    *flags = saved[0] >> 15;
    *owner = saved[1] ? saved[1] : UINT32_MAX;
}

// engine entry for D2Game EB8B0 (retail-layout port with injectable callees: d2game_eb8b0.cpp)
uint32_t D2GAME_EB8B0_InventoryItemId(D2UnitStrc* unit, int32_t index)
{
    if (!unit->pInventory) return UINT32_MAX;
    auto* node = INVENTORY_GetFirstItem(unit->pInventory);
    int32_t position = 1;
    while (node) {
        auto* item = INVENTORY_UnitIsItem(node);
        node = INVENTORY_GetNextItem(node);
        if (position == index) return item ? item->dwUnitId : UINT32_MAX;
        ++position;
    }
    return UINT32_MAX;
}

// 1.13c D2Game+0x8C530: the per-player callback, the 10-byte 0x8E to the player's client (player data +0x9C).
static void __fastcall SendCorpseAssignment(D2GameStrc*, D2UnitStrc* player, void* packet)
{
    D2GAME_PACKETS_SendPacket_6FC3C710(SUNIT_GetClientFromPlayer(player, __FILE__, __LINE__), packet, 10);
}

// 1.13c D2Game+0x8CF80: {0x8E, kind, player id, corpse id}, 10 bytes, to every living player's client (0xDFC10, callback 0x8C530).
void D2GAME_8CF80_CorpseAssignment(D2GameStrc* game, int32_t playerId,
                                  int32_t corpseId, uint8_t assign)
{
    D2GSPacketSrv8E packet{};
    static_assert(sizeof(packet) == 10);
    packet.nHeader = 0x8e;
    packet.nType = assign;
    packet.dwPlayerGUID = playerId;
    packet.dwCorpseGUID = corpseId;
    SUNIT_IterateLivingPlayers(game, SendCorpseAssignment, &packet);
}

// 1.13c D2Game+0x8B5A0
// The body is sub_6FC3DCA0 (game/D2Game/GAME/SCmd.cpp); this name stays for the monspawn driver.
void D2GAME_8B5A0_SendSkills(D2ClientStrc* client, D2UnitStrc* unit)
{
    sub_6FC3DCA0(client, unit);
}
