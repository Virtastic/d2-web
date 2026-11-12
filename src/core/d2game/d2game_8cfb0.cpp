// 1.13c D2Game+0x8CFB0: seven-byte packet {0x8D, unit id dword (-1 if none), party word} handed to the 0xDFC10 broadcast with the
// callback 0x6FCAC5F0 (EBX) and the caller's stack argument (EDI). EAX = unit. The party word is the 0xBBEB0 lookup when the
// unit is non-null, has a game (+0x80) and *unit == 0, else 0xFFFF.
#include <cstdint>
#include <cstring>
void D2Game_8CFB0_PartyBroadcast(void (*broadcast)(const void* packet, uint32_t edi), uint16_t (*partyId)(const void* unit),
                                 const uint8_t* unit, uint32_t edi)
{
    uint8_t b[8] = { 0x8D };
    uint32_t id = 0xFFFFFFFFu;
    if (unit) std::memcpy(&id, unit + 0xC, 4);
    std::memcpy(b + 1, &id, 4);
    uint16_t party = 0xFFFF;
    if (unit) {
        const uint8_t* game; std::memcpy(&game, unit + 0x80, sizeof game);
        uint32_t type; std::memcpy(&type, unit, 4);
        if (game && type == 0) party = partyId(unit);
    }
    std::memcpy(b + 5, &party, 2);
    broadcast(b, edi);
}

// The engine entry (1.10f sub_6FC3F8F0, declared in GAME/SCmd.h): the same packet, broadcast through
// SUNIT_IterateLivingPlayers (0xDFC10) with the 0x8C5F0 callback (sub_6FC3F930). EDI = pGame (the stack argument);
// the party word's 0x9BEB0 lookup reads the unit's own game (+0x80), as PARTY_GetPartyIdForUnitOwner(unit->pGame, unit).
#include <D2BasicTypes.h>
#include <Units/Units.h>
#include <GAME/SCmd.h>
#include <UNIT/SUnit.h>
#include <UNIT/Party.h>

void __fastcall sub_6FC3F8F0(D2GameStrc* pGame, D2UnitStrc* pPlayer)
{
    uint8_t b[8] = { 0x8D };
    const int32_t id = pPlayer ? (int32_t)pPlayer->dwUnitId : -1;
    int16_t party = -1;
    if (pPlayer && pPlayer->pGame && pPlayer->dwUnitType == UNIT_PLAYER)
    {
        party = PARTY_GetPartyIdForUnitOwner(pPlayer->pGame, pPlayer);
    }
    std::memcpy(b + 1, &id, 4);
    std::memcpy(b + 5, &party, 2);
    SUNIT_IterateLivingPlayers(pGame, sub_6FC3F930, b);
}
