// 1.13c D2Game+0x2D790 (ESI = pClient, EDI = pGame, plain ret): while the game byte +0x6A is 1 or 2, keeps calling 0x2D140 (EAX = client,
// [esp+4] = game) as long as the client still holds a pending save blob (+0x3D4 bit 4 and +0x17C). A null client is the 0xB8A assert.
#include <cstdint>
#include <cstring>

struct D2Game2D140Env;
void D2Game_2D140(void* pClient, uint32_t nArg, const D2Game2D140Env* pEnv);

// 1.13c D2Game+0x2D790
void D2Game_2D790(void* pClient, void* pGame, const D2Game2D140Env* pEnv, void (*fatal)(uint32_t))
{
    const uint8_t nState = static_cast<const uint8_t*>(pGame)[0x6A];
    if (nState != 1 && nState != 2) return;
    if (pClient == nullptr) { fatal(0xB8A); return; }
    const uint8_t* c = static_cast<const uint8_t*>(pClient);
    for (;;)
    {
        uint32_t nBlob; std::memcpy(&nBlob, c + 0x17C, 4);
        if ((c[0x3D4] & 0x10) == 0 || nBlob == 0) return;
        D2Game_2D140(pClient, static_cast<uint32_t>(reinterpret_cast<uintptr_t>(pGame)), pEnv);
    }
}
