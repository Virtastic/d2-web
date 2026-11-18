// 1.13c D2Client+0x3F500 -- constructor of the object whose vtable is PTR_FUN_6fb8675c.
#include <cstdint>
typedef uint32_t u32;
// ponytail: vtable VAs and the sub-object ctor (D2Client+0x3F3D0, UNMAPPED) are injected; no vector yet.
struct Ctor3F500Env {
  u32 vtBase, vtDerived;                 // 6fb85048, 6fb8675c
  u32 (*alloc)(u32 size, u32 line);      // Storm#401(0x54, "", -1, 0)
  u32 (*sub)(u32 mem, u32 p2, u32 p3, u32 p4, u32 p5); // D2Client+0x3F3D0 (mem,p2,p3,p4,p5) -- disasm: the 4 stack args are forwarded
};
// stdcall, ret 0x14: (self, p2, p3, p4, p5); returns self in EAX
u32 d2client_ctor_3f500(u32 *self, u32 p2, u32 p3, u32 p4, u32 p5, const Ctor3F500Env &e) {
  self[0] = e.vtBase;
  self[0] = e.vtDerived;
  u32 m = e.alloc(0x54, 0);
  self[2] = m ? e.sub(m, p2, p3, p4, p5) : 0;
  return (u32)(uintptr_t)self;
}
