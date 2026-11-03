// Callee seam for the D2Client light/ambient batch (0xA9A20-0xAA190): every call out of a port is one `call` with the register and
// stack-word values retail passes (unused ones 0). A module address is passed as 0xB2000000|rva, a pointer to a caller local as its
// host address. The game binds `call` to the matching function; the FN probe binds it to HookEnv (logs the call, returns the script).
#pragma once
#include <cstdint>
struct D2cA9Env {
    void* ctx;
    uint32_t (*call)(void* ctx, uint32_t rva, uint32_t eax, uint32_t ecx, uint32_t edx, uint32_t esi, uint32_t s0, uint32_t s1, uint32_t s2, uint32_t s3, uint32_t s4);
};
inline uint32_t d2c_hp(const void* p) { return (uint32_t)(uintptr_t)p; }
