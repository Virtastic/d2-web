#pragma once
#include <cstdint>
// Fog 10045 (ECX = pool, EDX = size; stack: file, line, 0): the pooled allocation every D2Common record goes through.
using D2FogPoolAlloc = uint32_t (*)(uint32_t nPool, uint32_t nSize, const char* szFile, int32_t nLine, int32_t nZero);
uint32_t D2Common_4E3F0_AllocSkillList(uint32_t nPool, D2FogPoolAlloc pfnAlloc);
uint32_t D2Common_67BD0_AllocWaypointData(uint32_t nPool, const uint16_t* pWaypointBit, D2FogPoolAlloc pfnAlloc);
