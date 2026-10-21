// D2Client 0xCD7A0..0xCD8CC batch: SEH unwind thunks. Each forwards a frame slot (ebp+off) to a cleanup callee.
#include <cstdint>
namespace d2client_batch_cd7a0 {
using P = uintptr_t;
struct Env {
 uint32_t (*rd32)(P);
 void (*f2580)(uint32_t);   // FUN_6fab2580
 void (*f2630)();           // FUN_6fab2630
 void (*f3060)();           // FUN_6fab3060
 void (*f2ec0)(uint32_t);   // FUN_6fab2ec0
 void (*f2190)();           // FUN_6fab2190
};
// 1.13c D2Client+0xCD7A0
void Unwind_CD7A0(const Env& e, P ebp) { e.f2580(e.rd32(ebp + 4)); }
// 1.13c D2Client+0xCD7F0
void Unwind_CD7F0(const Env& e) { e.f3060(); }
// 1.13c D2Client+0xCD7FB
void Unwind_CD7FB(const Env& e, P ebp) { e.f2580(e.rd32(ebp + 4)); }
// 1.13c D2Client+0xCD810
void Unwind_CD810(const Env& e) { e.f2630(); }
// 1.13c D2Client+0xCD81A
void Unwind_CD81A(const Env& e) { e.f3060(); }
// 1.13c D2Client+0xCD860
void Unwind_CD860(const Env& e, P ebp) { e.f2ec0(e.rd32(ebp - 0x14) + 4); }
// 1.13c D2Client+0xCD86D
void Unwind_CD86D(const Env& e) { e.f2190(); }
// 1.13c D2Client+0xCD882
void Unwind_CD882(const Env& e, P ebp) { e.f2580(e.rd32(ebp - 0x10)); }
// 1.13c D2Client+0xCD8A8
void Unwind_CD8A8(const Env& e, P ebp) { e.f2ec0(e.rd32(ebp + 4) + 0x28); }
// 1.13c D2Client+0xCD8B5
void Unwind_CD8B5(const Env& e, P ebp) { e.f2ec0(e.rd32(ebp + 4) + 0x38); }
// 1.13c D2Client+0xCD8C2
void Unwind_CD8C2(const Env& e, P ebp) { e.f2580(e.rd32(ebp - 0x10)); }
// 1.13c D2Client+0xCD8CC
void Unwind_CD8CC(const Env& e, P ebp) { e.f2580(e.rd32(ebp - 0x10)); }
}
