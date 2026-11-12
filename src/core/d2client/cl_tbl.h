// Shared pieces of the D2Client handlers reached through its pointer tables (one file per handler). Module globals are
// read through the Env at their retail addresses; byte and word globals are a read-modify-write of the dword there.
#pragma once
#include "cl_cmd.h"
namespace clt {
using clc::G;
using clc::S;
using clc::kBase;
inline uint32_t G8(const cl::Env& e, uint32_t rva) { return e.R(kBase + rva) & 0xffu; }
inline uint32_t G16(const cl::Env& e, uint32_t rva) { return e.R(kBase + rva) & 0xffffu; }
inline void S8(const cl::Env& e, uint32_t rva, uint32_t v) { e.W(kBase + rva, (e.R(kBase + rva) & ~0xffu) | (v & 0xffu)); }
inline void S16(const cl::Env& e, uint32_t rva, uint32_t v) { e.W(kBase + rva, (e.R(kBase + rva) & ~0xffffu) | (v & 0xffffu)); }
// the assertion path: source line through the reporter pair, then exit(-1)
inline void assertAt(const cl::Env& e, uint32_t line) {
    const uint32_t r = e.H(0xbf92, line);
    e.H(0xbf80, 0x6FB8359Fu, r, line);
    e.H(0x3664, 0xFFFFFFFFu);
}
// n bytes through the send routine (EBX = length, stack word 0 = the bytes)
inline uint32_t send(const cl::Env& e, const uint8_t* b, uint32_t n) {
    return e.H(0x143e0, (uint32_t)(uintptr_t)b, 0, 0, 0, 0, 0, 0, 0, n, 0);
}
// the nine-byte message {0x31, the dword at 0x11971D, the word at 0x11966F widened}
inline uint32_t send31(const cl::Env& e) {
    const uint32_t a = G(e, 0x11971D), w = G16(e, 0x11966F);
    const uint8_t b[12] = {0x31, (uint8_t)a, (uint8_t)(a >> 8), (uint8_t)(a >> 16), (uint8_t)(a >> 24), (uint8_t)w, (uint8_t)(w >> 8), 0, 0, 0, 0, 0};
    return send(e, b, 9);
}
// drops the open dialog whose handle sits at rva
inline void closeDlg(const cl::Env& e, uint32_t rva) {
    if (G(e, rva)) { e.H(0xa3d20); S(e, rva, 0); }
}
// a unit's stored level, capped at 0x4D (the cap is written back through the two setters)
inline uint32_t cappedLevel(const cl::Env& e, uint32_t u) {
    uint32_t n = e.H(0xc430, u);
    if ((int32_t)n >= 0x4e) { n = 0x4d; e.H(0xc4a2, u, n); e.H(0xc2c8, u, n); }
    return n;
}
// the closing three path calls: mode 10, the level, then the commit (path, unit, 0)
inline uint32_t pathCommit(const cl::Env& e, uint32_t path, uint32_t u, uint32_t n) {
    e.H(0xc38e, path, 10);
    e.H(0xc3b8, path, n);
    return e.H(0xc376, path, u, 0);
}
// the speech-menu reply state: marks the reply pending with its continuation
inline void replyPending(const cl::Env& e, uint32_t cont) {
    S8(e, 0x119661, 1);
    S(e, 0x11BF94, cont);
    S(e, 0x11BF98, 0);
}
// byte and word stores into unit / record memory (the dword there is read, changed and written back)
inline void M8(const cl::Env& e, uint32_t a, uint32_t v) { e.W(a, (e.R(a) & ~0xffu) | (v & 0xffu)); }
inline void M16(const cl::Env& e, uint32_t a, uint32_t v) { e.W(a, (e.R(a) & ~0xffffu) | (v & 0xffffu)); }
inline uint32_t P16(const cl::Env& e, uint32_t a) { return e.R(a) & 0xffffu; }
// the client unit lookup: EAX = id & 0x7F, ECX = id, EDX = the type's bucket row, stack word = type
inline uint32_t findUnit(const cl::Env& e, uint32_t type, uint32_t id) {
    return e.H(0xa4e20, type, 0, 0, 0, 0, id & 0x7f, id, kBase + 0x10a608 + (type << 9));
}
// the inline bucket walk of one unit type: the chain at table[id & 0x7F], linked through +0xE4, matched on the id at +0xC
inline uint32_t bucketWalk(const cl::Env& e, uint32_t table, uint32_t id) {
    uint32_t u = G(e, table + 4 * (id & 0x7f));
    while (u && e.R(u + 0xc) != id) u = e.R(u + 0xe4);
    return u;
}
// a wrapper's forwarding method: the inner object (+8 of the wrapper) goes in ECX to the entry at slot of its method table
inline uint32_t innerCall(const cl::Env& e, uint32_t self, uint32_t slot) {
    const uint32_t in = e.R(self + 8);
    return e.H(e.R(e.R(in) + slot) & 0xFFFFFFu, 0, 0, 0, 0, 0, 0, in);
}
}  // namespace clt
