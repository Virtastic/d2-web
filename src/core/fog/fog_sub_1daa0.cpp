// 1.13c Fog+0x1DAA0 (ordinal 10100): nested defer-scope counter, on the retail layout. depth = DAT 0x9AF60, pending = DAT 0x9AF64 (a 32-bit
// node address); node: +0x3c handle, +0x44 cur, +0x48 flags, +0x4c saved, +0x50 next. apply = Storm#282, fatal = the unrecoverable-error exit.
#include <cstdint>
#include <cstring>

extern "C" uint32_t fog_eax_1daa0 = 0;   // retail's stale EAX at the ret (the function is void)

static inline uint32_t rd(uintptr_t a) { uint32_t v; std::memcpy(&v, (const void*)a, 4); return v; }
static inline void wr(uintptr_t a, uint32_t v) { std::memcpy((void*)a, &v, 4); }

// 1.13c Fog+0x1DAA0
extern "C"
void fog_sub_1daa0(int32_t enter, uint32_t* depth, uint32_t* pending,
                   void (*apply)(uint32_t handle, uint32_t cur), void (*fatal)()) {
    const uint32_t old = *depth;
    fog_eax_1daa0 = old;
    if (enter) { *depth = old + 1; fog_eax_1daa0 = old + 1; return; }
    if ((int32_t)old <= 0) { fatal(); return; }   // FogError(6, "", 0x216, "Unrecoverable internal error %08x"), _exit(-1)
    if ((*depth = old - 1) != 0) return;
    for (uintptr_t n = *pending; n;) {
        const uint32_t cur = rd(n + 0x44);
        const uint32_t next = rd(n + 0x50);
        fog_eax_1daa0 = cur;
        if (cur != rd(n + 0x4c)) { apply(rd(n + 0x3c), cur); fog_eax_1daa0 = 0; }
        wr(n + 0x48, rd(n + 0x48) & 0xfffffff7u);
        wr(n + 0x4c, 0);
        wr(n + 0x50, 0);
        n = next;
    }
    *pending = 0;
}
