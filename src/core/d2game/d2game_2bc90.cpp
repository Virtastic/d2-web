// 1.13c D2Game+0x2BC90 (215 B, EAX = pGame, EDI = pName): gives the game a unique name. A name 0x2ABD0 reports free (non-zero) is copied to game+0x2A;
// otherwise the name is cut at 11 characters when fewer than 3 of the 16 bytes would remain, and "name~0", "name~1" ... are tried until one is free (the
// index reaching 100 is the 0x25C assert). 0x2ABD0 = [esp+4] name, ret 4. The copy is Fog/Storm #501 (bound 0x7FFFFFFF).
#include <cstdint>
#include <cstdio>
#include <cstring>

struct D2Game2BC90Env {
    uint32_t (*nameFree)(const char* pName);          // 0x2ABD0
    void (*copyName)(char* pDst, const char* pSrc);   // Storm #501
    void (*fatal)(uint32_t nLine);
};

// 1.13c D2Game+0x2BC90
void D2Game_2BC90(void* pGame, char* pName, const D2Game2BC90Env* pEnv)
{
    char* pDst = static_cast<char*>(pGame) + 0x2A;
    if (pEnv->nameFree(pName)) { pEnv->copyName(pDst, pName); return; }
    const uint32_t nLen = static_cast<uint32_t>(std::strlen(pName));
    if (15u - nLen < 3u) pName[0xB] = '\0';
    char aBuf[16];
    std::sprintf(aBuf, "%s~%d", pName, 0);
    int32_t nIdx = 1;
    if (!pEnv->nameFree(aBuf))
    {
        for (;;)
        {
            if (nIdx >= 100) { pEnv->fatal(0x25C); return; }
            std::sprintf(aBuf, "%s~%d", pName, nIdx);
            ++nIdx;
            if (pEnv->nameFree(aBuf)) break;
        }
        if (nIdx >= 100) { pEnv->fatal(0x25C); return; }
    }
    pEnv->copyName(pDst, aBuf);
}
