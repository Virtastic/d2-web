// 1.13c Fog+0x0E910 -- hash-table lookup by key under a per-bucket lock, then dispatch the found entry.
#include <cstdint>
#include <cstring>

namespace {
// Host seams: Win32 critical sections and the three Fog callees.
void (*g_lockInit)(void* cs) = nullptr;   // Fog+0x11690
void (*g_enter)(void* cs) = nullptr;      // EnterCriticalSection
void (*g_leave)(void* cs) = nullptr;      // LeaveCriticalSection
void (*g_after)() = nullptr;              // Fog+0xE550
void (*g_dispatch)(uint32_t v, uint32_t a, uint32_t b) = nullptr;  // Fog+0xDF00
}

// tbl is the retail EAX: +0xC mask, +0x14 bucket heads (u32*), +0x1C lock array (0x18 each), +0xBD0 enabled.
// Entry: +0 key, +4 next, +0x18 value.
void fog_sub_E910(const uint8_t* tbl, uint32_t key, uint32_t a, uint32_t b) {
    auto u32 = [&](const uint8_t* p) { uint32_t v; std::memcpy(&v, p, 4); return v; };
    if (!u32(tbl + 0xBD0)) return;
    uint32_t idx = u32(tbl + 0xC) & key;
    uint8_t* cs = const_cast<uint8_t*>(reinterpret_cast<const uint8_t*>(static_cast<uintptr_t>(u32(tbl + 0x1C)))) + idx * 0x18;
    if (g_lockInit) g_lockInit(cs);
    if (g_enter) g_enter(cs);
    uint32_t val = 0;
    const uint8_t* e = reinterpret_cast<const uint8_t*>(static_cast<uintptr_t>(
        u32(reinterpret_cast<const uint8_t*>(static_cast<uintptr_t>(u32(tbl + 0x14))) + idx * 4)));
    for (; e; e = reinterpret_cast<const uint8_t*>(static_cast<uintptr_t>(u32(e + 4))))
        if (u32(e) == key) { val = u32(e + 0x18); break; }
    if (g_leave) g_leave(cs);
    if (g_after) g_after();
    if (val && g_dispatch) g_dispatch(val, a, b);
}
