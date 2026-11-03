// 1.13c Fog+0x1DB40 (ordinal 10099): set an async request's priority, on the retail layout (32-bit node addresses; +0x3c handle, +0x44 priority,
// +0x48 magic|flags, +0x4c saved priority, +0x50 next). depth = DAT 0x9AF60, head = DAT 0x9AF64. fatal = the unrecoverable-error exit (lines 0x1df/0x1e0/0x1e1).
#include <cstdint>
#include <cstring>

extern "C" uint32_t fog_eax_1db40 = 0;   // retail's stale EAX at the ret (void function)

static inline uint32_t rd(uintptr_t a) { uint32_t v; std::memcpy(&v, (const void*)a, 4); return v; }
static inline void wr(uintptr_t a, uint32_t v) { std::memcpy((void*)a, &v, 4); }

// 1.13c Fog+0x1DB40
extern "C"
void fog_1db40_retail(uintptr_t obj, int32_t prio, const uint32_t* depth, uint32_t* head,
                      void (*apply)(uint32_t handle, uint32_t prio), void (*fatal)(uint32_t line)) {
    if (!obj) { fatal(0x1df); return; }
    const uint32_t flags = rd(obj + 0x48);
    fog_eax_1db40 = flags;
    if ((flags & 0xfffffff0u) != 0x41737960u) { fatal(0x1e0); return; }
    if (prio > 0) { fatal(0x1e1); return; }
    if ((int32_t)*depth > 0) {
        if (flags & 8) {
            if (prio > (int32_t)rd(obj + 0x44)) wr(obj + 0x44, (uint32_t)prio);
            return;
        }
        wr(obj + 0x4c, rd(obj + 0x44));
        wr(obj + 0x44, (uint32_t)prio);
        wr(obj + 0x48, flags | 8);
        fog_eax_1db40 = *head;
        wr(obj + 0x50, *head);
        *head = (uint32_t)obj;
        return;
    }
    if (prio != (int32_t)rd(obj + 0x44)) {
        wr(obj + 0x44, (uint32_t)prio);
        apply(rd(obj + 0x3c), (uint32_t)prio);
        fog_eax_1db40 = 0;
    }
}
