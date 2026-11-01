// src/core/d2game/d2game_gamelist.h -- the 1.13c D2Game game-list state the lock-family accessors share, and the helpers they use.
// Retail: slots[] is the dword table at D2Game+0x110C04 indexed by game id (1..0x400); slot 0 doubles as "the last game
// GAME_LockGame returned" (0x2BD70 stores it there); slots 1.. are the game GUIDs (0 free, -1 reserved). The game-list critical
// section is D2Game+0x110BE8, entered through Fog #10050 (thunk 0xA328, fastcall ecx = CS, edx = line 0x125).
// Records hold 32-bit pointers in retail; the host build is 64-bit, so pointer slots are read as dwords.
#pragma once
#include <cstdint>
#include <cstring>
#include <unistd.h>
#include <Windows.h>

namespace d2gl {

struct State { uint32_t slots[0x401]; };
inline State g_state;
inline CRITICAL_SECTION g_listLock;   // 0x6FD30BE8

constexpr uint32_t kGameLock = 0x18, kGameId = 0x28, kGameInitSeed = 0x7C, kGameClients = 0x8C, kGameUnitList = 0x1120;

inline uint32_t rd32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, static_cast<const uint8_t*>(p) + o, 4); return v; }
inline uint16_t rd16(const void* p, uint32_t o) { uint16_t v; std::memcpy(&v, static_cast<const uint8_t*>(p) + o, 2); return v; }
inline void* rdptr(const void* p, uint32_t o) { return reinterpret_cast<void*>(static_cast<uintptr_t>(rd32(p, o))); }

// Fog #10024/#10265 message + _exit(-1) at every assert site; only the outcome is modelled.
[[noreturn]] inline void fatal(uint32_t) { _exit(-1); }

// [slots + id*4]: a game id above the table reads past it in retail; the model reads 0.
inline uint32_t slotAt(uint32_t id) { return id <= 0x400 ? g_state.slots[id] : 0; }
// "0 and -1 are not games": the lookup keeps ESI = 0 for them.
inline uint32_t usableGuid(uint32_t g) { return (g == 0 || g == 0xFFFFFFFFu) ? 0 : g; }

// The game's own lock (+0x18), left with LeaveCriticalSection; a null lock is the 0x100F assert.
inline void leaveGameLock(void* pGame) {
    LPCRITICAL_SECTION p = static_cast<LPCRITICAL_SECTION>(rdptr(pGame, kGameLock));
    if (p == nullptr) fatal(0x100F);
    LeaveCriticalSection(p);
}

}  // namespace d2gl
