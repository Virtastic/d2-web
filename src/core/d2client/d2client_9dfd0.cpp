// Relation-bit sender (bit 1). Retail: EAX = unit, ECX = pass-through for the 0x9DCC0 call, EBX := 1; plain ret.
// No unit: nothing. Otherwise 0x9DCC0(unit, ecx, ebx=1); the player id is *(DAT 0x11BBFC)+0xC (-1 when no player); the party entry
// 0x9D630(edx = id) has a relation header at +0x34 whose list ([header]+0: node {id, flags, next}) is walked for the unit id
// (skipped when the player id equals the unit id). The 7-byte packet {0x5D, 1, bit2 as 0/1, unit id LE} goes to 0x143E0.
// Guest pointers here are the retail 32-bit words (the probe's blobs live in the low 4 GiB).
// 1.13c D2Client+0x9DFD0
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2c9dfd0Env {
    uintptr_t player;                                              // *(D2Client+0x11BBFC): player record (id at +0xC) or 0
    void (*c9dcc0)(uintptr_t unit, uint32_t ecx, uint32_t ebx);    // 0x9DCC0
    uintptr_t (*c9d630)(uint32_t pid);                             // 0x9D630 (EDX = player id)
    void (*c143e0)(const uint8_t* pkt, uint32_t n);                // 0x143E0
};
static uint32_t rd32_9dfd0(uintptr_t p) { uint32_t v; memcpy(&v, (const void*)p, 4); return v; }

extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_9dfd0(uintptr_t unit, uint32_t ecx, const D2c9dfd0Env* env) {
    if (!unit) return;
    env->c9dcc0(unit, ecx, 1);
    const uint32_t pid = env->player ? rd32_9dfd0(env->player + 0xC) : 0xFFFFFFFFu;
    const uint32_t uid = rd32_9dfd0(unit + 0x10);
    uint32_t bit = 0;
    const uintptr_t ent = env->c9d630(pid);
    if (ent) {
        const uintptr_t hdr = rd32_9dfd0(ent + 0x34);
        if (hdr && pid != uid)
            for (uintptr_t n = rd32_9dfd0(hdr); n; n = rd32_9dfd0(n + 8))
                if (rd32_9dfd0(n) == uid) { bit = rd32_9dfd0(n + 4) & 1u; break; }
    }
    uint8_t pkt[7] = { 0x5D, 1, (uint8_t)(bit != 0) };
    memcpy(pkt + 3, &uid, 4);
    env->c143e0(pkt, 7);
}
