// Shared rules of the D2Client client-side skill "do" handlers (the handlers the skill-do table points at). Each is
// fastcall(ECX = unit, EDX = skill id) with one stack word (the skill level) and returns 1 when the skill went on,
// 0 when it did not. Callees go through Env::H with the registers and stack words the retail call site passes.
#pragma once
#include "cl_cmd.h"
namespace clsk {
// Skills-table row (stride 0x23C) by id, 0 when the id is out of range
inline uint32_t row(const cl::Env& e, int32_t sk) {
    if (sk < 0 || sk >= (int32_t)e.R(e.dt + 0xba0)) return 0;
    return e.R(e.dt + 0xb98) + (uint32_t)sk * 0x23c;
}
// a missile id inside the Missiles table
inline bool mis(const cl::Env& e, int32_t m) { return m >= 0 && m < (int32_t)e.R(e.dt + 0xb6c); }
// mark the unit as having acted (flag 0x40 of the word at +0xC4)
inline void acted(const cl::Env& e, uint32_t u) { if (u) e.W(u + 0xc4, e.R(u + 0xc4) | 0x40); }
// the missile launch with eight stack words (missile, four zeros, skill, level, zero), EAX = unit
inline uint32_t launch8(const cl::Env& e, uint32_t u, int32_t m, int32_t sk, uint32_t lvl) {
    e.Xs((uint32_t)sk, lvl);
    return e.H(0xb9a80, (uint32_t)m, 0, 0, 0, 0, u);
}
// the plain missile launch (missile, skill, level), EAX = unit
inline uint32_t launch3(const cl::Env& e, uint32_t u, int32_t m, int32_t sk, uint32_t lvl) {
    return e.H(0xa15c0, (uint32_t)m, (uint32_t)sk, lvl, 0, 0, u);
}
// The summon-style launch: the row's missile (+0xEA) plus a per-monster offset byte (monster units only, read from
// the unit's data block at +0x14), gated by the missile check; the row's flag byte picks the launch routine.
inline uint32_t offsetLaunch(const cl::Env& e, uint32_t u, int32_t sk, uint32_t lvl, uint32_t byteOff) {
    const uint32_t r = row(e, sk);
    if (!r) return 0;
    int32_t m = e.R16s(r + 0xea);
    if (!mis(e, m)) return 0;
    uint32_t add = 0;
    if (u && e.R(u) == 1 && e.R(u + 0x14)) add = e.R8(e.R(u + 0x14) + byteOff);
    m += (int32_t)add;
    if (m >= (int32_t)e.R(e.dt + 0xb6c)) return 0;
    if (!e.H(0x1580, 0, 0, 0, 0, 0, (uint32_t)m)) return 0;
    acted(e, u);
    if (e.R8(e.bm + 4) & e.R8(r + 4)) launch8(e, u, m, sk, lvl); else launch3(e, u, m, sk, lvl);
    return 1;
}
// The owner-gated launch shared by three handlers: owner slot, the row's count formula (+0x138), then the seven-word
// routine with the three trailing words that tell the handlers apart.
inline uint32_t ownerLaunch(const cl::Env& e, uint32_t u, int32_t sk, uint32_t lvl, uint32_t k, uint32_t w5, uint32_t w6) {
    const uint32_t r = row(e, sk);
    if (!r) return 0;
    const int32_t a = (int32_t)e.H(0x40550, u, 0, 0, 0, 0, (uint32_t)sk);
    if (!mis(e, a)) return 0;
    acted(e, u);
    const int32_t c = (int32_t)e.H(0xc316, u, e.R(r + 0x138), (uint32_t)sk, lvl);
    if (c <= 0) return 0;
    e.Xs(w5, w6);
    return e.H(0xc9a50, (uint32_t)c, (uint32_t)a, (uint32_t)sk, lvl, k, u);
}
// The item-stat pair two handlers hand on: the row's two words at +0x10C / +0x10E, each raised by (grade - 1) when
// the row's flag byte, state (+0x80) and item stat (+0x54) are all usable and the unit carries that state.
inline void gradePair(const cl::Env& e, uint32_t u, uint32_t r, int32_t& a, int32_t& b) {
    a = e.R16s(r + 0x10c); b = e.R16s(r + 0x10e);
    if (!(e.R8(e.bm + 8) & e.R8(r + 4))) return;
    const int32_t st = e.R16s(r + 0x80);
    if (st < 0 || st >= (int32_t)e.R(e.dt + 0xc4)) return;
    const int32_t it = e.R16s(r + 0x54);
    if (it < 0 || it >= (int32_t)e.R(e.dt + 0xbd4)) return;
    const uint32_t s = e.H(0xc1c0, u, (uint32_t)st);
    if (!s) return;
    int32_t v = (int32_t)e.H(0xc4b4, s, (uint32_t)it, 0);
    v = v > 1 ? (v >= 3 ? 3 : v) : 1;
    a += v - 1; b += v - 1;
}
}  // namespace clsk
