// 1.13c D2Game+0xCC550 (EBX = packet bytes, ESI = unit, stack = client): client packet gate + dispatch.
// Retail handler table (0x6FD1A078, 8-byte stride) and Fog ordinals are seam calls so a replay can run the gate on blobs.
#include <cstdint>
#include <emscripten.h>

// ids: 1 Ordinal_10494() -> bool, 2 Ordinal_10055() -> word (sync time), 3 handler(opcode) ptr (0 = none), 4 call handler(opcode, client) -> result
struct D2GCC550Seam { uint32_t (*call)(uint32_t id, uint32_t a, uint32_t b); };

// unitMode = *(unit+0x10) (read only when unit != 0); stateWord = &client->+0x1dc4
static uint32_t GAME_CC550(const D2GCC550Seam& s, const uint8_t* pkt, uint32_t unit, uint32_t unitMode, uint32_t client, uint32_t* stateWord)
{
    const uint32_t op = pkt[0];
    if (op == 0 || op > 0x66 || !s.call(3, op, 0)) return 3;
    bool run;
    switch (op) {
    case 0x14: case 0x15: case 0x3C: case 0x43: case 0x66: run = true; break;
    case 0x41: if (!unit) return 0; run = unitMode == 0x11; break;
    default: run = !s.call(1, 0, 0) && unit && unitMode != 0 && unitMode != 0x11; break; // 0x20/0x26 decompile to the same test
    }
    if (!run) return 0;
    if (*stateWord > 1) *stateWord = s.call(2, 0, 0);
    return s.call(4, op, client); // retail: IsBadCodePtr -> log + _exit(-1) on a bad handler, not reproducible
}
