// 1.13c D2Client+0xAD160 -- env-seam style; the unit table walk and the callees' registers follow the disassembly.
#include <cstdint>
#include <cstring>
#include "sub_ad160.h"
namespace d2client_ad160 {
static uint32_t r32(uintptr_t p) { uint32_t v; memcpy(&v, (const void*)p, 4); return v; }
static uintptr_t rdp(uintptr_t p) { return r32(p); }   // retail's pointers are 32-bit words
// Finds the unit with this id in the 128-slot table at 0x10AA08 (slot = id & 0x7f, chain through +0xE4); 0 if none.
static uintptr_t find(const Env& e, uint32_t id) {
 uintptr_t slot = (uintptr_t)e.ga(0x10AA08) + (uintptr_t)(id & 0x7f) * 4;
 for (uintptr_t u = rdp(slot); u; u = rdp(u + 0xE4)) if (r32(u + 0xC) == id) return u;
 return 0;
}
// 1.13c D2Client+0xAD160 (returns EAX: the 0x9D630 record, 0 on the paths that do not reach it; ecx = packet: u32 at +1 = key for 0x9D630, bytes +5..+0x14 = the 16-byte block, u32 at +0x15 and +0x19 = unit ids)
uintptr_t sub_AD160(const Env& e, uintptr_t pkt) {
 uint32_t key = r32(pkt + 1), a = r32(pkt + 0x15), b = r32(pkt + 0x19);
 uintptr_t u = find(e, a);
 if (u) { if (r32(u) != 2) { e.fatal(0x5a); return 0; } e.ord501((uint8_t*)(rdp(u + 0x14) + 0x28), (const uint8_t*)(pkt + 5), 0x10); }
 if (b != 0xffffffffu) {
  u = find(e, b);
  if (u) { if (r32(u) != 2) { e.fatal(0x5a); return 0; } e.ord501((uint8_t*)(rdp(u + 0x14) + 0x28), (const uint8_t*)(pkt + 5), 0x10); }
  uint8_t* r = e.f9D630(pkt, key);
  if (r) { memcpy(r + 0x3c, &a, 4); memcpy(r + 0x40, &b, 4); }
  return (uintptr_t)r;
 }
 return 0;
}
}
