// 1.13c D2Game+0x2A850 (stdcall ret 4: client id; EAX = the join record): decides whether a join record is acceptable. A record whose
// +0x11 byte is set is refused while the callback table (D2Game+0x111C3C) is non-null. The player name at +0x15 and the account at +0x01
// must each be shorter than 16 bytes, the locale byte +0x2D at most 13, and the flags dword at +0x27 must have bit 1 or 2 set. The client's
// game name is then fetched by 0x498B0 (stack: client id, EBX = a 16-byte local buffer; the buffer is never read here) and the player name
// (EDI = record+0x15) is looked up by 0x497C0 (stack: the same buffer). The result is class byte +0x12 < 7.
#include "d2game_gamelist.h"

struct D2G2A850Env {
    uint32_t callbackTable;                                     // D2Game+0x111C3C
    uint32_t (*clientGameName)(uint32_t nClientId, uint8_t* pBuf);        // 0x498B0
    uint32_t (*findPlayerName)(const char* pName, uint8_t* pBuf);         // 0x497C0, EDI = name
};

static bool fits15(const uint8_t* p)
{
    for (int i = 0; i < 16; ++i) if (p[i] == 0) return true;
    return false;
}

// 1.13c D2Game+0x2A850
bool D2Game_2A850(const uint8_t* pRec, uint32_t nClientId, const D2G2A850Env* pEnv)
{
    const uint8_t nClass = pRec[0x12];
    const uint32_t nFlags = d2gl::rd32(pRec, 0x27);
    if (pEnv->callbackTable != 0 && pRec[0x11] != 0) return false;
    if (!fits15(pRec + 0x15)) return false;
    if (pRec[0x2D] > 0x0D) return false;
    if (!fits15(pRec + 0x01)) return false;
    if ((nFlags & 6) == 0) return false;
    uint8_t buf[16];
    if (pEnv->clientGameName(nClientId, buf) == 0) return false;
    if (pEnv->findPlayerName(reinterpret_cast<const char*>(pRec) + 0x15, buf) == 0) return false;
    return nClass < 7;
}
