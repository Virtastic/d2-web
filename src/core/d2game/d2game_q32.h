// Shared by the 1.13c quest-record ports (D2Game+0x60080-0x63270): retail 32-bit field walks over caller memory and the retail callees as one
// seam. A callee is called as X(rva, eax, ecx, edx, ebx, esi, stack words...) with retail argument placement; the play path maps each rva to the
// real routine, a FUNCTION-mode probe records it. Offsets are retail 1.13c offsets.
#pragma once
#include <cstdint>
#include <cstring>
namespace q32 {
inline uint8_t* P(uint32_t a) { return (uint8_t*)(uintptr_t)a; }
inline uint32_t rd32(uint32_t a, uint32_t o) { uint32_t v; std::memcpy(&v, P(a) + o, 4); return v; }
inline uint16_t rd16(uint32_t a, uint32_t o) { uint16_t v; std::memcpy(&v, P(a) + o, 2); return v; }
inline uint8_t rd8(uint32_t a, uint32_t o) { return P(a)[o]; }
inline void wr32(uint32_t a, uint32_t o, uint32_t v) { std::memcpy(P(a) + o, &v, 4); }
inline void wr16(uint32_t a, uint32_t o, uint16_t v) { std::memcpy(P(a) + o, &v, 2); }
inline void wr8(uint32_t a, uint32_t o, uint8_t v) { P(a)[o] = v; }
struct Call {
    uint32_t (*f)(void*, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);
    void* u;
    uint32_t operator()(uint32_t rva, uint32_t a = 0, uint32_t c = 0, uint32_t d = 0, uint32_t b = 0, uint32_t s = 0,
                        uint32_t s0 = 0, uint32_t s1 = 0, uint32_t s2 = 0, uint32_t s3 = 0) const { return f(u, rva, a, c, d, b, s, s0, s1, s2, s3); }
};
// First quest node of the given type: the dword at game+0x10F4 is the control; control[0] is the first node; node+0xF4 the next; node[0] the type. 0 if none.
inline uint32_t findQ(uint32_t game, int32_t type) {
    uint32_t ctl = rd32(game, 0x10f4);
    if (!ctl) return 0;
    for (uint32_t n = rd32(ctl, 0); n; n = rd32(n, 0xf4)) if ((int32_t)rd32(n, 0) == type) return n;
    return 0;
}
// D2Common ordinals the quest code uses (module thunks): #10920 player record, #10174 quest flag get, #10003 quest flag set.
constexpr uint32_t kO10920 = 0xA4C0, kO10174 = 0xA568, kO10003 = 0xA57A;
inline uint32_t questRec(const Call& X, uint32_t game, uint32_t unitOrClient) {
    uint32_t rec = X(kO10920, 0, 0, 0, 0, 0, unitOrClient);
    return rd32(rec, 0x10 + rd8(game, 0x6d) * 4);
}
inline int32_t qGet(const Call& X, uint32_t rec, uint32_t a, uint32_t b) { return (int32_t)X(kO10174, 0, 0, 0, 0, 0, rec, a, b); }
inline void qSet(const Call& X, uint32_t rec, uint32_t a, uint32_t b) { X(kO10003, 0, 0, 0, 0, 0, rec, a, b); }
// The 0x62700 family (quests 6, 4, 3, 2, 1): ECX = game, EDX = client, ret 4. Fires D2Game+0x354F0 (AL = quest, ECX = client, stack game).
// `al` is the quest number handed to 0x354F0 when it differs from the record's quest index (the act 5 handlers pass the index minus 4).
inline uint32_t questStartFlags(const Call& X, uint32_t game, uint32_t client, uint32_t q, uint32_t al) {
    uint32_t sk = questRec(X, game, client);
    int32_t last = qGet(X, sk, q, 0);
    bool fire = false, chk = true;
    if (last == 0) {
        last = qGet(X, sk, q, 0xf);
        if (last == 0) { fire = true; chk = false; }
    }
    if (chk) {
        last = qGet(X, sk, q, 0xd);
        if (last != 0) fire = true;
        else { last = qGet(X, sk, q, 0xe); if (last != 0) fire = true; }
    }
    if (fire) X(0x354F0, ((uint32_t)last & ~0xffu) | (al & 0xffu), client, 0, 0, 0, game);   // mov al, q keeps the last result's upper bytes
    return 0;
}
inline uint32_t questStartFlags(const Call& X, uint32_t game, uint32_t client, uint32_t q) { return questStartFlags(X, game, client, q, q); }
}  // namespace q32
