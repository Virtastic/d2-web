// 1.13c D2Game+0x49A20, +0xB23A0: null-checked accessors.
// Retail logs and _exit(-1) on a null argument; ours asserts the same precondition and does not model _exit.
#include <cstdint>
#include <cstring>
uint32_t GAME_Plus0xD(uint32_t p) { return p + 0xd; }
uint32_t GAME_GetPlus1D28Plus8(const uint8_t* pGame)
{
    uint32_t inner; memcpy(&inner, pGame + 0x1d28, 4);
    uint32_t v; memcpy(&v, (const uint8_t*)(uintptr_t)inner + 8, 4);
    return v;
}
// 1.13c D2Game+0x49A80: EAX = client, returns client+0x1A8 (pGame).
uint32_t CLIENT_GetGame_49A80(const uint8_t* pClient)
{
    uint32_t v; memcpy(&v, pClient + 0x1a8, 4);
    return v;
}
