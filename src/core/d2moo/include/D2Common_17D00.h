// D2Common 0x17D00 / 0x17EB0: grid path search over the outdoor cell map (DrlgVer.cpp vertex chains).
#pragma once
#include <cstdint>
// The 16-byte turn table the search nodes point into (node+0x18); retail's copy is at D2Common+0x8CAB8.
extern "C" const uint8_t* D2Common_17D00_TurnTable();
extern "C" uint32_t D2Common_17D00_SearchPath(uint32_t pPool, int32_t nLimit, uint32_t pLevel, uint32_t pTarget);

// 0x17EB0 callee seam: the Fog pool allocator (Ordinal 10045 through the 0x9222 thunk; ECX = pool, EDX = size,
// stack file, line, 0). The port zeroes the vertex itself, as retail does after the call.
struct D2Common17EB0Env { uint32_t (*alloc)(uint32_t pMemPool, uint32_t nSize, uint32_t nLine); };
extern "C" uint32_t D2Common_17EB0_BuildVertexPath(int32_t nIndex, uint32_t pLevel, const D2Common17EB0Env* env);
