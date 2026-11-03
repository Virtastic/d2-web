// Fog's client-memory free on retail's register convention (ordinal 10043): ECX = the block, EDX = the source file,
// stack: line, one unused word (ret 8). The block goes into a stack slot whose address the pool free takes in ECX, with
// EAX = 0 (no pool) and the file and line on the stack. Always answers 1.
#include <cstdint>

struct Fog1CCF0Env { void (*poolFree)(uint32_t pool, uint32_t* slot, uint32_t file, uint32_t line); };

uint32_t Fog_FreeClientMemory32(uint32_t block, uint32_t file, uint32_t line, const Fog1CCF0Env& env)
{
    uint32_t slot = block;
    env.poolFree(0, &slot, file, line);
    return 1;
}
