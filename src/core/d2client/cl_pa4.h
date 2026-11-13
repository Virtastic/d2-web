// Shared pieces of the D2Client panel message handlers, quest and NPC callbacks and the skill / state functions
// (one file per function). Module globals are read through the Env at their retail addresses; byte and word globals
// are a read-modify-write of the dword there. Each port also models the value the function leaves in EAX.
#pragma once
#include <initializer_list>
#include "cl_cmd.h"
namespace pa4 {
using clc::G;
using clc::S;
using clc::kBase;
inline uint32_t G8(const cl::Env& e, uint32_t rva) { return e.R(kBase + rva) & 0xffu; }
inline uint32_t G16(const cl::Env& e, uint32_t rva) { return e.R(kBase + rva) & 0xffffu; }
inline void S8(const cl::Env& e, uint32_t rva, uint32_t v) { e.W(kBase + rva, (e.R(kBase + rva) & ~0xffu) | (v & 0xffu)); }
inline void S16(const cl::Env& e, uint32_t rva, uint32_t v) { e.W(kBase + rva, (e.R(kBase + rva) & ~0xffffu) | (v & 0xffffu)); }
inline uint32_t P16(const cl::Env& e, uint32_t a) { return e.R(a) & 0xffffu; }
inline void M8(const cl::Env& e, uint32_t a, uint32_t v) { e.W(a, (e.R(a) & ~0xffu) | (v & 0xffu)); }
inline void M16(const cl::Env& e, uint32_t a, uint32_t v) { e.W(a, (e.R(a) & ~0xffffu) | (v & 0xffffu)); }
// a message handler's common end: handled = 1, result = 0, then the shared message tail (its answer is returned)
inline uint32_t fin(const cl::Env& e, uint32_t msg) {
    e.W(msg + 0x18, 1);
    e.W(msg + 0x1c, 0);
    return e.H(0xbe84, msg);
}
// the four mouse-button latches
inline void clearButtons(const cl::Env& e) {
    S(e, 0x11c3dc, 0);
    S(e, 0x11c3e0, 0);
    S(e, 0x11c3f8, 0);
    S(e, 0x11c3fc, 0);
}
// a unit's map coordinates through the two client helpers (ECX = unit)
inline uint32_t ux(const cl::Env& e, uint32_t u) { return e.H(0x1630, 0, 0, 0, 0, 0, 0, u); }
inline uint32_t uy(const cl::Env& e, uint32_t u) { return e.H(0x1660, 0, 0, 0, 0, 0, 0, u); }
// the client missile / overlay spawner: EDX = owner, stack (class, x, y, 0, 1)
inline uint32_t spawn(const cl::Env& e, uint32_t owner, uint32_t cls, uint32_t x, uint32_t y) {
    return e.H(0xaff10, cls, x, y, 0, 1, 0, 0, owner);
}
// the skills table row of an id, 0 when the id is outside the table
inline uint32_t skRow(const cl::Env& e, uint32_t id) {
    if ((int32_t)id < 0 || (int32_t)id >= (int32_t)e.R(e.dt + 0xba0)) return 0;
    return e.R(e.dt + 0xb98) + id * 0x23c;
}
// the missiles table row of an id, 0 when the id is outside the table
inline uint32_t misRow(const cl::Env& e, uint32_t id) {
    if ((int32_t)id < 0 || (int32_t)id >= (int32_t)e.R(e.dt + 0xb6c)) return 0;
    return e.R(e.dt + 0xb64) + id * 0x1a4;
}
// stack words 4.. of the call just made (the call log keeps four); a module address is logged by its offset
inline void xw(const cl::Env& e, uint32_t rva, std::initializer_list<uint32_t> w) {
    uint32_t b[9];
    uint32_t n = 0;
    for (uint32_t v : w) b[n++] = (v >= kBase && v < kBase + 0x140000u) ? (0xB2000000u | (v - kBase)) : v;
    e.Xw(rva, b, n);
}
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
// a callback held in a record: called through its slot
inline uint32_t slot(const cl::Env& e, uint32_t fp, uint32_t a = 0, uint32_t c = 0, uint32_t d = 0) {
    return e.H(fp & 0xFFFFFFu, 0, 0, 0, 0, 0, a, c, d);
}
// fills one menu record of the NPC menu table: entry count, the handler of its last entry, that entry's string id
inline uint32_t menuRec(const cl::Env& e, uint32_t rec, uint32_t n, uint32_t fn, uint32_t str) {
    S(e, rec, n);
    S(e, rec + 4 * n + 6, fn);
    S16(e, rec + 2 * n, str);
    return 1;
}
}  // namespace pa4
