// D2Client 0xCD8E0..0xCDBB0 batch: SEH unwind thunks forwarding a frame slot to a cleanup callee.
#include <cstdint>
namespace d2client_batch_cd8e0 {
using P = uintptr_t;
struct Env {
 uint32_t (*rd32)(P);
 void (*f1fb0)(uint32_t);  // FUN_6fab1fb0
 void (*f1ed0)(uint32_t);  // FUN_6fab1ed0
 void (*f1e70)();          // FUN_6fab1e70
 void (*f2ec0)(uint32_t);  // FUN_6fab2ec0
 void (*f2190)();          // FUN_6fab2190
 void (*f12e0)();          // FUN_6fab12e0
};
// 1.13c D2Client+0xCD8E0
void Unwind_CD8E0(const Env& e, P ebp) { e.f1fb0(e.rd32(ebp + 4)); }
// 1.13c D2Client+0xCD900
void Unwind_CD900(const Env& e, P ebp) { e.f1ed0(e.rd32(ebp + 4)); }
// 1.13c D2Client+0xCD90A
void Unwind_CD90A(const Env& e) { e.f1e70(); }
// 1.13c D2Client+0xCD915
void Unwind_CD915(const Env& e, P ebp) { e.f1fb0(e.rd32(ebp + 4)); }
// 1.13c D2Client+0xCD930
void Unwind_CD930(const Env& e, P ebp) { e.f2ec0(e.rd32(ebp + 4) + 4); }
// 1.13c D2Client+0xCD945
void Unwind_CD945(const Env& e) { e.f2190(); }
// 1.13c D2Client+0xCD968
void Unwind_CD968(const Env& e, P ebp) { e.f2ec0(e.rd32(ebp + 4) + 0x28); }
// 1.13c D2Client+0xCD975
void Unwind_CD975(const Env& e, P ebp) { e.f2ec0(e.rd32(ebp + 4) + 0x38); }
// 1.13c D2Client+0xCD990
void Unwind_CD990(const Env& e, P ebp) { e.f1ed0(e.rd32(ebp + 4)); }
// 1.13c D2Client+0xCD99A
void Unwind_CD99A(const Env& e) { e.f1e70(); }
// 1.13c D2Client+0xCDBB0
void Fn_CDBB0(const Env& e) { for (int i = 0x20; i; --i) e.f12e0(); }
}
