// 1.13c D2Game+0x9C620 -- batch lane: marker + implementation only. unit is EBX (register arg lost by
// Ghidra), game is param_1. Retail 32-bit layouts are explicit uint32 offsets; callees are env pointers.
#include <cstdint>
#include <cstring>

struct Env9C620 {
  void (*fatal)(int line);            // Ordinal_10265 + Ordinal_10024 + _exit(-1)
  void (*ord10920)(void* unit);       // GUESS args
  int16_t (*fn_9beb0)(void* unit);    // FUN_6fcbbeb0
  void* (*fn_9bd00)(int16_t id);      // FUN_6fcbbd00
  void (*fn_8e2a0)(void* unit);       // FUN_6fcae2a0 (unit in EDI)
};

static uint32_t r32(const void* b, uint32_t o) { uint32_t v; memcpy(&v, (const char*)b + o, 4); return v; }

// 1.13c D2Game+0x9C620
void D2Game_9C620(const Env9C620& e, void* game, void* unit) {
  if (unit && r32(unit, 0) == 0) e.ord10920(unit);
  if (!(unit && r32(unit, 0x80) != 0 && r32(unit, 0) == 0)) return;
  int16_t pid = e.fn_9beb0(unit);
  if (pid == -1) return;
  void* party = e.fn_9bd00(pid);
  if (!party) return;
  for (uint32_t n = r32(party, 4); n; n = r32((void*)(uintptr_t)n, 4)) {
    if (!game) e.fatal(0x2d3);
    const uint32_t id = r32((void*)(uintptr_t)n, 0);
    for (uint32_t u = r32(game, 0x1120 + (id & 0x7f) * 4); u; u = r32((void*)(uintptr_t)u, 0xe4)) {
      if (r32((void*)(uintptr_t)u, 0xc) == id) {
        if ((void*)(uintptr_t)u != unit) e.fn_8e2a0((void*)(uintptr_t)u);
        break;
      }
    }
  }
}
