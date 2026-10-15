// What Game.cpp needs from D2Net (the retail socket server) and from winmm/kernel32. D2Net is not
// vendored: the engine drives the sim in-process, so these are declarations the glue answers.
#pragma once
#include <Windows.h>
#include <atomic>
using D2NET_Unk_Callback = void (__stdcall*)(int32_t nClientId, const char* szFile, int32_t nLine);
void __stdcall D2NET_10019(D2NET_Unk_Callback pfCallback);
void __stdcall D2NET_10015(int32_t nClientId, const char* szFile, int32_t nLine);
void __stdcall D2NET_10016(int32_t nClientId);
uint32_t __stdcall D2NET_10006(int8_t a1, int32_t nClientId, void* pBuffer, uint32_t nBufferSize);
int32_t __stdcall SERVER_GetClientGameGUID(int32_t nClientId);
void __stdcall SERVER_SetClientGameGUID(int32_t nClientId, int32_t nGameGUID);
void __stdcall SERVER_GetIpAddressStringFromClientId(int32_t nClientId, char* szBuffer, int32_t nBufferSize);
int32_t __stdcall SERVER_WSAGetLastError();
int32_t __stdcall SERVER_ReadFromMessageList0(uint8_t* pBuffer, int32_t nBufferSize);
int32_t __stdcall SERVER_ReadFromMessageList1(uint8_t* pBuffer, int32_t nBufferSize);
int32_t __stdcall SERVER_ReadFromMessageList2(uint8_t* pBuffer, int32_t nBufferSize);
union LARGE_INTEGER { struct { DWORD LowPart; LONG HighPart; }; int64_t QuadPart; };
inline BOOL QueryPerformanceCounter(LARGE_INTEGER* p) { p->QuadPart = 0; return TRUE; }
inline BOOL QueryPerformanceFrequency(LARGE_INTEGER* p) { p->QuadPart = 0; return FALSE; }
inline void Sleep(DWORD) {}
inline DWORD timeGetTime() { return GetTickCount(); }
inline long InterlockedCompareExchange(volatile LONG* p, LONG, LONG) { return *p; }
inline long InterlockedIncrement(volatile LONG* p) { return ++*p; }
inline long InterlockedDecrement(volatile LONG* p) { return --*p; }
void SRegLoadValue(const char* szKey, const char* szValue, int32_t nFlags, DWORD* pValue);
// The engine's per-game pool is the process heap (Fog.h D2_ALLOC_POOL ignores it).
inline void FOG_CreateNewPoolSystem(void** pp, const char*, uint32_t, uint32_t) { *pp = nullptr; }
inline void FOG_DestroyMemoryPoolSystem(void*) {}
inline uint32_t FOG_GetMemoryUsage(void*) { return 0; }
