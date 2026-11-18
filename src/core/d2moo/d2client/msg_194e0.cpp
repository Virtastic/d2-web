// 1.13c D2Client+0x194E0: the unit-item sync message handler.
// EAX = p, a 10-byte message: +0 the message kind, +1 "has a unit", +2 the player unit id, +6 a second
// unit id. `hash` is the type-0 unit table (128 buckets; the id sits at unit+0xC, the chain link at
// unit+0xE4) and `gbl[0]` is pLocalPlayer. Every callee goes through the host in retail's call order,
// which returns its eax; the host is also handed exactly the register and stack words the descriptor
// logs (0 where retail's value is not part of the call).
#include "client_host.h"

static const uint32_t kItemCpp = 0xB2000000u | 0xD4964u;   // "..\\Source\\D2Client\\UNIT\\Item.cpp"
static const uint32_t kIndex = 0x10A608u;                  // the unit hash table global (module RVA)

extern "C" void d2_client_194e0(D2cHost h, const uint8_t* msg, const uint32_t* hash, const uint32_t* gbl) {
    auto go = [&](uint32_t rva, uint32_t eax, uint32_t ecx, uint32_t edx, uint32_t ebx, uint32_t esi,
                  uint32_t s0, uint32_t s1, uint32_t s2, uint32_t s3) {
        D2cCall k = { rva, eax, ecx, edx, ebx, esi, { s0, s1, s2, s3 }, { 0, 0, 0, 0 } };
        return h(&k); };
    auto look = [&](uint32_t id) -> const uint32_t* {   // bucket id&0x7F, then the +0xE4 chain
        const uint32_t* u = *(const uint32_t* const*)((const uint8_t*)hash + (id & 0x7F) * 4);
        while (u && u[3] != id) u = *(const uint32_t* const*)((const uint8_t*)u + 0xE4);
        return u; };
    // The Fog assert + _exit(-1) sites (0xBF92/0xBF80/0x3664, codes 0x5A/0xABA/0xAC8/0xAC9) are fatal in
    // retail; no input of the vector reaches one, so ours returns where retail would abort.
    if (!msg[1]) return;
    const uint32_t id1 = *(const uint32_t*)(msg + 2), id2 = *(const uint32_t*)(msg + 6);
    const uint32_t* u = look(id1);
    if (!u) return;
    if (u[0]) return;                                        // fatal 0x5A
    if (!(u[0x31] & 0x10000u) && u[4] && u[4] != 0x11) {
        go(0x2A6C0, u[4], id1 & 0x7F, 0, (uint32_t)(uintptr_t)msg, (uint32_t)(uintptr_t)u, 0, 0, 0, 0);
    }
    const uint32_t ub = (uint32_t)(uintptr_t)u;
    if (u == (const uint32_t*)(uintptr_t)gbl[0]) {           // the local player: sync the second unit first
        const uint32_t* u2 = look(id2);
        if (!u2) return;
        if (u2[0]) return;                                  // fatal 0x5A
        const uint32_t path2 = u2[0xb], u2b = (uint32_t)(uintptr_t)u2;
        go(0xC3AC, 0, 0, 0, path2, ub, path2, 0, 0, 0);
        go(0xCDD2, 0, 0, 0, path2, ub, u2b, 5, 0, 0);
        uint32_t r1660 = go(0x1660, 0, ub, 0, path2, ub, 0, 0, 0, 0);
        uint32_t r1630 = go(0x1630, 0, ub, 0, path2, ub, 0, 0, 0, 0);
        uint32_t r1f0 = go(0xC1F0, 0, 0, 0, path2, ub, ub, r1630, r1660, 0);
        if (!go(0xC6A0, 0, 0, 0, path2, ub, path2, u2b, r1f0, 0)) return;   // fatal 0xABA
        uint32_t r2d4 = go(0xC2D4, 0, 0, 0, path2, ub, u[0xb], 0, 0, 0);
        go(0xC3C4, 0, path2, 0, path2, ub, path2, r2d4, 0, 0);
    }
    const uint32_t head = u[0x18];                           // unit+0x60: the item list head
    uint32_t item = go(0xC4F0, 0, 0, 0, head, ub, head, 0, 0, 0);
    while (item) {
        const uint32_t next = go(0xC568, 0, 0, 0, head, ub, item, 0, 0, 0);
        if (go(0xC994, 0, 0, 0, head, ub, item, 0, 0, 0) == 3) {
            const uint32_t it2 = go(0xC526, 0, 0, 0, head, ub, item, 0, 0, 0);
            go(0xCA2A, 0, 0, 0, head, ub, head, it2, 0, 0);
            go(0x184D0, 0, 0, 0, head, ub, ub, it2, 0, 0);
            go(0x69A90, 0, ub, 0, head, ub, 0, 0, 0, 0);
            const uint32_t r6d0 = go(0xC6D0, 0, 0, 0, head, ub, head, it2, 0, 0);
            if (!r6d0 || r6d0 != it2) return;                // fatal 0xAC8 / 0xAC9
            if (!u[0]) go(0xCA0C, 0, 0, 0, head, ub, head, 0, 0, 0);
            if (!go(0xC1A8, 0, 0, 0, head, ub, it2, 0x100, 0x96, kItemCpp)) {
                go(0xC544, 0, 0, 0, head, ub, ub, it2, 0, 0);
            }
            const uint32_t byte = go(0xCA54, 0, 0, 0, head, ub, it2, 0, 0, 0) & 0xFF;
            go(0xCAA8, 0, 0, 0, head, ub, head, byte, 0, 0);
            go(0x6A220, ub, 0, 0, head, ub, 0, 0, 0, 0);
            uint32_t type, id;
            if (it2) { type = *(const uint32_t*)(uintptr_t)it2; id = *(const uint32_t*)(uintptr_t)(it2 + 0xC); }
            else { type = 6; id = 0xFFFFFFFFu; }
            go(0xA6EF0, 0, 0xB2000000u | (kIndex + type * 0x200u), id & 0x7F, head, ub, id, type, 0, 0);
        }
        item = next;
    }
}
