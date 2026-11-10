// The three position-and-vitals packets the per-client life update (1.13c D2Game+0xCC030) chooses between, and the
// client's last-sent record it compares against. The bit writers are the 1.13c functions; these are the 1.10f-named
// entry points the vendored update calls.
#include <cstdint>

#include <GAME/Clients.h>
#include <GAME/SCmd.h>

extern "C" void d2game_8b8c0(D2ClientStrc* client, uint32_t hp, uint32_t mana, uint32_t stamina, uint32_t potLife, uint32_t potMana, uint32_t x, uint32_t y, uint32_t dx, uint32_t dy);
extern "C" void d2game_8b7f0(D2ClientStrc* client, uint32_t hp, uint32_t mana, uint32_t stamina, uint32_t x, uint32_t y, uint32_t dx, uint32_t dy);
extern "C" void d2game_8b750(D2ClientStrc* client, uint32_t stamina, uint32_t x, uint32_t y, uint32_t dx, uint32_t dy);

// 1.13c D2Game+0x8B8C0 (0x18: life, mana, stamina, both potion percentages, position)
void __fastcall sub_6FC3D890(D2ClientStrc* pClient, uint16_t a2, uint16_t a3, uint16_t a4, uint8_t a5, uint8_t a6, uint16_t a7, uint16_t a8, uint8_t a9, uint8_t a10)
{
    d2game_8b8c0(pClient, a2, a3, a4, a5, a6, a7, a8, a9, a10);
}

// 1.13c D2Game+0x8B7F0 (0x95: life, mana, stamina, position)
void __fastcall sub_6FC3D9A0(D2ClientStrc* pClient, uint16_t a2, uint16_t a3, uint16_t a4, uint16_t a5, uint16_t a6, uint8_t a7, uint8_t a8)
{
    d2game_8b7f0(pClient, a2, a3, a4, a5, a6, a7, a8);
}

// 1.13c D2Game+0x8B750 (0x96: stamina, position)
void __fastcall D2GAME_PACKETS_SendPacket0x96_WalkVerify_6FC3DA90(D2ClientStrc* pClient, uint16_t a2, uint16_t a3, uint16_t a4, uint8_t a5, uint8_t a6)
{
    d2game_8b750(pClient, a2, a3, a4, a5, a6);
}

D2ClientPlayerDataStrc* __fastcall CLIENTS_GetClientPlayerData(D2ClientStrc* pClient)
{
    return pClient ? &pClient->clientPlayerData : nullptr;
}
