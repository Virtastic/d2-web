// D2Common 1.13c batch: stat-list callback dispatch (guest-layout port: pointers are 32-bit retail addresses).
#include <cstdint>
#include <cstring>
#include <emscripten.h>

namespace {
inline uint32_t Rd32(uint32_t a) { uint32_t v; std::memcpy(&v, reinterpret_cast<const void*>(static_cast<uintptr_t>(a)), 4); return v; }
}

// The three callees with their register operands explicit: 0x38160 (EAX = list+0x50, EDI = key << 16),
// 0x382B0 (EAX = list+0x24, EBX = key) and the callback (ECX = unit, EDX = key, stack: value, id).
struct D2Common388E0Env {
    int32_t (*direct)(uint32_t pDirect, uint32_t keyShifted);
    int32_t (*index)(uint32_t pFull, uint32_t key);
    void (*callback)(uint32_t unit, uint32_t key, uint32_t value, uint32_t id);
};

// 1.13c D2Common+0x388E0 (#10771): stdcall(unit, id, key, callback). The third word is the key the lookups
// take (the 1.10f-style "unused" slot is not unused in 1.13c); id is only handed to the callback.
extern "C" void D2Common_388E0_StatCallback(uint32_t unit, uint32_t id, uint32_t key, const D2Common388E0Env* env)
{
    if (!unit) return;
    const uint32_t list = Rd32(unit + 0x5c);
    if (!list) return;
    if (static_cast<int32_t>(Rd32(list + 0x10)) >= 0) return;
    if (env->direct(list + 0x50, key << 16) != -1) return;
    const int32_t i = env->index(list + 0x24, key);
    if (i < 0) return;
    const uint32_t entry = Rd32(list + 0x24) + static_cast<uint32_t>(i) * 8;
    if (entry) env->callback(unit, key, Rd32(entry + 4), id);
}
