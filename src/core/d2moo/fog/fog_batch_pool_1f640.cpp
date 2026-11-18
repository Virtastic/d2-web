// src/core/fog/fog_batch_pool_1f640.cpp -- four 1.13c Fog.dll alpha-lane rows from the cached
// Ghidra decompiles: the comma-grouped 64-bit decimal formatter (+0x1F640), the event-wait thunk
// (+0xCFD0), and the two pool-header accessors (+0x1B7E0 set, +0x1B790 get).
// Not done: +0x1AE90/+0x1B830/+0x1BCB0 (pool create/destroy/trim need the VirtualAlloc-backed
// pool arena), +0xF2F0/+0xF3C0/+0xF6E0/+0xE7E0/+0xD820 (task-queue teardown over unmapped
// ordinals and the fatal-error path).
#include <cstdint>
#include <Windows.h>
#include <emscripten.h>

extern "C" void fog_sub_11690(uint32_t* section);   // critical-section validator

// DAT_6ff9af94: the default pool header used when the caller passes NULL.
static uint32_t g_fogDefaultPool[0x5f3];

// 1.13c Fog+0x1F640
// Format a signed 64-bit value as decimal with ',' between thousand groups into out (capacity
// min(cap,32) including the NUL). Zero formats as the empty string. If the digits and commas do
// not fit, out is filled with cap-1 '*' and terminated.
extern "C" EMSCRIPTEN_KEEPALIVE char* fog_sub_1f640(char* out, uint32_t cap, uint32_t lo, uint32_t hi) {
    if (cap > 0x20) cap = 0x20;
    int64_t v = static_cast<int64_t>((static_cast<uint64_t>(hi) << 32) | lo);
    char buf[32];
    char* cur = buf;
    int run = 0;
    while (v != 0) {
        if (cur >= buf + (cap - 1)) {
            for (uint32_t i = 0; i < cap - 1; ++i) out[i] = '*';
            out[cap - 1] = '\0';
            return out;
        }
        int64_t q = v / 10;
        *cur++ = static_cast<char>('0' + (v - q * 10));
        v = q;
        if (++run == 3) {
            if (v == 0) break;
            *cur++ = ',';
            run = 0;
        }
    }
    char* o = out;
    while (cur > buf) *o++ = *--cur;
    *o = '\0';
    return out;
}

// 1.13c Fog+0xCFD0
// Wait on the event handle at dword 0x2F5 of the object unless its first dword is set (then 0).
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t fog_sub_cfd0(const uint32_t* obj, uint32_t ms,
                                                      uint32_t (*wait)(uint32_t handle, uint32_t ms)) {
    if (obj[0] != 0) return 0;
    return wait(obj[0x2f5], ms);
}

// 1.13c Fog+0x1B7E0
// Store param_2 in the pool header's first dword under its lock (header critical section at +4).
// No-op if the pool is uninitialised (dword 7 == 0). NULL selects the default pool.
extern "C" EMSCRIPTEN_KEEPALIVE void fog_sub_1b7e0(uint32_t* pool, uint32_t value) {
    if (!pool) pool = g_fogDefaultPool;
    if (pool[7] == 0) return;
    auto* cs = reinterpret_cast<LPCRITICAL_SECTION>(pool + 1);
    fog_sub_11690(reinterpret_cast<uint32_t*>(cs));
    EnterCriticalSection(cs);
    pool[0] = value;
    fog_sub_11690(reinterpret_cast<uint32_t*>(cs));
    LeaveCriticalSection(cs);
}

// 1.13c Fog+0x1B790
// Read dword 0x5EA of the pool header under its lock. NULL selects the default pool.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t fog_sub_1b790(uint32_t* pool) {
    if (!pool) pool = g_fogDefaultPool;
    auto* cs = reinterpret_cast<LPCRITICAL_SECTION>(pool + 1);
    fog_sub_11690(reinterpret_cast<uint32_t*>(cs));
    EnterCriticalSection(cs);
    uint32_t v = pool[0x5ea];
    fog_sub_11690(reinterpret_cast<uint32_t*>(cs));
    LeaveCriticalSection(cs);
    return v;
}
