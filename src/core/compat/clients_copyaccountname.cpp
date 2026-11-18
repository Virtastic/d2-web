// CLIENTS_CopyAccountNameToBuffer (1.13c D2Game+0x4690): copies the client's account name into the
// caller's buffer, bounded at 50 bytes and NUL-terminated (Clients.cpp itself is not linked).
#include <D2Common.h>
#include <Storm.h>
#include <GAME/Clients.h>

void __fastcall CLIENTS_CopyAccountNameToBuffer(D2ClientStrc* pClient, char* szAccount)
{
    SStrCopy(szAccount, pClient->szAccount, 50u);
}
