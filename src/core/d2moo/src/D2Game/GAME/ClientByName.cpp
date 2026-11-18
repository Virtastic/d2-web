#include "GAME/Game113c.h"
#include "GAME/Clients.h"
#include <Fog.h>
#include <Storm.h>
#include <Windows.h>
#include <test_seam.h>

// 1.13c D2Game 0x6FD303B8: the by-name client list, bucketed by the low byte of Fog #10137's CRC16 of the name (linked through pServerNextByName).
D2ClientStrc* gpClientListByName_6FD303B8[256];
static CRITICAL_SECTION gClientByNameLock_6FD30BD0;

// 1.13c D2Game+0x497C0
// EDI = name, stack = optional 16-byte output. Walks the name's bucket under the by-name lock and compares case-insensitively with the 16-byte client names; the first match has its stored name copied out (when an output was given) and answers 0, no match answers 1.
int32_t __fastcall CLIENTS_GetClientByName(const char* szName, char* szOutName)
{
    const uint8_t nBucket = (uint8_t)D2_CALL(FOG_ComputeStringCRC16, szName);
    D2_CALL(FOG_10050_EnterCriticalSection, &gClientByNameLock_6FD30BD0, 0xAE3);
    int32_t nResult = 1;
    for (D2ClientStrc* pClient = gpClientListByName_6FD303B8[nBucket]; pClient; pClient = pClient->pServerNextByName)
    {
        if (D2_CALL(SStrCmpI, pClient->szName, szName, 0x10) == 0)
        {
            if (szOutName)
            {
                D2_CALL(SStrCopy, szOutName, pClient->szName, 0x10);
            }
            nResult = 0;
            break;
        }
    }
    LeaveCriticalSection(&gClientByNameLock_6FD30BD0);
    return nResult;
}
