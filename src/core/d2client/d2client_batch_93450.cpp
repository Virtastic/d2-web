// D2Client 0xCD5D7..0xCD681 batch: SEH unwind-funclet thunks (env-seam). `ebp` is the unwound frame's EBP.
#include <cstdint>
namespace d2client_batch_93450 {
using P = uintptr_t;
struct Env {
 uint32_t (*rd32)(P); void (*wr32)(P, uint32_t);
 P guardFlags;                 // DAT_6fbd3708
 void (*f3060)(); void (*f1030)(P); void (*f1150)(P); void (*f1000)(P);
 void (*f4e630)();             // thunk_FUN_6fb4e630 (callee 0x19A0)
 void (*f1a40)();
};
// 1.13c D2Client+0xCD5D7
void Unwind_CD5D7(const Env& e) { e.wr32(e.guardFlags, e.rd32(e.guardFlags) & 0xfffffffeu); }
// 1.13c D2Client+0xCD5EF
void Unwind_CD5EF(const Env& e) { e.f3060(); }
// 1.13c D2Client+0xCD5FA
void Unwind_CD5FA(const Env& e, P ebp) { e.f1030(ebp - 0x178); }
// 1.13c D2Client+0xCD60C
void Unwind_CD60C(const Env& e) { e.f3060(); }
// 1.13c D2Client+0xCD617
void Unwind_CD617(const Env& e, P ebp) { e.f1150(e.rd32(ebp - 0x160)); }
// 1.13c D2Client+0xCD636
void Unwind_CD636(const Env& e, P ebp) { e.f1000(e.rd32(ebp - 0x10)); }
// 1.13c D2Client+0xCD64B
void Unwind_CD64B(const Env& e, P ebp) { e.f1000(e.rd32(ebp - 0x10)); }
// 1.13c D2Client+0xCD660
void Unwind_CD660(const Env& e) { e.f4e630(); }
// 1.13c D2Client+0xCD66B
void Unwind_CD66B(const Env& e) { e.f1a40(); }
// 1.13c D2Client+0xCD676
void Unwind_CD676(const Env& e) { e.f4e630(); }
// 1.13c D2Client+0xCD681
void Unwind_CD681(const Env& e) { e.f4e630(); }
}
