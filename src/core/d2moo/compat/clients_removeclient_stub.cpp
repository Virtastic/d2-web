// Weak trapping stub for CLIENTS_RemoveClientFromGame: Clients.cpp is not linked, and Game.cpp
// references it. The main link tolerates the undefined symbol; the prove link (probes pull Game.o
// in) does not. Same for sub_6FC3C690 (SCmd.cpp is not linked). Never executed on the FN path; the real port is d2game_4b7f0.cpp.
#include <D2Common.h>
#include <GAME/Clients.h>
#include <GAME/SCmd.h>
#include <GAME/CCmd.h>
#include <GAME/Task.h>
#include <Server.h>

__attribute__((weak)) void __fastcall CLIENTS_RemoveClientFromGame(D2GameStrc*, int32_t, BOOL)
{
    __builtin_trap();
}

__attribute__((weak)) void __fastcall sub_6FC3C690(int32_t)
{
    __builtin_trap();
}

__attribute__((weak)) int32_t __fastcall CCMD_IsStringZeroTerminated(const char* pData, int32_t nDataSize)
{
    __builtin_trap();
}

__attribute__((weak)) int32_t __fastcall CLIENT_GetSaveHeader_6FC34350(D2ClientStrc* pClient)
{
    __builtin_trap();
}

__attribute__((weak)) uint32_t __fastcall CLIENT_IsSaveLoaded(D2ClientStrc* pClient)
{
    __builtin_trap();
}

__attribute__((weak)) int32_t __fastcall CLIENTS_AddPlayerToGame(D2ClientStrc* pClient, D2GameStrc* pGame, int32_t a3, D2ActiveRoomStrc* pRoomArg, int32_t nXArg, int32_t nYArg)
{
    __builtin_trap();
}

__attribute__((weak)) void __fastcall CLIENT_SetSaveLoaded_6FC34390(D2ClientStrc* pClient)
{
    __builtin_trap();
}

__attribute__((weak)) int32_t __fastcall CLIENTS_GetClientIdByName(const char* szName)
{
    __builtin_trap();
}

__attribute__((weak)) int32_t __fastcall CLIENTS_IsInGame(D2GameStrc* pGame, int32_t nClientId)
{
    __builtin_trap();
}

__attribute__((weak)) D2PacketDataStrc* __fastcall CLIENTS_PacketDataList_GetHead(D2ClientStrc* pClient)
{
    __builtin_trap();
}

__attribute__((weak)) D2PacketDataStrc* __fastcall CLIENTS_PacketDataList_PopHead(D2ClientStrc* pClient)
{
    __builtin_trap();
}

__attribute__((weak)) void __fastcall CLIENTS_PacketDataList_Reset(D2ClientStrc* pClient, D2PacketDataStrc* pSinglePacketData)
{
    __builtin_trap();
}

__attribute__((weak)) void __fastcall CLIENTS_RefreshUnitsUpdateList(D2ClientStrc* pClient, uint32_t nUpdateSize)
{
    __builtin_trap();
}

__attribute__((weak)) void __fastcall CLIENTS_SetGameData(D2GameStrc* pGame)
{
    __builtin_trap();
}

__attribute__((weak)) BOOL __fastcall D2Game_10042(D2TaskStrc* pTask, int nTaskType, D2LinkStrc* pPrevTaskBalanceLink)
{
    __builtin_trap();
}

__attribute__((weak)) void __fastcall D2GAME_PACKETS_SendPacket0x01_6FC3C7C0(D2ClientStrc* pClient, char nHeader, D2GameStrc* pGame)
{
    __builtin_trap();
}

__attribute__((weak)) void __fastcall D2GAME_PACKETS_SendPacket0x5C_6FC3E090(D2ClientStrc* pClient, DWORD dwUnitId)
{
    __builtin_trap();
}

__attribute__((weak)) void __stdcall D2NET_10016(int32_t nClientId)
{
    __builtin_trap();
}

__attribute__((weak)) void __stdcall D2NET_10019(D2NET_Unk_Callback pfCallback)
{
    __builtin_trap();
}

__attribute__((weak)) void __stdcall SERVER_GetIpAddressStringFromClientId(int32_t nClientId, char* szBuffer, int32_t nBufferSize)
{
    __builtin_trap();
}

__attribute__((weak)) int32_t __stdcall SERVER_ReadFromMessageList0(uint8_t* pBuffer, int32_t nBufferSize)
{
    __builtin_trap();
}

__attribute__((weak)) int32_t __stdcall SERVER_ReadFromMessageList1(uint8_t* pBuffer, int32_t nBufferSize)
{
    __builtin_trap();
}

__attribute__((weak)) int32_t __stdcall SERVER_ReadFromMessageList2(uint8_t* pBuffer, int32_t nBufferSize)
{
    __builtin_trap();
}

__attribute__((weak)) void __stdcall SERVER_SetClientGameGUID(int32_t nClientId, int32_t nGameGUID)
{
    __builtin_trap();
}

__attribute__((weak)) int32_t __stdcall SERVER_WSAGetLastError()
{
    __builtin_trap();
}

__attribute__((weak)) void __fastcall sub_6FC31EF0(D2ClientStrc* pClient, D2UnitStrc* pPlayer, D2GameStrc* pGame, D2ActiveRoomStrc* pRoomArg, int32_t nXArg, int32_t nYArg)
{
    __builtin_trap();
}

__attribute__((weak)) int32_t __fastcall sub_6FC3C640(int32_t nClientId, int16_t nGameId, int16_t nClientCount, const char* szGameName)
{
    __builtin_trap();
}

__attribute__((weak)) void __fastcall sub_6FC3C6B0(int32_t nClientId)
{
    __builtin_trap();
}

__attribute__((weak)) void __fastcall sub_6FC3C6D0(int32_t nClientId, uint32_t nErrorCode)
{
    __builtin_trap();
}

__attribute__((weak)) void __stdcall TASK_FreeAllQueueSlots()
{
    __builtin_trap();
}

__attribute__((weak)) void __fastcall D2GAME_PACKETS_SendPacket0x8B_6FC3F810(D2ClientStrc*, DWORD, BYTE)
{
    __builtin_trap();
}

__attribute__((weak)) void __fastcall D2GAME_PACKETS_SendPacket0x8C_6FC3F8B0(D2UnitStrc*, DWORD, DWORD, WORD)
{
    __builtin_trap();
}

// The party-relations broadcast after a join (PartyScreen.cpp, Party.cpp). The packet-handler probes pull the
// party screen into the prove link, which then failed on this missing definition and dropped every probe it took.
__attribute__((weak)) void __fastcall sub_6FC3F8F0(D2GameStrc*, D2UnitStrc*)
{
    __builtin_trap();
}

// The relation-state packet between two players (PlayerList.cpp). A probe that pulls the player list into the
// prove link failed on this missing definition and was dropped before it ran.
__attribute__((weak)) void __fastcall sub_6FC3F840(D2GameStrc*, int32_t, int32_t, int16_t)
{
    __builtin_trap();
}
