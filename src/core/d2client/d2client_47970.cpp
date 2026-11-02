// 1.13c D2Client+0x47970 (242 B): slot-addressed unit packet handler. EAX = packet {+1 byte slot, +2 dword unit id}.
// Transcribed from the disassembly (the decompile hides every register input below): slot table stride is 0x200 (shl edx,9),
// 0xA4E20 takes EAX = id&0x7F, ECX = id, EDX = &table[slot], stack = slot; 0x6B300 takes EAX = 0x48 and seven stack words;
// 0x88A70 takes EBX = a constant that names the arm (0x11FB / 0x11FF / 0xF8F); 0xA5360 takes EAX = module address 0x45E30.
#include <cstdint>
#include <cstring>
namespace d2client_47970 {
using U = uint32_t;
static U rd(const void* p, U o) { U v; std::memcpy(&v, static_cast<const uint8_t*>(p) + o, 4); return v; }
static const uint8_t* at(U a) { return reinterpret_cast<const uint8_t*>(static_cast<uintptr_t>(a)); }
struct Globals { U p973b; uint8_t cc33e; };   // RVA 0x11973B (dword), 0x11C33E (byte)
struct Services {
  virtual ~Services() = default;
  virtual U a4e20(U eax, U ecx, U edx, U slot) = 0;      // slot table row lookup (fastcall + 1 stack word)
  virtual U cur() = 0;                                   // 0x46150
  virtual void f88a70(U unit, U ebx) = 0;                // stdcall 4 words (unit,0,0,0)
  virtual void f6b300(U unit) = 0;                       // EAX = 0x48, words (unit,3,0,0,0,0,0)
  virtual U ord10174(U a, U b, U c) = 0;                 // thunk 0xC58C, stdcall 3
  virtual void a5360() = 0;                              // EAX = module address 0x45E30
};
// 1.13c D2Client+0x47970
void sub_47970(const uint8_t* pkt, Globals& g, Services& s) {
  const U slot = pkt[1], id = rd(pkt, 2);
  const U u = s.a4e20(id & 0x7f, id, slot * 0x200 + 0x10a608, slot);
  if (!u) return;
  if (rd(at(u), 4) == 0x216) {
    U ebx = 0x11ff;
    if (rd(at(u), 0) == 1) { const U p = rd(at(u), 0x14); if (p && rd(at(p), 0x3c) != 0xffffffffu) ebx = 0x11fb; }
    s.f88a70(u, ebx); return;
  }
  if (u == s.cur()) return;
  s.f6b300(u);
  if (rd(at(u), 4) != 0x14b) return;
  if (s.cur()) return;
  if (!g.p973b) return;
  if (s.ord10174(g.p973b, 0xc, 8)) return;
  if (s.ord10174(g.p973b, 0xc, 1)) return;
  g.cc33e = 0;
  s.a5360();
  if (g.cc33e == 0) s.f88a70(u, 0xf8f);
}
}
