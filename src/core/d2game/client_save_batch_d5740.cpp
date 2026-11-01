// Prior art: D2MOO Clients, SUnit, SCmd and PlrSave2; contracts checked against
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

// 1.13c D2Game+0x49D70
void D2GAME_49D70_SetAct(D2ClientStrc* client, uint8_t act)
{
    assert(client && act < 5);
    client->nAct = act;
}

// 1.13c D2Game+0x49AE0
void D2GAME_49AE0_SetCreateTime(D2ClientStrc* client, uint32_t time)
{
    assert(client);
    client->nCreateTime = time;
}

// 1.13c D2Game+0xE0040
void D2GAME_E0040_SetPlayerClient(D2UnitStrc* player, D2ClientStrc* client)
{
    assert(player && player->dwUnitType == 0);
    UNITS_GetPlayerData(player)->pClient = client;
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

// 1.13c D2Game+0xEB8B0
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

static void __fastcall SendCorpseAssignment(D2GameStrc*, D2UnitStrc* player, void* packet)
{
    D2GAME_PACKETS_SendPacket_6FC3C710(SUNIT_GetClientFromPlayer(player, __FILE__, __LINE__), packet, 10);
}

// 1.13c D2Game+0x8CF80
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
void D2GAME_8B5A0_SendSkills(D2ClientStrc* client, D2UnitStrc* unit)
{
    assert(unit && unit->pSkills && client);
    uint32_t count = 0;
    for (auto* s = SKILLS_GetFirstSkillFromSkillList(unit->pSkills); s; s = SKILLS_GetNextSkill(s)) ++count;
    assert(count < 256);
    // The wire format is six header bytes followed by three bytes per skill.
    std::vector<uint8_t> packet(6 + 3 * count);
    packet[0] = 0x94;
    packet[1] = static_cast<uint8_t>(count);
    // retail reads the id from the client's player unit (client+0x1a8 -> +0xc), not from the argument
    const uint32_t id = *reinterpret_cast<uint32_t*>(*reinterpret_cast<uint8_t**>(reinterpret_cast<uint8_t*>(client) + 0x1a8) + 0xc);
    for (unsigned b = 0; b < 4; ++b) packet[2+b] = static_cast<uint8_t>(id >> (b*8));
    size_t offset = 6;
    for (auto* s = SKILLS_GetFirstSkillFromSkillList(unit->pSkills); s; s = SKILLS_GetNextSkill(s)) {
        const uint16_t skill = SKILLS_GetSkillIdFromSkill(s, __FILE__, __LINE__);
        packet[offset++] = static_cast<uint8_t>(skill);
        packet[offset++] = static_cast<uint8_t>(skill >> 8);
        packet[offset++] = static_cast<uint8_t>(SKILLS_GetSkillLevel(unit, s, 0));
    }
    D2GAME_PACKETS_SendPacket_6FC3C710(client, packet.data(), static_cast<int32_t>(packet.size()));
}
