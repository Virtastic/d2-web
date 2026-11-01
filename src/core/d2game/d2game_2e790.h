// Retail-layout (32-bit offset) ports of the 1.13c D2Game object/quest pool helpers. Every engine callee is a
// parameter: the Fog pool allocator (thunk 0xA2E6) and free (0xA2EC) take ECX = pool, EDX = size/block and the
// stack (file, line, 0); `file` is the source-name word the caller passes through unchanged.
#pragma once
#include <cstdint>
#include <cstring>

using D2GPoolAlloc = uint32_t (*)(uint32_t pool, uint32_t size, uint32_t file, uint32_t line, uint32_t zero);
using D2GPoolFree = void (*)(uint32_t pool, uint32_t block, uint32_t file, uint32_t line, uint32_t zero);

namespace d2g32 {
inline uint32_t rd32(uint32_t a) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
inline uint16_t rd16(uint32_t a) { uint16_t v; std::memcpy(&v, (const void*)(uintptr_t)a, 2); return v; }
inline uint8_t rd8(uint32_t a) { return *(const uint8_t*)(uintptr_t)a; }
inline void wr32(uint32_t a, uint32_t v) { std::memcpy((void*)(uintptr_t)a, &v, 4); }
inline void wr8(uint32_t a, uint8_t v) { *(uint8_t*)(uintptr_t)a = v; }
}

// 0x2E790: EDI = game (pool at +0x1C), EBX = timer-arg block.
void OBJECTS_FreeTimerArgs(uint32_t game, uint32_t timerArg, D2GPoolFree poolFree, uint32_t file);
