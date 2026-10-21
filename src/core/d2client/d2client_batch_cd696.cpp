// D2Client 0xCD696..0xCD78B batch: SEH unwind thunks. Each forwards a frame slot (ebp+off) to a cleanup callee.
#include <cstdint>
namespace d2client_batch_cd696 {
using P = uintptr_t;
struct Env {
 uint32_t (*rd32)(P);
 void (*f1000)(uint32_t);   // FUN_6fab1000
 void (*f2210)(uint32_t);   // FUN_6fab2210
 void (*f3060)();           // FUN_6fab3060
};
// 1.13c D2Client+0xCD696
void Unwind_CD696(const Env& e, P ebp) { e.f1000(e.rd32(ebp - 0x684)); }
// 1.13c D2Client+0xCD6AE
void Unwind_CD6AE(const Env& e, P ebp) { e.f1000(e.rd32(ebp - 0x21c)); }
// 1.13c D2Client+0xCD6BC
void Unwind_CD6BC(const Env& e, P ebp) { e.f1000(e.rd32(ebp - 0x218)); }
// 1.13c D2Client+0xCD6CA
void Unwind_CD6CA(const Env& e, P ebp) { e.f1000(e.rd32(ebp - 0x21c)); }
// 1.13c D2Client+0xCD6D8
void Unwind_CD6D8(const Env& e, P ebp) { e.f1000(e.rd32(ebp - 0x218)); }
// 1.13c D2Client+0xCD6F0
void Unwind_CD6F0(const Env& e) { e.f3060(); }
// 1.13c D2Client+0xCD6FB
void Unwind_CD6FB(const Env& e) { e.f3060(); }
// 1.13c D2Client+0xCD710
void Unwind_CD710(const Env& e, P ebp) { e.f2210(e.rd32(ebp + 4)); }
// 1.13c D2Client+0xCD750
void Unwind_CD750(const Env& e) { e.f3060(); }
// 1.13c D2Client+0xCD775
void Unwind_CD775(const Env& e) { e.f3060(); }
// 1.13c D2Client+0xCD780
void Unwind_CD780(const Env& e) { e.f3060(); }
// 1.13c D2Client+0xCD78B
void Unwind_CD78B(const Env& e, P ebp) { e.f2210(e.rd32(ebp + 0xc)); }
}
