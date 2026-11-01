// 1.13c D2Game+0x2A220 (163 B)
// function list name GAME_ResolveGameNameConflict is wrong (that is 0x2BC90); this is the game-id allocator
// (1.10f GAME_AllocGameId): Ordinal 10050 enters the 0x6FD30BE8 lock, then a 1..0x400 round-robin
// cursor (low word of 0x6FD2E350) is advanced from its old value; the first slot of the table at
// 0x6FD30C04 that is 0 is marked -1 (taken), the cursor is left one past it, the lock is left and the
// slot index is returned. A full lap with no free slot returns 0 with the cursor left one past the start.
// Callers 0x2C2D0 / 0x2C570 (game create).
#include <cstdint>
#include <Windows.h>

static uint16_t g_gameIdCursor = 1;   // 0x6FD2E350 low word
static int32_t  g_gameIdSlots[0x401]; // 0x6FD30C04, indexed by cursor value (0 free, -1 taken)
static CRITICAL_SECTION g_gameIdLock; // 0x6FD30BE8

uint32_t GAME_AllocGameId_2A220()
{
    EnterCriticalSection(&g_gameIdLock);                    // Ordinal 10050
    const uint16_t start = g_gameIdCursor;
    uint16_t cur = start;
    uint16_t next = cur + 1; if (next > 0x400) next = 1;
    g_gameIdCursor = next;
    while (g_gameIdSlots[cur] != 0) {
        const uint16_t advanced = next + 1 > 0x400 ? 1 : next + 1;
        g_gameIdCursor = advanced;
        if (next == start) { LeaveCriticalSection(&g_gameIdLock); return 0; }
        cur = next; next = advanced;
    }
    g_gameIdSlots[cur] = -1;
    LeaveCriticalSection(&g_gameIdLock);
    return cur;
}
