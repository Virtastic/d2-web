// 1.13c D2Client+0x3F500 -- constructor of the object whose vtable is PTR_FUN_6fb8675c.
#include <cstdint>
typedef uint32_t u32;
// ponytail: vtable VAs and the sub-object ctor (D2Client+0x3F3D0, UNMAPPED) are injected; no vector yet.
struct Ctor3F500Env {
  u32 vtBase, vtDerived;                 // 6fb85048, 6fb8675c
  u32 (*alloc)(u32 size, u32 line);      // Storm#401(0x54, "", -1, 0)
  u32 (*sub)(u32 mem, u32 self);         // D2Client+0x3F3D0 (mem,...,self)
};
u32 d2client_ctor_3f500(u32 *self, const Ctor3F500Env &e) {
  self[0] = e.vtBase;
  self[0] = e.vtDerived;
  u32 m = e.alloc(0x54, 0);
  self[2] = m ? e.sub(m, (u32)(uintptr_t)self) : 0;
  return (u32)(uintptr_t)self;
}
